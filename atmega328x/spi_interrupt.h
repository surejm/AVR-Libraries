/**
 ******************************************************************************
 * @file	spi_interrupt.h
 * @author	Hampus Sandberg
 * @version	0.2
 * @date	2013-02-12
 * @brief	Contains function prototypes, constants to manage the SPI-peripheral on 
 *			ATmega328x
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SPI_INTERRUPT_H_
#define SPI_INTERRUPT_H_

/* Includes ------------------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/**
 * @brief  SPI peripheral speed
 */
typedef enum
{
	SPI_CLOCK_DIV4 =	0x00,
	SPI_CLOCK_DIV16 =	0x01,
	SPI_CLOCK_DIV64 =	0x02,
	SPI_CLOCK_DIV128 =	0x03,
	SPI_CLOCK_DIV2 =	0x04,
	SPI_CLOCK_DIV8 =	0x05,
	SPI_CLOCK_DIV32 =	0x06
} SPI_Clock_TypeDef;
#define IS_SPI_CLOCK(CLOCK) (((CLOCK) == SPI_CLOCK_DIV4) || ((CLOCK) == SPI_CLOCK_DIV16) || \
                            ((CLOCK) == SPI_CLOCK_DIV64)|| ((CLOCK) == SPI_CLOCK_DIV128) || \
                            ((CLOCK) == SPI_CLOCK_DIV2) || ((CLOCK) == SPI_CLOCK_DIV8) || \
                            ((CLOCK) == SPI_CLOCK_DIV32))

/**
 * @brief  SPI interrupt mode
 */
typedef enum
{
	SPI_INTERRUPT_DISABLED =	0x00,
	SPI_INTERRUPT_ENABLED =		0x01,
} SPI_Interrupt_TypeDef;
#define IS_SPI_INTERRUPT(INTERRUPT) (((INTERRUPT) == SPI_INTERRUPT_DISABLED) || ((INTERRUPT) == SPI_INTERRUPT_ENABLED))

/**
 * @brief  SPI Init structure definition
 */
typedef struct
{
    SPI_Clock_TypeDef SPI_Clock;			/** Specifies the clock for the SPI peripheral.
												This parameter can be any value of SPI_Clock_TypeDef */
	SPI_Interrupt_TypeDef SPI_Interrupt;	/** Specifies if the SPI transfer complete interrupt
												should be enabled or not */
} SPI_InitTypeDef;

/* Function prototypes -------------------------------------------------------*/

void SPI_Init(SPI_InitTypeDef* SPI);
void SPI_Write(uint8_t Data);

#endif /* SPI_INTERRUPT_H_ */