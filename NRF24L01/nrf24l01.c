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

/* 
 * Put a file named "board.h" in the project root directory and add appropriate 
 * board from the boards folder at AVR/boards.
 * In that file we define CE_PIN, CSN_PIN etc and also include spi and uart 
 * library for the board in use
 */
#include <board.h>

#include <circularBuffer/circularBuffer.h>
#include <MILLIS_COUNT/millis_count.h>
#include "nrf24l01_register_map.h"
#include "nrf24l01.h"

/* Private defines -----------------------------------------------------------*/
#if !defined(CE_PIN) && !defined(CE_DDR) && !defined(CE_PORT)
#error "Please define CE_PIN, CE_DDR, CE_PORT in board.h"
#endif
// #define CE_PIN			PORTD4
// #define CE_DDR			DDRD
// #define CE_PORT			PORTD
#define ENABLE_RF		(CE_PORT |= (1 << CE_PIN))
#define DISABLE_RF		(CE_PORT &= ~(1 << CE_PIN))

#if !defined(CSN_PIN) && !defined(CSN_DDR) && !defined(CSN_PORT)
#error "Please define CSN_PIN, CSN_DDR, CSN_PORT in board.h"
#endif
// #define CSN_PIN			PORTD3
// #define CSN_DDR			DDRD
// #define CSN_PORT		PORTD
#define SELECT_NRF24L01		(CSN_PORT &= ~(1 << CSN_PIN))
#define DESELECT_NRF24L01	(CSN_PORT |= (1 << CSN_PIN))


#if !defined(IRQ_PIN) && !defined(IRQ_DDR) && !defined(IRQ_PORT) && !defined(INTERRUPT_VECTOR) && !defined(IRQ_INTERRUPT)
#error "Please define IRQ_PIN, IRQ_DDR, IRQ_PORT, INTERRUPT_VECTOR, IRQ_INTERRUPT in board.h"
#endif
// #define IRQ_PIN				PORTD2	// INT0
// #define IRQ_DDR				DDRD
// #define IRQ_PORT			PORTD
// #define INTERRUPT_VECTOR	INT1_vect
// #define IRQ_INTERRUPT		1

#define TX_POWERUP			NRF24L01_WriteRegisterOneByte(CONFIG, CONFIG_VALUE | ((1 << PWR_UP) | (0 << PRIM_RX)))
#define RX_POWERUP			NRF24L01_WriteRegisterOneByte(CONFIG, CONFIG_VALUE | ((1 << PWR_UP) | (1 << PRIM_RX)))
#define RESET_STATUS		NRF24L01_WriteRegisterOneByte(STATUS, (1 << TX_DS) | (1 << MAX_RT))
#define RESET_STATUS_RX_DR  NRF24L01_WriteRegisterOneByte(STATUS, (1 << RX_DR))
#define RESET_STATUS_ALL	NRF24L01_WriteRegisterOneByte(STATUS, (1 << TX_DS) | (1 << MAX_RT) | (1 << RX_DR))

#define IS_VALID_PIPE(PIPE)	((PIPE) < 6)
#define GetPipeNumber (((NRF24L01_GetStatus()) & 0xF) >> 1)
#define GetPipeFromStatus(THE_STATUS) (((THE_STATUS) & 0xF) >> 1)

#define DEFAULT_CHANNEL		66

#define CONFIG_VALUE		((1 << EN_CRC) | (0 << CRCO))

#define TX_MODE_TIMEOUT		100

#define MAX_PIPES			6

/* Private variables ---------------------------------------------------------*/
volatile uint8_t _inTxMode;
volatile CircularBuffer_TypeDef _rxPipeBuffer[6];
volatile uint16_t _checksumErrors;
uint16_t _resetCount;

/* Private Function Prototypes -----------------------------------------------*/
static void NRF24L01_WriteRegisterOneByte(uint8_t Register, uint8_t Data);
static void NRF24L01_ReadRegister(uint8_t Register, uint8_t* Storage, uint8_t ByteCount);
static void NRF24L01_WriteRegister(uint8_t Register, uint8_t * Data, uint8_t ByteCount);

static void NRF24L01_FlushTX();
static void NRF24L01_FlushRX();
static void NRF24L01_ResetToRx();

static uint8_t NRF24L01_GetData(uint8_t* Storage);
static uint8_t NRF24L01_DataReady();

static void NRF24L01_PowerUpRx();
static void NRF24L01_PowerUpTx();


/* Functions -----------------------------------------------------------------*/

