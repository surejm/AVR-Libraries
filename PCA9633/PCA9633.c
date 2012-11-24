/*
 * PCA9633.c
 *
 * Created: 11/23/2012 9:50:53 PM
 *  Author: Hampus
 */ 

/*
	Improvements that can be made:
	- Sleep wake 
*/


#include "PCA9633.h"
#include <util/delay.h>

/************************************************************************
	"Private" functions
 ***********************************************************************/
void setRegister(const uint8_t registerToSet, const uint8_t value)
{
	if (registerToSet < PCA9633_REGISTER_COUNT)
	{
		twiBeginTransmission(PCA9633_ADDRESS);
		twiWrite(PCA9633_AUTO_INC_NO | registerToSet);
		twiWrite(value);
		twiEndTransmission();
	}
}

uint8_t activateRegister(const uint8_t registerToActivate)
{
	if (registerToActivate < PCA9633_REGISTER_COUNT)
	{
		twiBeginTransmission(PCA9633_ADDRESS);
		twiWrite(PCA9633_AUTO_INC_NO | registerToActivate);
		twiEndTransmission();
		return 1;
	}
	return 0;
}

/************************************************************************
	"Public" functions
 ***********************************************************************/
void pca9633setup()
{
	PCA9633_OE_DDR |= _BV(PCA9633_OE);
	pca9633outputOff();
	
	twiSetup();
	setRegister(PCA9633_MODE1, 0x00);
	
	const uint8_t ledoutValue[4] = {PCA9633_LEDOUT_PWM, PCA9633_LEDOUT_PWM, PCA9633_LEDOUT_PWM, PCA9633_LEDOUT_PWM}; 
	pca9633setLedout(ledoutValue);
}

void pca9633setOutput(const uint8_t output, const uint8_t value)
{
	if (output < 4)
		setRegister(output + 2, value);
}

void pca9633setAllOutputs(const uint8_t value0, const uint8_t value1, const uint8_t value2, const uint8_t value3)
{
	twiBeginTransmission(PCA9633_ADDRESS);
	twiWrite(PCA9633_AUTO_INC_PWM | PCA9633_PWM0);
	twiWrite(value0);
	twiWrite(value1);
	twiWrite(value2);
	twiWrite(value3);
	twiEndTransmission();
}

void pca9633outputOff() { PCA9633_OE_PORT |= _BV(PCA9633_OE); }

void pca9633outputOn() { PCA9633_OE_PORT &= ~_BV(PCA9633_OE); }

uint8_t pca9633outputIsOn() { return PCA9633_OE_PINX & _BV(PCA9633_OE); }

/************************************************************************
	PCA9633 Configuration
 ***********************************************************************/
void pca9633setLedout(const uint8_t value[4])
{
	if (value[0] < 4 && value[1] < 4 && value[2] < 4 && value[3] < 4)
	{
		uint8_t totalValue = value[3] << 6 | value[2] << 4 | value[1] << 2 | value[0];
		setRegister(PCA9633_LEDOUT, totalValue);
	}
}

void pca9633goToSleep()
{
	uint8_t mode1Value;
	if (activateRegister(PCA9633_MODE1) && twiRequestFrom(PCA9633_ADDRESS, &mode1Value, 1))
	{		
		mode1Value |= _BV(4);
		setRegister(PCA9633_MODE1, mode1Value);
	}
}

void pca9633wakeUp()
{
	uint8_t mode1Value;
	if (activateRegister(PCA9633_MODE1) && twiRequestFrom(PCA9633_ADDRESS, &mode1Value, 1))
	{
		mode1Value &= ~_BV(4);
		setRegister(PCA9633_MODE1, mode1Value);
	}
}

uint8_t pca9633isSleeping()
{
	uint8_t mode1Value;
	if (activateRegister(PCA9633_MODE1) && twiRequestFrom(PCA9633_ADDRESS, &mode1Value, 1))
		return mode1Value & _BV(4);
		
	return 2;
}

/************************************************************************
	PCA9633 Test
 ***********************************************************************/
void pca9633test()
{
	pca9633outputOn();	
	for (int i = 0; i < 256; i++)
	{
		pca9633setOutput(3, i);
		_delay_ms(10);
	}
	pca9633goToSleep();
	if (pca9633isSleeping())
		pca9633wakeUp();
	
	for (int i = 255; i > 0; i--)
	{
		pca9633setOutput(3, i);
		_delay_ms(10);
	}
}

#ifdef DEBUG_PCA9633
void pca9633readAllRegisters()
{
	uint8_t storage[PCA9633_REGISTER_COUNT];
	twiBeginTransmission(PCA9633_ADDRESS);
	twiWrite(PCA9633_AUTO_INC_ALL | PCA9633_MODE1);
	twiEndTransmission();
	
 	twiRequestFrom(PCA9633_ADDRESS, &storage, PCA9633_REGISTER_COUNT);
}
#endif
