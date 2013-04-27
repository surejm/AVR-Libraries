/*
 * PCA9633.c
 *
 * Created: 11/23/2012 9:50:53 PM
 *  Author: Hampus
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <atmega328x/twi.h>
#include <COLOR_8_BIT/color_8_bit.h>

#include "PCA9633.h"

/************************************************************************
	"Private" functions
 ***********************************************************************/
static void setRegister(const uint8_t registerToSet, const uint8_t value)
{
	if (registerToSet < PCA9633_REGISTER_COUNT)
	{
		TWI_BeginTransmission(PCA9633_ADDRESS);
		TWI_Write(PCA9633_AUTO_INC_NO | registerToSet);
		TWI_Write(value);
		TWI_EndTransmission();
	}
}

static uint8_t activateRegister(const uint8_t registerToActivate)
{
	if (registerToActivate < PCA9633_REGISTER_COUNT)
	{
		TWI_BeginTransmission(PCA9633_ADDRESS);
		TWI_Write(PCA9633_AUTO_INC_NO | registerToActivate);
		TWI_EndTransmission();
		return 1;
	}
	return 0;
}

/************************************************************************
	"Public" functions
 ***********************************************************************/
void pca9633setup()
{
	static uint8_t setupDone = 0;
	if (!setupDone)
	{
		PCA9633_OE_DDR |= _BV(PCA9633_OE);
		pca9633outputOff();
		
		if (!TWI_Initialized())
		{
			TWI_Init_TypeDef twiInit;
			twiInit.TWI_Frequency = 400000;
			twiInit.TWI_Mode = TWI_MODE_MASTER;
			twiInit.TWI_Prescaler = TWI_PRESCALER_1;
			TWI_Init(&twiInit);
		}
		
		pca9633setLedoutRegister(PCA9633_LEDOUT_PWM, PCA9633_LEDOUT_PWM, PCA9633_LEDOUT_PWM, PCA9633_LEDOUT_PWM);
		setupDone = 1;
	}
}

void pca9633setOutput(const uint8_t output, const uint8_t value)
{
	if (output < 4)
		setRegister(output + 2, value);
}

void pca9633setAllOutputs(const uint8_t value0, const uint8_t value1, const uint8_t value2, const uint8_t value3)
{
	TWI_BeginTransmission(PCA9633_ADDRESS);
	TWI_Write(PCA9633_AUTO_INC_PWM | PCA9633_PWM0);
	TWI_Write(value0);
	TWI_Write(value1);
	TWI_Write(value2);
	TWI_Write(value3);
	TWI_EndTransmission();
}

void pca9633outputOff() { PCA9633_OE_PORT |= _BV(PCA9633_OE); }
void pca9633outputOn() { PCA9633_OE_PORT &= ~_BV(PCA9633_OE); }
uint8_t pca9633outputIsOn() { return !(PCA9633_OE_PINX & _BV(PCA9633_OE)); }

rgba8 pca9633getRgba()
{
	TWI_BeginTransmission(PCA9633_ADDRESS);
	TWI_Write(PCA9633_AUTO_INC_PWM | PCA9633_PWM0);
	TWI_EndTransmission();

	uint8_t storage[4];
	TWI_RequestFrom(PCA9633_ADDRESS, storage, 4);
	rgba8 colors = {storage[0], storage[1], storage[2], storage[3]};
	return colors;
}



/************************************************************************
	PCA9633 Sleep
 ***********************************************************************/
void pca9633goToSleep()
{
	uint8_t mode1Value;
	if (activateRegister(PCA9633_MODE1) && TWI_RequestFrom(PCA9633_ADDRESS, &mode1Value, 1))
	{		
		mode1Value |= _BV(4);
		setRegister(PCA9633_MODE1, mode1Value);
	}
}

void pca9633wakeUp()
{
	uint8_t mode1Value;
	if (activateRegister(PCA9633_MODE1) && TWI_RequestFrom(PCA9633_ADDRESS, &mode1Value, 1))
	{
		mode1Value &= ~_BV(4);
		setRegister(PCA9633_MODE1, mode1Value);
	}
}

uint8_t pca9633isSleeping()
{
	uint8_t mode1Value;
	if (activateRegister(PCA9633_MODE1) && TWI_RequestFrom(PCA9633_ADDRESS, &mode1Value, 1))
		return mode1Value & _BV(4);
		
	return 2;
}

/************************************************************************
	PCA9633 Configuration
 ***********************************************************************/
void pca9633setLedoutRegister(const uint8_t value0, const uint8_t value1, const uint8_t value2, const uint8_t value3)
{
	if (value0 < 4 && value1 < 4 && value2 < 4 && value3 < 4)
	{
		setRegister(PCA9633_MODE1, 0x00);
		volatile uint8_t totalValue = value3 << 6 | value2 << 4 | value1 << 2 | value0;
		setRegister(PCA9633_LEDOUT, totalValue);
	}
}

uint8_t pca9633outputIsInverted()
{
	activateRegister(PCA9633_MODE2);
	uint8_t value;
	TWI_RequestFrom(PCA9633_ADDRESS, &value, 1);
	return value & _BV(4);
}

void pca9633invertOutputs()
{
	activateRegister(PCA9633_MODE2);
	uint8_t value;
	TWI_RequestFrom(PCA9633_ADDRESS, &value, 1);
	value |= _BV(4);
	value &= ~(_BV(0) | _BV(1));
	setRegister(PCA9633_MODE2, value);
}

void pca9633nonInvertOutputs()
{
	activateRegister(PCA9633_MODE2);
	uint8_t value;
	TWI_RequestFrom(PCA9633_ADDRESS, &value, 1);
	value &= ~_BV(4);
	setRegister(PCA9633_MODE2, value);
}

void pca9633setOeMode(uint8_t mode)
{
	if (mode < 0x03)
	{
		activateRegister(PCA9633_MODE2);
		uint8_t value;
		TWI_RequestFrom(PCA9633_ADDRESS, &value, 1);
		value &= ~0x02;
		value |= mode;
		setRegister(PCA9633_MODE2, value);
	}
}


/************************************************************************
	PCA9633 Test
 ***********************************************************************/
void pca9633test()
{
	pca9633outputOn();	
	for (int i = 0; i < 256; i++)
	{
		pca9633setOutput(0, i);
		pca9633setOutput(1, i);
		pca9633setOutput(2, i);
		pca9633setOutput(3, i);
		_delay_ms(10);
	}
	pca9633goToSleep();
	if (pca9633isSleeping())
		pca9633wakeUp();
	
	for (int i = 255; i > 0; i--)
	{
		pca9633setOutput(0, i);
		pca9633setOutput(1, i);
		pca9633setOutput(2, i);
		pca9633setOutput(3, i);
		_delay_ms(10);
	}
}

#ifdef DEBUG_PCA9633
void pca9633readAllRegisters()
{
	uint8_t storage[PCA9633_REGISTER_COUNT];
	TWI_BeginTransmission(PCA9633_ADDRESS);
	TWI_Write(PCA9633_AUTO_INC_ALL | PCA9633_MODE1);
	TWI_EndTransmission();
	
 	TWI_RequestFrom(PCA9633_ADDRESS, storage, PCA9633_REGISTER_COUNT);
}
#endif
