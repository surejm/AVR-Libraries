/**
 ******************************************************************************
 * @file	pca9685.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-04-04
 * @brief	Contains functions to manage the PCA9685 LED Driver
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <atmega328x/twi.h>
#include <assert/assert.h>
#include "pca9685.h"

/* Private defines -----------------------------------------------------------*/
#define MODE1				0x00
	#define MODE1_ALLCALL	0
	#define MODE1_SUB3		1
	#define MODE1_SUB2		2
	#define MODE1_SUB1		3
	#define MODE1_SLEEP		4
	#define MODE1_AI		5
	#define MODE1_EXTCLK	6
	#define MODE1_RESTART	7

#define MODE2				0x01
	#define MODE2_OUTNE0	0
	#define MODE2_OUTNE1	1
	#define MODE2_OUTDRV	2
	#define MODE2_OCH		3
	#define MODE2_INVRT		4

#define SUBADR1				0x02
#define SUBADR2				0x03
#define SUBADR3				0x04
#define ALLCALLADR			0x05

#define LEDn_ON_L(n)		(0x06 + (n)*4)
#define LEDn_ON_H(n)		(0x07 + (n)*4)
#define LEDn_OFF_L(n)		(0x08 + (n)*4)
#define LEDn_OFF_H(n)		(0x09 + (n)*4)

#define ALL_LED_ON_L		0xFA
#define ALL_LED_ON_H		0xFB
#define ALL_LED_OFF_L		0xFC
#define ALL_LED_OFF_H		0xFD

#define PRE_SCALE			0xFE

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/**
 * @brief	Initializes the PCA9685
 * @param	None
 * @retval	1: A PCA9685 at address [Address] has been initialized
 * @retval	0: Initialization failed
 */
uint8_t PCA9685_Init(PCA9685_Init_TypeDef *PCA9685_InitStruct);
{
	// Check parameters
	assert_param(IS_PCA9685_INV_OUTPUTS(PCA9685_InitStruct->InvOutputs));
    assert_param(IS_PCA9685_OUTPUT_DRIVER(PCA9685_InitStruct->OutputDriver));
    assert_param(IS_PCA9685_OUTPUT_NOT_EN(PCA9685_InitStruct->OutputNotEn));

	if (!TWI_Initialized())
		TWI_InitStandard();
	
	uint8_t status = 0;
	
	/* MODE1 Register:
	 * Internal clock
	 * Register Auto-Increment enabled
	 * Normal mode
	 * Does not respond to subaddresses
	 * Responds to All Call I2C-bus address
	 */
	status = TWI_BeginTransmission(PCA9685_InitStruct->Address);
	if (status)
	{
		TWI_Write(MODE1);
		TWI_Write((1 << MODE1_AI) | (1 << MODE1_ALLCALL));
	}
	TWI_EndTransmission();
	
	/* MODE2 Register:
	 * Internal clock
	 * Register Auto-Increment enabled
	 * Normal mode
	 * Does not respond to subaddresses
	 * Responds to All Call I2C-bus address
	 */
	if (status)
	{
		uint8_t mode2 = (PCA9685_InitStruct->InvOutputs << MODE2_INVRT) |
						(PCA9685_InitStruct->OutputDriver << MODE2_OUTDRV) |
						(PCA9685_InitStruct->OutputNotEn << MODE2_OUTNE0);
		TWI_BeginTransmission(PCA9685_InitStruct->Address);
		TWI_Write(MODE2);
		TWI_Write(mode2);
		TWI_EndTransmission();
	}
	
	return status;
}

/* Interrupt Service Routines ------------------------------------------------*/