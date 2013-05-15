/**
 ******************************************************************************
 * @file	spi.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-03-17
 * @brief	Contains function prototypes, constants to manage the 24AA16 EEPROM
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EEPROM_24AA16_H_
#define EEPROM_24AA16_H_

/* Includes ------------------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void EEPROM_24AA16_Init();
uint8_t eeprom24aa16writeToAddress(uint16_t address, uint8_t dataByte);
uint8_t eeprom24aa16readFromAddress(uint16_t address);

void EEPROM_24AA16_Test();

uint32_t _eepromLastWriteMillis;

#endif /* EEPROM_24AA16_H_ */