/**
 ******************************************************************************
 * @file	nrf24l01.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-04-27
 * @brief	
 ******************************************************************************
 */

/*
TODO:
- Buffer for each pipe where you can do like, dataAvailableInPipe(3), getDataFromPipe(3)
- NRF24L01_Write(), so the user don't have to think about splitting the data into payloads
- Implement how different addresses are handled
*/

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <atmega328x/spi.h>
#include "nrf24l01_register_map.h"
#include "nrf24l01.h"

/* Private defines -----------------------------------------------------------*/
#define CE_PIN			PORTD4
#define CE_DDR			DDRD
#define CE_PORT			PORTD
#define ENABLE_RF		(CE_PORT |= (1 << CE_PIN))
#define DISABLE_RF		(CE_PORT &= ~(1 << CE_PIN))

#define CSN_PIN			PORTD3
#define CSN_DDR			DDRD
#define CSN_PORT		PORTD
#define SELECT_NRF24L01		(CSN_PORT &= ~(1 << CSN_PIN))
#define DESELECT_NRF24L01	(CSN_PORT |= (1 << CSN_PIN))

#define IRQ_PIN			PORTD2	// INT0
#define IRQ_DDR			DDRD
#define IRQ_PORT		PORTD

#define TX_POWERUP NRF24L01_WriteRegisterOneByte(CONFIG, CONFIG_VALUE | ((1 << PWR_UP) | (0 << PRIM_RX)))
#define RX_POWERUP NRF24L01_WriteRegisterOneByte(CONFIG, CONFIG_VALUE | ((1 << PWR_UP) | (1 << PRIM_RX)))
#define RESET_STATUS NRF24L01_WriteRegisterOneByte(STATUS, (1 << TX_DS) | (1 << MAX_RT))

#define DEFAULT_CHANNEL		66

#define CONFIG_VALUE		((1 << MASK_RX_DR) | (1 << EN_CRC) | (0 << CRCO))

#define TIMEOUT_WRITE		500
#define TX_MODE_TIMEOUT		500


/* Private variables ---------------------------------------------------------*/
volatile uint8_t _inTxMode;
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/**
 * @brief	Initializes the nRF24L01
 * @param	None
 * @retval	0: Initialization failed
 * @retval	1: Initialization succeeded
 */
void NRF24L01_Init() 
{
	CE_DDR |= (1 << CE_PIN);
	CSN_DDR |= (1 << CSN_PIN);
	DISABLE_RF;
	DESELECT_NRF24L01;

	// Interrupt init
	IRQ_DDR &= ~(1 << IRQ_PIN);
	EICRA &= ~((1 << ISC00) | (1 << ISC01));
	EICRA |= (ISC01); // The falling edge of INT0 generates an interrupt request.
	EIMSK |= (1 << INT0);
	sei();

    // Initialize spi module
    SPI_InitTypeDef spiInit;
    spiInit.SPI_Clock = SPI_CLOCK_DIV2;
    SPI_Init(&spiInit);
	
	_delay_ms(50);
	
	// Set channel
	NRF24L01_SetRFChannel(DEFAULT_CHANNEL);

	// Set length of incoming payload
	NRF24L01_WriteRegisterOneByte(RX_PW_P0, PAYLOAD_SIZE);

	// Flush buffers
	NRF24L01_FlushTX();
	NRF24L01_FlushRX();
	RESET_STATUS;

	// Start receiver
	_inTxMode = 0;        // Start in receiving mode
	RX_POWERUP;     // Power up in receiving mode
	ENABLE_RF;     // Listening for pakets
}

/**
 * @brief	Write data and send it to the address specified in TX_ADDR
 * @param	Data: Pointer to where the data is stored
 * @param	ByteCount: The number of bytes in Data
 * @retval	The status register
 * @note	W_TX_PAYLOAD commands wants LSByte first but Data is MSByte so the for loop is backwards
 *			Send as much data as defined in the payload
 */
