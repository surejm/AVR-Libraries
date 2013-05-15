/**
 ******************************************************************************
 * @file	BQ32000.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-03-05
 * @brief	Contains functions to manage the BQ32000 RTC
 * @todo	- bool validDate(dateAndTime_TypeDef);
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <assert/assert.h>
#include <atmega328x/twi.h>
#include <atmega328x/uart.h>
#include "BQ32000.h"

/* Private defines -----------------------------------------------------------*/
#define BQ32000_SECONDS		0x00
#define BQ32000_MINUTES		0x01
#define BQ32000_CENT_HOURS	0x02
#define BQ32000_DAY			0x03
#define BQ32000_DATE		0x04
#define BQ32000_MONTH		0x05
#define BQ32000_YEARS		0x06

#define BQ32000_CAL_CFG1	0x07
#define BQ32000_TCH2		0x08
#define BQ32000_CFG2		0x09

#define BQ32000_SF_KEY_1	0x20
#define BQ32000_SF_KEY_2	0x21
#define BQ32000_SFR			0x22

#define BQ32000_ADDRESS		0x68

#define BQ32000_MORNING		8
#define BQ32000_NIGHT		23
#define BQ32000_MIDDAY		12

/* Private variables ---------------------------------------------------------*/
volatile dateAndTime_TypeDef _bq32000time;
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	...
 * @param	...
 * @retval	...
 */
void BQ32000_Init()
{
	TWI_InitStandard();
	BQ32000_UpdateDateTime();
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
void BQ32000_UpdateDateTime()
{
	TWI_BeginTransmission(BQ32000_ADDRESS);
	TWI_Write(BQ32000_SECONDS);
	TWI_EndTransmission();
	
	uint8_t storage[7];
	TWI_RequestFrom(BQ32000_ADDRESS, storage, 7);
	
	_bq32000time.second = ((storage[0] & 0x70) >> 4) * 10 + (storage[0] & 0x0F);
	_bq32000time.minute = ((storage[1] & 0x70) >> 4) * 10 + (storage[1] & 0x0F);
	_bq32000time.hour = ((storage[2] & 0x30) >> 4) * 10 + (storage[2] & 0x0F);
	
	_bq32000time.weekday = storage[3] & 0x07;
	_bq32000time.date = ((storage[4] & 0x30) >> 4) * 10 + (storage[4] & 0x0F);
	_bq32000time.month = ((storage[5] & 0x10) >> 4) * 10 + (storage[5] & 0x0F);
	_bq32000time.year = ((storage[6] & 0xF0) >> 4) * 10 + (storage[6] & 0x0F);
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
void BQ32000_SetDateTime(const dateAndTime_TypeDef newDateTime)
{
	if (IS_DATE_AND_TIME(newDateTime))
	{
		TWI_BeginTransmission(BQ32000_ADDRESS);
		TWI_Write(BQ32000_SECONDS);
		TWI_Write((newDateTime.second / 10) << 4 | (newDateTime.second % 10));
		TWI_Write((newDateTime.minute / 10) << 4 | (newDateTime.minute % 10));
		TWI_Write((newDateTime.hour / 10) << 4 | (newDateTime.hour % 10));
		TWI_Write(newDateTime.weekday);
		TWI_Write((newDateTime.date / 10) << 4 | (newDateTime.date % 10));
		TWI_Write((newDateTime.month / 10) << 4 | (newDateTime.month % 10));
		TWI_Write((newDateTime.year / 10) << 4 | (newDateTime.year % 10));
		
		TWI_EndTransmission();
	}
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
void BQ32000_PrintToUart()
{
	BQ32000_UpdateDateTime();
	
	UART_WriteString("Current time and date is: ");
	if (_bq32000time.hour < 10) UART_WriteString("0");
	UART_WriteUintAsString(_bq32000time.hour);
	UART_WriteString(":");
	if (_bq32000time.minute < 10) UART_WriteString("0");
	UART_WriteUintAsString(_bq32000time.minute);
	UART_WriteString(":");
	if (_bq32000time.second < 10) UART_WriteString("0");
	UART_WriteUintAsString(_bq32000time.second);

	UART_WriteString(" - ");

	UART_WriteUintAsString(_bq32000time.date);
	UART_WriteString("/");
	UART_WriteUintAsString(_bq32000time.month);
	UART_WriteString("/20");
	UART_WriteUintAsString(_bq32000time.year);
	UART_WriteString("\r");
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
uint8_t BQ32000_ItsNighttime()
{
	BQ32000_UpdateDateTime();
	volatile uint8_t hour = _bq32000time.hour;
	return (hour < BQ32000_MORNING || hour > BQ32000_NIGHT);
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
uint8_t BQ32000_ItsMidday()
{
	BQ32000_UpdateDateTime();
	volatile uint8_t hour = _bq32000time.hour;
	return (hour == BQ32000_MIDDAY);
}

/* Interrupt Service Routines ------------------------------------------------*/