/**
 * @brief	Initializes the nRF24L01
 * @param	None
 * @retval	0: Initialization failed
 * @retval	1: Initialization succeeded
 */
void NRF24L01_Init() 
{
	_checksumErrors = 0;
	_resetCount = 0;
	
	for (uint8_t i = 0; i < MAX_PIPES; i++)
	{
		CIRCULAR_BUFFER_Init(&_rxPipeBuffer[i]);
	}
	
	CE_DDR |= (1 << CE_PIN);
	CSN_DDR |= (1 << CSN_PIN);
	DISABLE_RF;
	DESELECT_NRF24L01;

	// Interrupt init
	IRQ_DDR &= ~(1 << IRQ_PIN);
	#if (IRQ_INTERRUPT == 0)
	EICRA &= ~((1 << ISC00) | (1 << ISC01));
	EICRA |= (1 << ISC01); // The falling edge of INT0 generates an interrupt request.
	EIMSK |= (1 << INT0);
	
	#elif (IRQ_INTERRUPT == 1)
	EICRA &= ~((1 << ISC10) | (1 << ISC11));
	EICRA |= (1 << ISC11); // The falling edge of INT1 generates an interrupt request.
	EIMSK |= (1 << INT1);
	#endif
	
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
	NRF24L01_WriteRegisterOneByte(RX_PW_P1, PAYLOAD_SIZE);
	NRF24L01_WriteRegisterOneByte(RX_PW_P2, PAYLOAD_SIZE);
	NRF24L01_WriteRegisterOneByte(RX_PW_P3, PAYLOAD_SIZE);
	NRF24L01_WriteRegisterOneByte(RX_PW_P4, PAYLOAD_SIZE);
	NRF24L01_WriteRegisterOneByte(RX_PW_P5, PAYLOAD_SIZE);
	
	// Enable all RX pipes
	NRF24L01_EnablePipes(ALL_PIPES);

	// Flush buffers
	NRF24L01_FlushTX();
	NRF24L01_FlushRX();
	RESET_STATUS_ALL;

	// Start receiver
	_inTxMode = 0;        // Start in receiving mode
	RX_POWERUP;     // Power up in receiving mode
	ENABLE_RF;     // Listening for pakets
}

/**
 * @brief	Write data and send it to the address specified in TX_ADDR
 * @param	Data: Pointer to where the data is stored
 * @param	ByteCount: The number of bytes in Data
 * @retval	None
 * @note	W_TX_PAYLOAD commands wants LSByte first but Data is MSByte so the for loop is backwards
 *			Send as much data as defined in the payload
 */
void NRF24L01_WritePayload(uint8_t* Data, uint8_t DataCount)
{
	// You can only send the amount of data specified in MAX_DATA_COUNT
	if (DataCount > MAX_DATA_COUNT) return;

	// Wait until last payload is sent or timeout has occurred
	uint16_t timeoutTimer = millis16bit();
    while (_inTxMode)
	{
		if (millis16bit() - timeoutTimer >= TX_MODE_TIMEOUT) NRF24L01_ResetToRx();
	}
	
	uint8_t checksum = NRF24L01_GetChecksum(Data, DataCount);
	
    DISABLE_RF;
	NRF24L01_PowerUpTx();
	NRF24L01_FlushTX();
    
    SELECT_NRF24L01;
    SPI_Write(W_TX_PAYLOAD);
	SPI_Write(DataCount);	// Write data count
	uint8_t i;
	for (i = 0; i < DataCount; i++) SPI_Write(Data[i]);		// Write data
	SPI_Write(checksum);	// Write checksum
	for (i++; i <= MAX_DATA_COUNT; i++) SPI_Write(PAYLOAD_FILLER_DATA);	// Fill the rest of the payload
    DESELECT_NRF24L01;
    
    ENABLE_RF;
}


/**
 * @brief	Write data of arbitrary length [0-255]
 * @param	Data: Pointer to where the data is stored
 * @param	ByteCount: The number of bytes in Data
 * @retval	None
 */
