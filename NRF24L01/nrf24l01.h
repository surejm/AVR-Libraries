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
#define MAX_DATA_COUNT		PAYLOAD_SIZE-1
#define PAYLOAD_FILLER_DATA	0x00

/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void NRF24L01_Init();
void NRF24L01_WritePayload(uint8_t* Data, uint8_t ByteCount);

uint8_t NRF24L01_SetRxPipeAddressSeparated(uint8_t Pipe, uint32_t AddressMSBytes, uint8_t AddressLSByte);
uint8_t NRF24L01_SetTxAddressSeparated(uint32_t AddressMSBytes, uint8_t AddressLSByte);

uint8_t NRF24L01_SetRFChannel(uint8_t Channel);
uint8_t NRF24L01_GetStatus();

void NRF24L01_WriteRegisterOneByte(uint8_t Register, uint8_t Data);
void NRF24L01_ReadRegister(uint8_t Register, uint8_t* Storage, uint8_t ByteCount);
void NRF24L01_WriteRegister(uint8_t Register, uint8_t * Data, uint8_t ByteCount);

uint8_t NRF24L01_GetFIFOStatus();
uint8_t NRF24L01_TxFIFOEmpty();
void NRF24L01_FlushTX();
void NRF24L01_FlushRX();

void NRF24L01_ResetToRx();


void mirf_set_RADDR(uint8_t * adr);
void mirf_set_TADDR(uint8_t * adr);
uint8_t mirf_data_ready();
uint8_t NRF24L01_GetData(uint8_t* Storage);

#endif /* _MIRF_H_ */
