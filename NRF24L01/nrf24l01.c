/**
 ******************************************************************************
 * @file	nrf24l01.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-04-27
 * @brief	
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <atmega328x/spi.h>
#include "nrf24l01_register_map.h"
#include "nrf24l01.h"

/* Private defines -----------------------------------------------------------*/
#define CE_PIN			PORTD2
#define CE_DDR			DDRD
#define CE_PORT			PORTD
#define CHIP_ENABLE		(CE_PORT |= (1 << CE_PIN))
#define CHIP_DISABLE	(CE_PORT &= ~(1 << CE_PIN))

#define CSN_PIN			PORTD3
#define CSN_DDR			DDRD
#define CSN_PORT		PORTD
#define SELECT_NRF24L01		(CSN_PORT &= ~(1 << CSN_PIN))
#define DESELECT_NRF24L01	(CSN_PORT |= (1 << CSN_PIN))

#define IRQ_PIN			PORTD4
#define IRQ_DDR			DDRD
#define IRQ_PORT		PORTD

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static uint8_t NRF24L01_DoCommand(uint8_t Commmand);
/* Functions -----------------------------------------------------------------*/

/**
 * @brief	Initializes the nRF24L01
 * @param	None
 * @retval	None
 */
void NRF24L01_Init()
{
	CE_DDR |= (1 << CE_PIN);
	CSN_DDR |= (1 << CSN_PIN);
	IRQ_DDR &= ~(1 << IRQ_PIN);
	
	DESELECT_NRF24L01;
	CHIP_DISABLE;
	
	SPI_InitTypeDef spiInit;
	spiInit.SPI_Clock = SPI_CLOCK_DIV2;
	SPI_Init(&spiInit);	
}

/**
 * @brief	Reads the statusregister in the nRF24L01
 * @param	None
 * @retval	The status register
 */
uint8_t NRF24L01_ReadStatus()
{
	SELECT_NRF24L01;
	uint8_t status = SPI_WriteRead(NOP);
	DESELECT_NRF24L01;
	
	return status;
}

/**
 * @brief	Reads data from a register in the nRF24L01
 * @param	Register: The register to read from
 * @param	Storage: Pointer to where to store the read data
 * @param	ByteCount: How many bytes to read
 * @retval	The status register
 */
uint8_t NRF24L01_ReadRegister(uint8_t Register, uint8_t* Storage, uint8_t ByteCount)
{
	if (IS_VALID_REGISTER(Register))
	{
		SELECT_NRF24L01;
		uint8_t status = SPI_WriteRead(R_REGISTER | Register);
		for (uint8_t i = 0; i < ByteCount; i++)
		{
			Storage[i] = SPI_WriteRead(0x00);
		}
		DESELECT_NRF24L01;
		
		return status;
	}
	return 0;
}

/**
 * @brief	Write data to a register in the nRF24L01
 * @param	Register: The register to read from
 * @param	Storage: Pointer to where to store the read data
 * @param	ByteCount: How many bytes to read
 * @retval	The status register
 */
uint8_t NRF24L01_WriteRegister(uint8_t Register, uint8_t* Data, uint8_t ByteCount)
{
	if (IS_VALID_REGISTER(Register))
	{
		SELECT_NRF24L01;
		uint8_t status = SPI_WriteRead(W_REGISTER | Register);
		for (uint8_t i = 0; i < ByteCount; i++)
		{
			SPI_WriteRead(Data[i]);
		}
		DESELECT_NRF24L01;
		
		return status;
	}
	return 0;
}

/**
 * @brief	Set the address for the different RX pipes on the nRF24L01
 * @param	Pipe: The pipe to set
 * @param	AddressMSBytes: The 4 highest bytes in the address
 * @param	AddressLSByte: The lowest byte in the address
 * @retval	The status register
 */
uint8_t NRF24L01_SetRxPipeAddress(uint8_t Pipe, uint32_t AddressMSBytes, uint8_t AddressLSByte)
{
	if (Pipe <= 1)
	{
		SELECT_NRF24L01;
		uint8_t status = SPI_WriteRead(W_REGISTER | (10 + Pipe));
		SPI_WriteRead(AddressLSByte);		
		for (uint8_t i = 0; i < 4; i++)
		{
			SPI_WriteRead((AddressMSBytes >> 8*i) & 0xFF);
		}
		DESELECT_NRF24L01;
		
		return status;
	}
	else if (Pipe <= 5)
	{
		SELECT_NRF24L01;
		uint8_t status = SPI_WriteRead(W_REGISTER | (10 + Pipe));
		SPI_WriteRead(AddressLSByte);	// MSByte of address in pipe 2 to 5 is equal to RX_ADDR_P1[39:8]
		DESELECT_NRF24L01;
		
		return status;
	}
	return 0;
}

static uint8_t NRF24L01_DoCommand(uint8_t Commmand)
{
	if (IS_VALID_COMMAND(Commmand))
	{
		SELECT_NRF24L01;
		uint8_t status = SPI_WriteRead(Commmand);
		DESELECT_NRF24L01;
		return status;
	}
	return 0;
}

/* Interrupt Service Routines ------------------------------------------------*/