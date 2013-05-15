/**
 ******************************************************************************
 * @file	nrf24l01.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-04-27
 * @brief	
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef NRF24L01_H_
#define NRF24L01_H_

/* Includes ------------------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
#define MSB_BYTES(BYTES)	((BYTES >> 8) & 0xFFFFFFFF)
#define LSB_BYTE(BYTES)		(BYTES & 0xFF)

#define NRF24L01_SetTxAddress(Address)	(NRF24L01_SetTxAddressSeparated(MSB_BYTES(Address), LSB_BYTE(Address)))
#define NRF24L01_SetRxPipeAddress(Pipe, Address) (NRF24L01_SetRxPipeAddressSeparated(Pipe, MSB_BYTES(Address), LSB_BYTE(Address)))

/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
uint8_t NRF24L01_Init();
uint8_t NRF24L01_PowerUp();
uint8_t NRF24L01_PowerDown();

uint8_t NRF24L01_RXmode();
uint8_t NRF24L01_TXmode();
uint8_t NRF24L01_StandbyOneMode();

uint8_t NRF24L01_GetStatus();
uint8_t NRF24L01_ReadRegister(uint8_t Register, uint8_t* Storage, uint8_t ByteCount);
uint8_t NRF24L01_WriteRegister(uint8_t Register, uint8_t* Data, uint8_t ByteCount);
uint8_t NRF24L01_WriteRegisterOneByte(uint8_t Register, uint8_t Data);

uint8_t NRF24L01_SetRxPipeAddressSeparated(uint8_t Pipe, uint32_t AddressMSBytes, uint8_t AddressLSByte);
uint8_t NRF24L01_SetTxAddressSeparated(uint32_t AddressMSBytes, uint8_t AddressLSByte);

uint8_t NRF24L01_GetLostPacketsCount();
uint8_t NRF24L01_GetRetransmitCount();

uint8_t NRF24L01_SetRFChannel(uint8_t Channel);
uint8_t NRF24L01_GetRFChannel();

uint8_t NRF24L01_GetPipeNumberForPayload();
uint8_t NRF24L01_EnablePipes(uint8_t Pipes);
uint8_t NRF24L01_DisablePipes(uint8_t Pipes);

uint8_t NRF24L01_Write(uint8_t* Data, uint8_t ByteCount);

uint8_t NRF24L01_GetFIFOStatus();
uint8_t NRF24L01_TxFIFOEmpty();

void NRF24L01_FlushTX();
void NRF24L01_FlushRX();


#endif /* NRF24L01_H_ */