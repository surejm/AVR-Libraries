/**
 ******************************************************************************
 * @file	spi_interrupt.c
 * @author	Hampus Sandberg
 * @version	0.2
 * @date	2013-02-12
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
#include <avr/interrupt.h>
#include <assert/assert.h>
#include <circularBuffer/circularBuffer.h>
#include "spi_interrupt.h"

/* Private defines -----------------------------------------------------------*/
#define SPI_DDR			DDRB
#define SPI_PORT		PORTB
#define SPI_MOSI_PIN	PORTB3
#define SPI_MISO_PIN	PORTB4
#define SPI_SCK_PIN		PORTB5

/* Private variables ---------------------------------------------------------*/
SPI_Interrupt_TypeDef _spiInterrupt;
CircularBuffer_TypeDef _spiBufferTX;
volatile uint8_t _spiTransferComplete;
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
	assert_param(IS_SPI_INTERRUPT(SPI_InitStruct->SPI_Interrupt));
	
	// Set pins as output
	SPI_DDR |= (1 << SPI_MOSI_PIN) | (1 << SPI_SCK_PIN);
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
	
	if (SPI_InitStruct->SPI_Interrupt == SPI_INTERRUPT_ENABLED)
	{
		_spiInterrupt = SPI_INTERRUPT_ENABLED;
		circularBuffer_Init(&_spiBufferTX);
		_spiTransferComplete = 1;
		
		SPCR |= (1 << SPIE);
		sei();
	}
}

/**
 * @brief	Writes parameter data to the SPI peripheral
 * @param	Data: byte of data to be sent
 * @retval	None
 */
void SPI_Write(uint8_t Data)
{
	if (_spiInterrupt)
	{
		while (circularBuffer_IsFull(&_spiBufferTX));
		circularBuffer_Insert(&_spiBufferTX, Data);
		// If all data is already sent new data must be sent to start the SPI
		if (_spiTransferComplete)
		{
			_spiTransferComplete = 0;
			SPI_STC_vect();
			//SPDR = circularBuffer_Remove(&_spiBufferTX);
		}			
	}
	else
	{
		// Start Transmission
		SPDR = Data;
		// Wait for transmission complete
		while (!(SPSR & (1 << SPIF)));	
	}
	// Necessary?
	// return SPDR;
}

/* Interrupt Service Routines ------------------------------------------------*/
ISR(SPI_STC_vect)
{
	// TODO: Test on hardware!!!
	if (circularBuffer_IsEmpty(&_spiBufferTX))
	{
		_spiTransferComplete = 1;
	}
	else
	{
		SPDR = circularBuffer_Remove(&_spiBufferTX);
	}
}