void NRF24L01_WritePayload(uint8_t* Data, uint8_t ByteCount)
{
	// You can only send the amount of data specified in MAX_DATA_COUNT
	if (ByteCount > MAX_DATA_COUNT) return;
	
	uint16_t timeOfEnter = millis16bit();
	// Wait until last payload is sent
    while (_inTxMode);
// 	{
// 		if ((NRF24L01_GetStatus() & ((1 << TX_DS)  | (1 << MAX_RT))))
// 		{
// 			_inTxMode = 0;
// 			break;
// 		}
// 		
// /*		if (millis16bit() - timeOfEnter >= TIMEOUT_WRITE) NRF24L01_ResetToRx();*/
// 	}
		
    DISABLE_RF;
	NRF24L01_PowerUpTx();
	NRF24L01_FlushTX();
    
    SELECT_NRF24L01;
    SPI_Write(W_TX_PAYLOAD);
	SPI_Write(ByteCount);
	uint8_t i;
	for (i = 0; i < ByteCount; i++) SPI_Write(Data[i]);
	for (; i < MAX_DATA_COUNT; i++) SPI_Write(PAYLOAD_FILLER_DATA);
    DESELECT_NRF24L01;
    
    ENABLE_RF;
}


void NRF24L01_PowerUpRx()
{
	_inTxMode = 0;
	NRF24L01_WriteRegisterOneByte(CONFIG, CONFIG_VALUE | ((1 << PWR_UP) | (1 << PRIM_RX)));
	NRF24L01_WriteRegisterOneByte(STATUS, (1 << TX_DS) | (1 << MAX_RT));
	//_delay_us(50);		// NOT GOOD!!!
}

void NRF24L01_PowerUpTx()
{
	_inTxMode = 1;
	NRF24L01_WriteRegisterOneByte(CONFIG, CONFIG_VALUE | ((1 << PWR_UP) | (0 << PRIM_RX)));
}

