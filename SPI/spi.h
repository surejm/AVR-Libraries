/*
 * spi.h
 *
 * Created: 2012-09-13 15:33:17
 *  Author: Hampus
 */ 


#ifndef SPI_H_
#define SPI_H_

#define SPI_DDR			DDRB
#define SPI_PORT		PORTB
#define SPI_DATA_PIN	PORTB3
#define SPI_SCK_PIN		PORTB5

#define SPI_CLOCK_DIV4		0
#define SPI_CLOCK_DIV16		1
#define SPI_CLOCK_DIV64		2
#define SPI_CLOCK_DIV128	3
#define SPI_CLOCK_DIV2		4
#define SPI_CLOCK_DIV8		5
#define SPI_CLOCK_DIV32		6

void spiSetup(uint8_t clockFreq);
uint8_t spiWrite(uint8_t theData);


#endif /* SPI_H_ */