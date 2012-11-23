/*
 * spi.c
 *
 * Created: 2012-09-13 15:33:26
 *  Author: Hampus
 */ 

#include <avr/io.h>
#include "spi.h"

void spiSetup(uint8_t clockFreq)
{
	// Set pins as output
	SPI_DDR |= _BV(SPI_DATA_PIN) |_BV(SPI_SCK_PIN);
		
	// Enable SPI
	SPCR |= _BV(MSTR) | _BV(SPE);
	
	// Set clock frequency - If unvalid set it to default
	if (clockFreq > 6) clockFreq = SPI_CLOCK_DIV4;

	SPCR &= ~0x03;
	SPSR &= ~_BV(SPI2X);
	
	switch (clockFreq)
	{
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
		SPSR |= _BV(SPI2X);
		break;
	case SPI_CLOCK_DIV8:
		SPCR |= 0x01;
		SPSR |= _BV(SPI2X);
		break;
	case SPI_CLOCK_DIV32:
		SPCR |= 0x02;
		SPSR |= _BV(SPI2X);
		break;
	}
}

uint8_t spiWrite(uint8_t theData)
{
	// Start Transmission
	SPDR = theData;
	//volatile int test = SPDR;
	// Wait for transmission complete
	while(!(SPSR & (1 << SPIF)));
	
	return SPDR;
}