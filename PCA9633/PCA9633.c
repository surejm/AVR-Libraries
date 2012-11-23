/*
 * PCA9633.c
 *
 * Created: 11/23/2012 9:50:53 PM
 *  Author: Hampus
 */ 

#include "PCA9633.h"
#include <util/delay.h>

void pca9633setup()
{
	PCA9633_OE_DDR |= _BV(PCA9633_OE);
	//pca9633outputOff();
	pca9633outputOn();
	
	twiSetup();
	twiBeginTransmission(PCA9633_ADDRESS);
	twiWrite(0x0F & PCA9633_MODE1);
	twiWrite(0x00);
	twiEndTransmission();
	
	twiBeginTransmission(PCA9633_ADDRESS);
	twiWrite(0x00 & PCA9633_LEDOUT);
	twiWrite(0xAA);
	twiEndTransmission();
}

void pca9633test()
{
	for (int i = 0; i < 256; i++)
	{
		twiBeginTransmission(PCA9633_ADDRESS);
		twiWrite(0x0F & PCA9633_PWM3);
		twiWrite(i);
		twiEndTransmission();
		_delay_ms(10);
	}
	for (int i = 255; i > 0; i--)
	{
		twiBeginTransmission(PCA9633_ADDRESS);
		twiWrite(0x0F & PCA9633_PWM3);
		twiWrite(i);
		twiEndTransmission();
		_delay_ms(10);
	}
}

void pca9633setOutput(const uint8_t output, const uint8_t value)
{
	
}

void pca9633setAllOutputs(const uint8_t value1, const uint8_t value2, const uint8_t value3, const uint8_t value4)
{
	
}

void pca9633outputOff()
{
	PCA9633_OE_PORT |= _BV(PCA9633_OE);
}

void pca9633outputOn()
{
	PCA9633_OE_PORT &= ~_BV(PCA9633_OE);
}