void NRF24L01_Write(uint8_t* Data, uint8_t DataCount)
{
	// Only copy to good-sized parts if necessary
	if (DataCount <= MAX_DATA_COUNT)
	{
		NRF24L01_WritePayload(Data, DataCount);
	}
	else
	{
		uint8_t tempData[MAX_DATA_COUNT];
		uint8_t tempDataCount = 0;
		uint8_t payloadCount = 0;
			
		for (uint16_t i = 0; i < DataCount; i++)
		{
			tempData[i - payloadCount * (MAX_DATA_COUNT)] = Data[i];
			tempDataCount++;
				
			if (tempDataCount == MAX_DATA_COUNT)
			{
				NRF24L01_WritePayload(tempData, tempDataCount);
				tempDataCount = 0;
				payloadCount += 1;
				
				// ERROR: Doesn't seem to work if DataCount > 60
			}
		}
			
		// If it wasn't a multiple of MAX_DATA_COUNT write the rest
		if (tempDataCount)
		{
			NRF24L01_WritePayload(Data, tempDataCount);
		}
	}
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
		uint8_t status = SPI_WriteRead(W_REGISTER | (RX_ADDR_P0 + Pipe));
		SPI_Write(AddressLSByte);
		for (uint8_t i = 0; i < 4; i++)
		{
			SPI_Write((AddressMSBytes >> 8*i) & 0xFF);
		}
		DESELECT_NRF24L01;
		
		return status;
	}
	else if (IS_VALID_PIPE(Pipe))
	{
		SELECT_NRF24L01;
		uint8_t status = SPI_WriteRead(W_REGISTER | (RX_ADDR_P0 + Pipe));
		SPI_Write(AddressLSByte);	// MSByte of address in pipe 2 to 5 is equal to RX_ADDR_P1[39:8]
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
 * @brief	Enable the specified pipes on nRF24L01
 * @param	Pipes: The pipes that should be enabled
 * @retval	The status register
 */
void NRF24L01_EnablePipes(uint8_t Pipes)
{
	if (Pipes <= 0x3F)
	{
		uint8_t pipeValue;
		NRF24L01_ReadRegister(EN_RXADDR, &pipeValue, 1);
		
		pipeValue |= (Pipes);
		NRF24L01_WriteRegisterOneByte(EN_RXADDR, pipeValue);
	}
}

/**
 * @brief	Disable the specified pipes on nRF24L01
 * @param	Pipes: The pipes that should be disabled
 * @retval	The status register
 */
void NRF24L01_DisablePipes(uint8_t Pipes)
{
	if (Pipes <= 0x3F)
	{
		uint8_t pipeValue;
		NRF24L01_ReadRegister(EN_RXADDR, &pipeValue, 1);
		pipeValue &= ~(Pipes);
		
		NRF24L01_WriteRegisterOneByte(EN_RXADDR, pipeValue);
	}
}

/**
 * @brief	Get the pipe which there's data on
 * @param	None
 * @retval	The pipe number
 */
uint8_t NRF24L01_GetPipeNumber()
{
	return GetPipeNumber;
}

/**
 * @brief	Check if a pipe has available data in it's buffer
 * @param	Pipe: The pipe to check for data
 * @retval	The available data for the specified pipe
 */
uint8_t NRF24L01_GetAvailableDataForPipe(uint8_t Pipe)
{
	if (IS_VALID_PIPE(Pipe))
	{
		return CIRCULAR_BUFFER_GetCount(&_rxPipeBuffer[Pipe]);
	}
	return 0;
}

/**
 * @brief	Get a certain amount of data from a specified pipe
 * @param	Pipe: The pipe to check for data
 * @param	Storage: Pointer to where the data should be stored
 * @param	DataCount: The amount of data to get
 * @retval	None
 * @note	It's good to check that the requested amount of data is available
			first by calling NRF24L01_GetAvailableDataForPipe()
 */
void NRF24L01_GetDataFromPipe(uint8_t Pipe, uint8_t* Storage, uint8_t DataCount)
{
	if (IS_VALID_PIPE(Pipe))
	{
		for (uint8_t i = 0; i < DataCount; i++)
		{
			Storage[i] = CIRCULAR_BUFFER_Remove(&_rxPipeBuffer[Pipe]);
		}
	}
}

/**
 * @brief	Get the checksum for a package of data
 * @param	Data: The data for which to calculate the checksum on
 * @param	DataCount: The amount of data
 * @retval	The calculated checksum
 * @note	Checksum = ~(DataCount + Data1 + Data2 + ... + Data N)
 */
uint8_t NRF24L01_GetChecksum(uint8_t* Data, uint8_t DataCount)
{
	volatile uint8_t checksum = DataCount;
	for (uint8_t i = 0; i < DataCount; i++)
	{
		checksum += Data[i];
	}
	return ~checksum;
}

/**
 * @brief	Get the amount of checksum errors that has occurred
 * @param	None
 * @retval	The amount of checksum errors
 */
uint16_t NRF24L01_GetChecksumErrors()
{
	return _checksumErrors;
}

/**
 * @brief	Get the number of times the nRF24L01 has been reset since start
 * @param	None
 * @retval	None
 * @note	Can be used to see if there's problems. If two devices transmit on the same
			address the nRF24L01 will hang and the library will do a reset
 */
uint16_t NRF24L01_GetResetCount()
{
	return _resetCount;
}

#ifdef NRF24L01_UART_DEBUG
/**
 * @brief	Write some debug info to the UART
 * @param	None
 * @retval	None
 */
void NRF24L01_WriteDebugToUart()
{
	UART_WriteString("------------\r");
	uint8_t status = NRF24L01_GetStatus();
	UART_WriteString("Status: ");
	UART_WriteHexByte(status, 1);
	UART_WriteString("\r");
	
	uint8_t pipe = NRF24L01_GetPipeNumber();
	UART_WriteString("Pipe: ");
	UART_WriteUintAsString(pipe);
	UART_WriteString("\r");
	
	uint8_t en_rxaddr;
	NRF24L01_ReadRegister(EN_RXADDR, &en_rxaddr, 1);
	UART_WriteString("EN_RXADDR: ");
	UART_WriteHexByte(en_rxaddr, 1);
	UART_WriteString("\r");

	uint8_t addrTx[5];
	NRF24L01_ReadRegister(TX_ADDR, addrTx, 5);
	UART_WriteString("TX_ADDR: ");
	UART_WriteHexByte(addrTx[0], 1);
	UART_WriteHexByte(addrTx[1], 0);
	UART_WriteHexByte(addrTx[2], 0);
	UART_WriteHexByte(addrTx[3], 0);
	UART_WriteHexByte(addrTx[4], 0);
	UART_WriteString("\r");

	uint8_t addr0[5];
	NRF24L01_ReadRegister(RX_ADDR_P0, addr0, 5);
	UART_WriteString("RX_ADDR_P0: ");
	UART_WriteHexByte(addr0[0], 1);
	UART_WriteHexByte(addr0[1], 0);
	UART_WriteHexByte(addr0[2], 0);
	UART_WriteHexByte(addr0[3], 0);
	UART_WriteHexByte(addr0[4], 0);
	UART_WriteString("\r");

	uint8_t addr1[5];
	NRF24L01_ReadRegister(RX_ADDR_P1, addr1, 5);
	UART_WriteString("RX_ADDR_P1: ");
	UART_WriteHexByte(addr1[0], 1);
	UART_WriteHexByte(addr1[1], 0);
	UART_WriteHexByte(addr1[2], 0);
	UART_WriteHexByte(addr1[3], 0);
	UART_WriteHexByte(addr1[4], 0);
	UART_WriteString("\r");

	uint8_t addr2;
	NRF24L01_ReadRegister(RX_ADDR_P2, &addr2, 1);
	UART_WriteString("RX_ADDR_P2: ");
	UART_WriteHexByte(addr2, 1);
	UART_WriteString("\r");

	uint8_t addr3;
	NRF24L01_ReadRegister(RX_ADDR_P3, &addr3, 1);
	UART_WriteString("RX_ADDR_P3: ");
	UART_WriteHexByte(addr3, 1);
	UART_WriteString("\r");

	uint8_t addr4;
	NRF24L01_ReadRegister(RX_ADDR_P4, &addr4, 1);
	UART_WriteString("RX_ADDR_P4: ");
	UART_WriteHexByte(addr4, 1);
	UART_WriteString("\r");

	uint8_t addr5;
	NRF24L01_ReadRegister(RX_ADDR_P5, &addr5, 1);
	UART_WriteString("RX_ADDR_P5: ");
	UART_WriteHexByte(addr5, 1);
	UART_WriteString("\r");
}
#endif /* NRF24L01_UART_DEBUG */

/* Private Functions ---------------------------------------------------------*/
/**
 * @brief	Write one byte to a register in the nRF24L01
 * @param	Register: The register to read from
 * @param	Data: The byte to write
 * @retval	None
 */
static void NRF24L01_WriteRegisterOneByte(uint8_t Register, uint8_t Data)
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
static void NRF24L01_ReadRegister(uint8_t Register, uint8_t* Storage, uint8_t ByteCount)
{
	if (IS_VALID_REGISTER(Register))
	{
		SELECT_NRF24L01;
		SPI_Write(R_REGISTER | Register);
		for (uint8_t i = 0; i < ByteCount; i++)
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
static void NRF24L01_WriteRegister(uint8_t Register, uint8_t* Data, uint8_t ByteCount)
{
	if (IS_VALID_REGISTER(Register))
	{
		SELECT_NRF24L01;
		SPI_Write(W_REGISTER | Register);
		for (uint8_t i = 0; i < PAYLOAD_SIZE; i++)
		{
			SPI_WriteRead(Data[i]);
		}
		DESELECT_NRF24L01;
	}	
}

/**
 * @brief	Flush the TX buffer
 * @param	None
 * @retval	None
 */
static void NRF24L01_FlushTX()
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
static void NRF24L01_FlushRX()
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
static void NRF24L01_ResetToRx()
{
	_resetCount++;
	
	RESET_STATUS_ALL;
	DISABLE_RF;
	DESELECT_NRF24L01;

	// Start receiver
	_inTxMode = 0;	// Start in receiving mode
	RX_POWERUP;		// Power up in receiving mode
	ENABLE_RF;		// Listening for pakets
}

/**
 * @brief	Gets data from the RX buffer
 * @param	Storage: Pointer to where the data should be stored
 * @retval	The amount of data received
 * @note	The checksum is checked here and if it doesn't match it will return 0 as
 *			the amount of data
 */
static uint8_t NRF24L01_GetData(uint8_t* Storage)
{	
	SELECT_NRF24L01;
	SPI_WriteRead(R_RX_PAYLOAD);
	uint8_t dataCount = SPI_WriteRead(PAYLOAD_FILLER_DATA);	
	for (uint8_t i = 0; i < dataCount + 1; i++)
	{
		Storage[i] = SPI_WriteRead(PAYLOAD_FILLER_DATA);
	}	
	DESELECT_NRF24L01;
	NRF24L01_FlushRX();
	NRF24L01_WriteRegisterOneByte(STATUS, (1 << RX_DR));
	
	return dataCount;
}

/**
 * @brief	Checks if there is data available
 * @param	None
 * @retval	0: If no data is ready
 * @retval	1: If data is ready
 */
static uint8_t NRF24L01_DataReady()
// Checks if data is available for reading
{
	static uint16_t timeoutTimer = 0;
	if (_inTxMode)
	{
		if (millis16bit() - timeoutTimer >= TX_MODE_TIMEOUT) NRF24L01_ResetToRx();
		return 0;
	}
	
	volatile uint8_t status = NRF24L01_GetStatus();
	return (status & (1 << RX_DR));
}

/**
 * @brief	Power up in RX mode
 * @param	None
 * @retval	None
 */
static void NRF24L01_PowerUpRx()
{
	_inTxMode = 0;
	NRF24L01_WriteRegisterOneByte(CONFIG, CONFIG_VALUE | ((1 << PWR_UP) | (1 << PRIM_RX)));
	NRF24L01_WriteRegisterOneByte(STATUS, (1 << TX_DS) | (1 << MAX_RT));
	//_delay_us(50);		// NOT GOOD!!!
}

/**
 * @brief	Power up in TX mode
 * @param	None
 * @retval	None
 */
static void NRF24L01_PowerUpTx()
{
	_inTxMode = 1;
	NRF24L01_WriteRegisterOneByte(CONFIG, CONFIG_VALUE | ((1 << PWR_UP) | (0 << PRIM_RX)));
}

/* Interrupt Service Routines ------------------------------------------------*/
ISR(INTERRUPT_VECTOR)
{
	volatile uint8_t status = NRF24L01_GetStatus();
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
	else if (status & (1 << RX_DR))
	{
		volatile uint8_t pipe = GetPipeFromStatus(status);
		if (IS_VALID_PIPE(pipe))
		{
			uint8_t buffer[MAX_DATA_COUNT];
			uint8_t availableData = NRF24L01_GetData(buffer);
			
			uint8_t receivedChecksum = buffer[availableData];
			uint8_t calculatedChecksum = NRF24L01_GetChecksum(buffer, availableData);
			
			if (receivedChecksum == calculatedChecksum)
			{
				for (uint8_t i = 0; i < availableData; i++)
				{
					if (!CIRCULAR_BUFFER_IsFull(&_rxPipeBuffer[pipe]))
						CIRCULAR_BUFFER_Insert(&_rxPipeBuffer[pipe], buffer[i]);
				}
			}
			else
			{
				// Checksum error
				_checksumErrors++;
			}
		}
		RESET_STATUS_RX_DR;
	}
}