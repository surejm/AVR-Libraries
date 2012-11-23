/*
 * nec_ir.c
 *
 * Created: 2012-10-24 19:23:03
 *  Author: Hampus
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <UART/uart.h>
#include "nec_ir.h"


/************************************************************************
	Setup the micro counter
************************************************************************/
void setupMicroCounter()
{
	/*
		Prescale = 1;
		1/(8000000/1) = 0.125 us
		Count 80 steps = 80*0.125 = 10us
	*/
	TCCR2A = _BV(WGM21);
	OCR2A = 79;
}

/************************************************************************
	Start the micro counter
************************************************************************/
void startMicroCounter()
{
	TCCR2B = _BV(CS20);
	TIMSK2 = _BV(OCIE2A);
	sei();
}

/************************************************************************
	Stop the micro counter
************************************************************************/
void stopMicroCounter()
{
	TCCR2B = 0;
	_microCount = 0;
	TIMSK2 = 0;
}
/************************************************************************
	Interrupt routine for Timer 2 compare A match
************************************************************************/
ISR(TIMER2_COMPA_vect)
{
	// Will overflow after approx 71.58 minutes
	_microCount += 10;
}

/************************************************************************
	Setup the IR Receiver
************************************************************************/
void necIrSetup(void(*theManageIrDataFunc)(uint32_t))
{
	_irManageDataFunc = theManageIrDataFunc;
	setupMicroCounter();
	DDRD &= ~_BV(PORTD3);
	EICRA |= _BV(ISC10);
	EIMSK |= _BV(INT1);
	sei();
}

/************************************************************************
	Interrupt routine for external intterupt 1 (IR Receiver)
************************************************************************/
ISR(INT1_vect)
{
	uint8_t logicLevel = PIND & _BV(PORTD3);
	if (logicLevel) logicLevel = LOW;
	else logicLevel = HIGH;
	
	static uint8_t bitCount = 0;
	static uint32_t irData = 0;
	static uint8_t start = TRUE;
		
	// Change to high:
	if (logicLevel == HIGH)
	{
		// Check for second low level startbit
		if (start && _microCount > 4400 && _microCount < 4600)
		{
			start = FALSE;
			_microCount = 0;
		}
		// Check for first startbit
		else if (start)
		{
			startMicroCounter();
		}			
		// Done with collecting all 32 bits
		else if (bitCount == 32)
		{
			static uint32_t irDataCorrectEndian = 0;
			irDataCorrectEndian = 0;
			for (uint8_t i = 0; i < 8; ++i)
			{
				irDataCorrectEndian |= (irData & 0xF) << 4*(7-i);
				irData = irData >> 4;
			}
			irData = irDataCorrectEndian;
			
			_irManageDataFunc(irDataCorrectEndian);
			/*
			uartTransmit((irData >> 24) & 0xFF);	
			uartTransmit((irData >> 16) & 0xFF);
			uartTransmit((irData >> 8) & 0xFF);
			uartTransmit(irData & 0xFF);
			*/
			
			bitCount = irData = 0;
			start = TRUE;
			stopMicroCounter();
		}
		else
		{
			if (_microCount > 460 && _microCount < 660)
			{
				++bitCount;
				irData = (irData >> 1);
				_microCount = 0;
			}
			else if (_microCount > 1590 && _microCount < 1790)
			{
				++bitCount;
				irData = (irData >> 1) | 0x80000000;
				_microCount = 0;
			}
			else
			{
				bitCount = irData = 0;
				start = TRUE;
				stopMicroCounter();
			}
		}
		
	}
	//Change to low:
	else
	{
		if (start && _microCount > 8850 && _microCount < 9150)
			_microCount = 0;
		else if (start)
			stopMicroCounter();
		else if (_microCount > 460 && _microCount < 660)
			_microCount = 0;
		else
		{
			start = TRUE;
			stopMicroCounter();
		}
	}
}