/**
 ******************************************************************************
 * @file	spi.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-02-10
 * @brief	Contains functions to manage the SPI-peripheral on ATmega328x
 *			- Initialization
 *			- Write data
 *			- Buffer functionality if interrupt is used
 *			- Receive data (not implemented yet)
 * @note	The interrupt approach is unnecessary cause the SPI clock is so
			fast that it takes more time to jump to ISR than to just poll the
			flag. Makes it a lot cleaner as well.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <assert/assert.h>
#include "spi.h"

/* Private defines -----------------------------------------------------------*/
#define SPI_DDR			DDRB
#define SPI_PORT		PORTB
#define SPI_SS_PIN		PORTB2
#define SPI_MOSI_PIN	PORTB3
#define SPI_MISO_PIN	PORTB4
#define SPI_SCK_PIN		PORTB5

#define MASTER_MODE		(SPCR |= (1 << MSTR))
#define SLAVE_MODE		(SPCR &= ~(1 << MSTR))

#define SPI_FLAG		(SPSR & (1 << SPIF))

/* Private variables ---------------------------------------------------------*/
uint8_t _spiInitStatus;
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the SPI peripheral according to the specified parameters in the SPI_InitStruct.
 * @param	SPI_InitStruct: pointer to a SPI_InitTypeDef structure that contains
 *			the configuration information for the SPI peripheral.
 * @retval	None
 */
void SPI_Init(SPI_InitTypeDef* SPI_InitStruct)
{	
	// Check parameters
	assert_param(IS_SPI_CLOCK(SPI_InitStruct->SPI_Clock));
	
	// Set pins as output and inputs
	SPI_DDR |= (1 << SPI_MOSI_PIN) | (1 << SPI_SCK_PIN);
	SPI_DDR &= ~(1 << SPI_MISO_PIN);
	// Enable SPI
	SPCR = (1 << MSTR) | (1 << SPE);
	
	// Set clock frequency
	SPCR &= ~0x03;
	SPSR &= ~(1 << SPI2X);
	
	switch (SPI_InitStruct->SPI_Clock)
	{
	case SPI_CLOCK_DIV4:
		break;
	case SPI_CLOCK_DIV16:
		SPCR |= 0x01;
		break;
	case SPI_CLOCK_DIV64:
		SPCR |= 0x02;
		break;
	case SPI_CLOCK_DIV128:
		SPCR |= 0x03;
		break;
	case SPI_CLOCK_DIV2:
		SPSR |= (1 << SPI2X);
		break;
	case SPI_CLOCK_DIV8:
		SPCR |= 0x01;
		SPSR |= (1 << SPI2X);
		break;
	case SPI_CLOCK_DIV32:
		SPCR |= 0x02;
		SPSR |= (1 << SPI2X);
		break;
	}
	
	_spiInitStatus = 1;
}

/**
 * @brief	Writes parameter data to the SPI peripheral
 * @param	Data: byte of data to be sent
 * @retval	None
 */
void SPI_Write(uint8_t Data)
{
	// Start Transmission
	SPDR = Data;
	// Wait for transmission complete
	while (!SPI_FLAG);
}

/**
 * @brief	Reads data from a SPI slave
 * @param	None
 * @retval	The data read
 */
uint8_t SPI_Read()
{
	// Wait for data byte to be received
	while (!SPI_FLAG);
	return SPDR;
}

/**
 * @brief	Reads and writes data
 * @param	Data: Data to write
 * @retval	The data read
 */
uint8_t SPI_WriteRead(uint8_t Data)
{
	// Start Transmission
	SPDR = Data;
	// Wait for transmission complete
	while (!SPI_FLAG);
	return SPDR;
}

/**
 * @brief	Checks to see if SPI has been initialized
 * @param	None
 * @retval	None
 */
uint8_t SPI_Initialized()
{
	return _spiInitStatus;
}

/* Interrupt Service Routines ------------------------------------------------*/