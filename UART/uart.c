/*
 * uart.c
 *
 * Created: 2012-09-14 22:59:00
 *  Author: Hampus
 */

#include "uart.h"
#include <avr/interrupt.h>

void uartSetup(const uint8_t theBaudRate, void(*theUartManageFunc)(uint8_t))
{
	_uartManageDataFunc = theUartManageFunc;
	cli();
	UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	
	if (theBaudRate == BAUD_57600 || theBaudRate == BAUD_115200)
		UCSR0A |= _BV(U2X0);
	
	if (theBaudRate == BAUD_9600 || theBaudRate == BAUD_19200 || theBaudRate == BAUD_38400 || 
		theBaudRate == BAUD_57600 || theBaudRate == BAUD_115200)
			UBRR0 = theBaudRate;
	else
		UBRR0 = BAUD_9600;
	sei();
}

void uartTransmit(const uint8_t theData)
{
	while (!(UCSR0A & _BV(UDRE0)));
	UDR0 = theData;
}

ISR(USART_RX_vect)
{
	uint8_t receivedData = 0;
	receivedData = UDR0;
	if (_uartManageDataFunc) _uartManageDataFunc(receivedData);
}

void uartTransmitString(const char* theString)
{
	while (*theString != 0x00)
	{
		uartTransmit(*theString);
		*theString++;
	}
}