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

#define PAYLOAD_SIZE		32
#define DATA_COUNT_INDEX	0
#define MAX_DATA_COUNT		PAYLOAD_SIZE-2	// 1 byte datacount + 1 byte checksum
#define PAYLOAD_FILLER_DATA	0x00

#define NRF24L01_MAX_AVAILABLE_DATA	CIRCULARBUFFER_SIZE

/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void NRF24L01_Init();
void NRF24L01_WritePayload(uint8_t* Data, uint8_t ByteCount);

uint8_t NRF24L01_SetRxPipeAddressSeparated(uint8_t Pipe, uint32_t AddressMSBytes, uint8_t AddressLSByte);
uint8_t NRF24L01_SetTxAddressSeparated(uint32_t AddressMSBytes, uint8_t AddressLSByte);

uint8_t NRF24L01_SetRFChannel(uint8_t Channel);
uint8_t NRF24L01_GetStatus();

uint8_t NRF24L01_GetFIFOStatus();
uint8_t NRF24L01_TxFIFOEmpty();

void NRF24L01_EnablePipes(uint8_t Pipes);
void NRF24L01_DisablePipes(uint8_t Pipes);
uint8_t NRF24L01_GetPipeNumber();
uint8_t NRF24L01_GetAvailableDataForPipe(uint8_t Pipe);
void NRF24L01_GetDataFromPipe(uint8_t Pipe, uint8_t* Storage, uint8_t DataCount);

uint8_t NRF24L01_GetChecksum(uint8_t* Data, uint8_t DataCount);
uint16_t NRF24L01_GetChecksumErrors();

void NRF24L01_WriteDebugToUart();

#endif /* _MIRF_H_ */
