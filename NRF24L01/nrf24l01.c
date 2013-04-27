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

#define GOOD_INIT		((1 << PWR_UP) | (1 << PRIM_RX) | (1 << EN_CRC))

/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/**
 * @brief	Initializes the nRF24L01
 * @param	None
 * @retval	0: Initialization failed
 * @retval	1: Initialization succeeded
 */
uint8_t NRF24L01_Init()
{
	CE_DDR |= (1 << CE_PIN);
	CSN_DDR |= (1 << CSN_PIN);
	IRQ_DDR &= ~(1 << IRQ_PIN);
	
	DESELECT_NRF24L01;
	CHIP_DISABLE;
	
	SPI_InitTypeDef spiInit;
	spiInit.SPI_Clock = SPI_CLOCK_DIV2;
	SPI_Init(&spiInit);
	
	NRF24L01_PowerUp();
	NRF24L01_RXmode();
	
	uint8_t config = 0;
	NRF24L01_ReadRegister(CONFIG, &config, 1);
	
	if (config != GOOD_INIT)
		return 0;
	else
		return 1;
}

/**
 * @brief	Power up the nRF24L01
 * @param	None
 * @retval	The status register
 */
uint8_t NRF24L01_PowerUp()
{
	uint8_t configValue;
	NRF24L01_ReadRegister(CONFIG, &configValue, 1);
	
	configValue |= (1 << PWR_UP);
	return NRF24L01_WriteRegisterOneByte(CONFIG, configValue);
}

/**
 * @brief	Power down the nRF24L01
 * @param	None
 * @retval	The status register
 */
uint8_t NRF24L01_PowerDown()
{
	uint8_t configValue;
	NRF24L01_ReadRegister(CONFIG, &configValue, 1);
	
	configValue &= ~(1 << PWR_UP);
	return NRF24L01_WriteRegisterOneByte(CONFIG, configValue);
}

/**
 * @brief	Enter RX mode
 * @param	None
 * @retval	The status register
 */
uint8_t NRF24L01_RXmode()
{
	uint8_t configValue;
	NRF24L01_ReadRegister(CONFIG, &configValue, 1);
	
	configValue |= (1 << PRIM_RX);
	return NRF24L01_WriteRegisterOneByte(CONFIG, configValue);
}

/**
 * @brief	Enter TX mode
 * @param	None
 * @retval	The status register
 */
uint8_t NRF24L01_TXmode()
{
	uint8_t configValue;
	NRF24L01_ReadRegister(CONFIG, &configValue, 1);
	
	configValue &= ~(1 << PRIM_RX);
	return NRF24L01_WriteRegisterOneByte(CONFIG, configValue);
}

/**
 * @brief	Reads the statusregister in the nRF24L01
 * @param	None
 * @retval	The status register
 */
uint8_t NRF24L01_GetStatus()
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
 * @brief	Write one byte to a register in the nRF24L01
 * @param	Register: The register to read from
 * @param	Data: The byte to write
 * @retval	The status register
 */
uint8_t NRF24L01_WriteRegisterOneByte(uint8_t Register, uint8_t Data)
{
	return NRF24L01_WriteRegister(Register, &Data, 1);
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
		uint8_t status = SPI_WriteRead(W_REGISTER | (RX_ADDR_P0 + Pipe));
		SPI_WriteRead(AddressLSByte);	// MSByte of address in pipe 2 to 5 is equal to RX_ADDR_P1[39:8]
		DESELECT_NRF24L01;
		
		return status;
	}
	return 0;
}

/**
 * @brief	Set the address for the TX on the nRF24L01
 * @param	AddressMSBytes: The 4 highest bytes in the address
 * @param	AddressLSByte: The lowest byte in the address
 * @retval	The status register
 */
uint8_t NRF24L01_SetTxAddress(uint32_t AddressMSBytes, uint8_t AddressLSByte)
{
	SELECT_NRF24L01;
	uint8_t status = SPI_WriteRead(W_REGISTER | TX_ADDR);
	SPI_WriteRead(AddressLSByte);
	for (uint8_t i = 0; i < 4; i++)
	{
		SPI_WriteRead((AddressMSBytes >> 8*i) & 0xFF);
	}
	DESELECT_NRF24L01;
	
	return status;
}