uint8_t NRF24L01_IsSending()
{
	uint8_t status;
	if (_inTxMode)
	{
		status = NRF24L01_GetStatus();
		// if sending successful (TX_DS) or max retries exceeded (MAX_RT)
		if ((status & ((1 << TX_DS)  | (1 << MAX_RT))))
		{
			NRF24L01_PowerUpRx();
			return 0;
		}

		return 1;
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
uint8_t NRF24L01_SetRxPipeAddressSeparated(uint8_t Pipe, uint32_t AddressMSBytes, uint8_t AddressLSByte)
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
uint8_t NRF24L01_SetTxAddressSeparated(uint32_t AddressMSBytes, uint8_t AddressLSByte)
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
		SPI_WriteRead(W_REGISTER | RF_CH);
		SPI_WriteRead(Channel);
		DESELECT_NRF24L01;
		
		return 1;
	}
	return 0;
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
 * @brief	Write one byte to a register in the nRF24L01
 * @param	Register: The register to read from
 * @param	Data: The byte to write
 * @retval	None
 */
void NRF24L01_WriteRegisterOneByte(uint8_t Register, uint8_t Data)
{
	if (IS_VALID_REGISTER(Register))
	{
		SELECT_NRF24L01;
		SPI_WriteRead(W_REGISTER | (REGISTER_MASK & Register));
		SPI_WriteRead(Data);
		DESELECT_NRF24L01;
	}		
}

/**
 * @brief	Reads data from a register in the nRF24L01
 * @param	Register: The register to read from
 * @param	Storage: Pointer to where to store the read data
 * @param	ByteCount: How many bytes to read
 * @retval	The status register
 * @note	LSByte is read first
 */
void NRF24L01_ReadRegister(uint8_t Register, uint8_t* Storage, uint8_t ByteCount)
{
	if (IS_VALID_REGISTER(Register))
	{
		SELECT_NRF24L01;
		uint8_t status = SPI_WriteRead(R_REGISTER | Register);
		for (uint8_t i = 0; i < PAYLOAD_SIZE; i++)
		{
			Storage[i] = SPI_WriteRead(Storage[i]);
		}
		DESELECT_NRF24L01;
	}	
}

/**
 * @brief	Write data to a register in the nRF24L01
 * @param	Register: The register to read from
 * @param	Storage: Pointer to where to store the read data
 * @param	ByteCount: How many bytes to read
 * @retval	None
 */
void NRF24L01_WriteRegister(uint8_t Register, uint8_t* Data, uint8_t ByteCount)
{
	if (IS_VALID_REGISTER(Register))
	{
		SELECT_NRF24L01;
		uint8_t status = SPI_WriteRead(W_REGISTER | Register);
		for (uint8_t i = 0; i < PAYLOAD_SIZE; i++)
		{
			SPI_WriteRead(Data[i]);
		}
		DESELECT_NRF24L01;
	}	
}

/**
 * @brief	Get the TX FIFO status
 * @param	None
 * @retval	The TX FIFO status
 */
uint8_t NRF24L01_GetFIFOStatus()
{
	uint8_t FIFOStatus = 0;
	NRF24L01_ReadRegister(FIFO_STATUS, &FIFOStatus, 1);
	return FIFOStatus;
}

/**
 * @brief	Check if the TX FIFO is empty
 * @param	None
 * @retval	1: If empty
 * @retval	0: If not empty
 */
uint8_t NRF24L01_TxFIFOEmpty()
{
	uint8_t FIFOStatus = NRF24L01_GetFIFOStatus();
	return ((FIFOStatus & (1 << TX_EMPTY)) >> TX_EMPTY);
}

/**
 * @brief	Flush the TX buffer
 * @param	None
 * @retval	None
 */
void NRF24L01_FlushTX()
{
	SELECT_NRF24L01;
	SPI_WriteRead(FLUSH_TX);
	DESELECT_NRF24L01;
}

/**
 * @brief	Flush the RX buffer
 * @param	None
 * @retval	None
 */
void NRF24L01_FlushRX()
{
	SELECT_NRF24L01;
	SPI_WriteRead(FLUSH_RX);
	DESELECT_NRF24L01;
}

/**
 * @brief	Reset everything and power up as a receiver
 * @param	None
 * @retval	None
 */
void NRF24L01_ResetToRx()
{
	DISABLE_RF;
	RX_POWERUP;
	ENABLE_RF;
	_inTxMode = 0;
		
	RESET_STATUS;
}



void mirf_set_RADDR(uint8_t * adr)
// Sets the receiving address
{
	DISABLE_RF;
	NRF24L01_WriteRegister(RX_ADDR_P0,adr,5);
	ENABLE_RF;
}

void mirf_set_TADDR(uint8_t * adr)
// Sets the transmitting address
{
	NRF24L01_WriteRegister(TX_ADDR, adr,5);
}

uint8_t mirf_data_ready()
// Checks if data is available for reading
{
	static uint16_t timeoutTimer = 0;
	if (_inTxMode)
	{
		if (millis16bit() - timeoutTimer >= TX_MODE_TIMEOUT) NRF24L01_ResetToRx();
		return 0;
	}
	
	uint8_t status;
	// Read MiRF status
	SELECT_NRF24L01;                                // Pull down chip select
	status = SPI_WriteRead(NOP);               // Read status register
	DESELECT_NRF24L01;                                // Pull up chip select
	return status & (1<<RX_DR);
}

uint8_t NRF24L01_GetData(uint8_t* Storage)
// Reads DEFAULT_PAYLOAD bytes into data array
{
	SELECT_NRF24L01;                               // Pull down chip select
	SPI_WriteRead(R_RX_PAYLOAD);            // Send cmd to read rx payload
	uint8_t dataCount = SPI_WriteRead(PAYLOAD_FILLER_DATA);	
	for (uint8_t i = 0; i < dataCount; i++)
	{
		Storage[i] = SPI_WriteRead(PAYLOAD_FILLER_DATA);
	}
	DESELECT_NRF24L01;                               // Pull up chip select
	NRF24L01_FlushRX();
	NRF24L01_WriteRegisterOneByte(STATUS, (1<<RX_DR));   // Reset status register
	return dataCount;
}


/* Interrupt Service Routines ------------------------------------------------*/
ISR (INT0_vect)
{
	volatile uint8_t status = NRF24L01_GetStatus();
// 	if (_inTxMode)
// 	{
// 		NRF24L01_ResetToRx();
// 	}
	// Data Sent TX FIFO interrupt, asserted when packet transmitted on TX.
	if (status & (1 << TX_DS))
	{
		NRF24L01_PowerUpRx();
	}
	// Maximum number of TX retransmits interrupt
	else if (status & (1 << MAX_RT))
	{
		NRF24L01_PowerUpRx();
	}
}