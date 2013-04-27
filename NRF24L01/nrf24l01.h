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
/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void NRF24L01_Init();
uint8_t NRF24L01_ReadStatus();
uint8_t NRF24L01_ReadRegister(uint8_t Register, uint8_t* Storage, uint8_t ByteCount);
uint8_t NRF24L01_WriteRegister(uint8_t Register, uint8_t* Data, uint8_t ByteCount);

uint8_t NRF24L01_SetRxPipeAddress(uint8_t Pipe, uint32_t AddressMSBytes, uint8_t AddressLSByte);


#endif /* NRF24L01_H_ */