/**
 * @brief	Count lost packets. The counter is overflow protected to 15, and discontinues at max until reset
 * @param	None
 * @retval	The count for lost packets
 * @note	The counter is reset by writing to RF_CH
 */
uint8_t NRF24L01_GetLostPacketsCount()
{
	SELECT_NRF24L01;
	uint8_t status = SPI_WriteRead(R_REGISTER | OBSERVE_TX);
	uint8_t txStatus = SPI_WriteRead(0x00);
	DESELECT_NRF24L01;
	
	return ((txStatus & 0xF0) >> 4);
}

/**
 * @brief	Count retransmitted packets
 * @param	None
 * @retval	The count for retransmits
 * @note	The counter is reset when transmission of a new packet starts
 */
uint8_t NRF24L01_GetRetransmitCount()
{
	SELECT_NRF24L01;
	uint8_t status = SPI_WriteRead(R_REGISTER | OBSERVE_TX);
	uint8_t txStatus = SPI_WriteRead(0x00);
	DESELECT_NRF24L01;
	
	return (txStatus & 0xF);
}


/**
 * @brief	Set the RF channel to transmit on
 * @param	Channel: The channel
 * @retval	The status register or 0 if invalid channel
 * @note	Freq = 2400 + RF_CH [MHz], -> 2400 MHz - 2525 MHz operation frequencies
 */
uint8_t NRF24L01_SetRFChannel(uint8_t Channel)
{
	if (Channel <= 125)
	{
		SELECT_NRF24L01;
		uint8_t status = SPI_WriteRead(W_REGISTER | RF_CH);
		SPI_WriteRead(Channel);
		DESELECT_NRF24L01;
		
		return status;
	}
	return 0;
}

/**
 * @brief	Get the RF channel nRF24L01 is currently operating on
 * @param	None
 * @retval	The RF channel nRF24L01 operates on
 */
uint8_t NRF24L01_GetRFChannel()
{
	uint8_t channel = 0xFF;
	NRF24L01_ReadRegister(RF_CH, &channel, 1);
	return channel;
}


/**
 * @brief	Get the RF Data pipe number for the payload available for reading from RX_FIFO
 * @param	None
 * @retval	The pipe number or 0x07 when RX FIFO is empty
 */
uint8_t NRF24L01_GetPipeNumberForPayload()
{
	return ((NRF24L01_GetStatus() & 0xE) >> 1);
}

/**
 * @brief	Enable the specified pipes on nRF24L01
 * @param	Pipes: The pipes that should be enabled
 * @retval	The status register
 */
uint8_t NRF24L01_EnablePipes(uint8_t Pipes)
{
	if (Pipes <= 0x3F)
	{
		uint8_t pipeValue;
		NRF24L01_ReadRegister(EN_RXADDR, &pipeValue, 1);
		
		pipeValue |= (Pipes);
		
		SELECT_NRF24L01;
		uint8_t status = SPI_WriteRead(W_REGISTER | EN_RXADDR);
		SPI_WriteRead(pipeValue);
		DESELECT_NRF24L01;
		
		return status;
	}
	return 0;
}

/**
 * @brief	Disable the specified pipes on nRF24L01
 * @param	Pipes: The pipes that should be disabled
 * @retval	The status register
 */
uint8_t NRF24L01_DisablePipes(uint8_t Pipes)
{
	if (Pipes <= 0x3F)
	{
		uint8_t pipeValue;
		NRF24L01_ReadRegister(EN_RXADDR, &pipeValue, 1);
		
		pipeValue &= ~(Pipes);
		
		SELECT_NRF24L01;
		uint8_t status = SPI_WriteRead(W_REGISTER | EN_RXADDR);
		SPI_WriteRead(pipeValue);
		DESELECT_NRF24L01;
		
		return status;
	}
	return 0;
}


/* Interrupt Service Routines ------------------------------------------------*/