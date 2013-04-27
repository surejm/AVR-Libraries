/**
 ******************************************************************************
 * @file	EEPROM_24AA16.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-02-10
 * @brief	Contains functions to manage the 24AA16 EEPROM
 *			- Initialization
 *			- Write data
 *			- Read data
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>
#include <atmega328x/twi.h>
#include <MILLIS_COUNT/millis_count.h>
#include "EEPROM_24AA16.h"

/* Private defines -----------------------------------------------------------*/
#define BLOCK0			0x00
#define BLOCK1			0x01
#define BLOCK2			0x02
#define BLOCK3			0x03
#define BLOCK4			0x05
#define BLOCK5			0x06
#define BLOCK6			0x07
#define ADDRESS_BASE	0x50

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the 24AA16
 * @param	None
 * @retval	None
 */
void EEPROM_24AA16_Init()
{
	if (!TWI_Initialized())
		TWI_InitStandard();

	if (!MILLIS_COUNT_Initialized())
		MILLIS_COUNT_Init();
}

/**
 * @brief	Writes one byte to the EEPROM
 * @param	Address: Address to write to
 * @param	NewData: Byte to write
 * @retval	0: Invalid Address
 * @retval	1: Data written to address
 * @retval	2: Data at address is the same as new data so no write done
 */
uint8_t eeprom24aa16writeToAddress(uint16_t Address, uint8_t NewData)
{
	uint8_t block = Address / 256;
	if (block > 7) return 0;
	uint8_t blockAddress = Address - 256 * block;

	if (millis() - _eepromLastWriteMillis < 6) _delay_ms(6);
	TWI_BeginTransmission(ADDRESS_BASE | block);
	TWI_Write(blockAddress);
	TWI_EndTransmission();	
	
	uint8_t oldData = 0;
	TWI_RequestFrom(ADDRESS_BASE | block, &oldData, 1);
	
	if (oldData != NewData)
	{
		TWI_BeginTransmission(ADDRESS_BASE | block);
		TWI_Write(blockAddress);
		TWI_Write(NewData);
		TWI_EndTransmission();
		_eepromLastWriteMillis = millis();
		return 1;
	}
	return 2;
}

/**
 * @brief	Read one byte from the EEPROM
 * @param	Address: Address to read from
 * @retval	0: Invalid Address
 * @retval	0-0xFF: Data read
 */
uint8_t eeprom24aa16readFromAddress(uint16_t address)
{
	uint8_t block = address / 256;
	if (block > 7) return 0;
	uint8_t blockAddress = address - 256 * block;
	
	if (millis() - _eepromLastWriteMillis < 6) _delay_ms(6);
	TWI_BeginTransmission(ADDRESS_BASE | block);
	TWI_Write(blockAddress);
	TWI_EndTransmission();
	
	uint8_t data = 0;
	TWI_RequestFrom(ADDRESS_BASE | block, &data, 1);
	
	return data;
}

/**
 * @brief	Test the EEPROM
 * @param	None
 * @retval	None
 */
void EEPROM_24AA16_Test()
{
	volatile uint8_t status = eeprom24aa16writeToAddress(30, 0xDE);
	volatile uint8_t data = eeprom24aa16readFromAddress(30);
}