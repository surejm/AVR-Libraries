/**
 ******************************************************************************
 * @file	uart.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-02-12
 * @brief	Contains functions to manage the SPI-peripheral on ATmegaxxu2 (ATmega32u2 etc)
 *			- Initialization
 *			- Write data
 *			- Buffer functionality if interrupt is used
 *			- Receive data
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <assert/assert.h>
#include <circularBuffer/circularBuffer.h>
#include "uart.h"

/* Private defines -----------------------------------------------------------*/
#define UART_DDR		DDRD
#define UART_PORT		PORTD
#define UART_RX_PIN		PORTD2
#define UART_TX_PIN		PORTD3

/* Private variables ---------------------------------------------------------*/
volatile UART_Status_TypeDef _uartRXstatus;
volatile CircularBuffer_TypeDef _uartBufferRX;
volatile UART_Status_TypeDef _uartTXstatus;
volatile CircularBuffer_TypeDef _uartBufferTX;
uint8_t _uartInitStatus;

/* Private functions ---------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the UART peripheral according to the specified parameters in the UART_InitStruct.
 * @param	UART_InitStruct: pointer to a UART_Init_TypeDef structure that contains
 *			the configuration information for the UART peripheral.
 * @retval	None
 */
void UART_Init(UART_Init_TypeDef *UART_InitStruct)
{
	// Check parameters
	assert_param(IS_UART_BAUD_RATE(UART_InitStruct->UART_BaudRate));
	
	// Set pins
	UART_DDR |= (1 << UART_TX_PIN);
	UART_DDR &= ~(1 << UART_RX_PIN);
	
	_uartRXstatus = UART_RX_BUFFER_EMPTY;
	_uartTXstatus = UART_TX_BUFFER_EMPTY;
	
	UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
	
	if (UART_InitStruct->UART_BaudRate == UART_BAUD_57600 || UART_InitStruct->UART_BaudRate == UART_BAUD_115200)
	{	
		UCSR1A |= (1 << U2X1);
	}		
	UBRR1 = UART_InitStruct->UART_BaudRate;
	
	// Initialize the buffers
	circularBuffer_Init(&_uartBufferRX);
	circularBuffer_Init(&_uartBufferTX);
	
	sei();
	_uartInitStatus = 1;
}

/**
 * @brief	Writes data to the TX buffer and enables the "Data Register Empty" interrupt so that 
 *			the data can be written to the UART
 * @param	Data: data to be written to the UART
 * @retval	None
 */
void UART_Write(const uint8_t Data)
{
	// TODO: Add timemout
	while (circularBuffer_IsFull(&_uartBufferTX));
	circularBuffer_Insert(&_uartBufferTX, Data);
	// Activate the "Data Register Empty" interrupt
	UCSR1B |= (1 << UDRIE1);
}

void UART_WriteString(const char* String)
{
	while (*String != 0x00)
	{
		UART_Write(*String);
		*String++;
	}
}

void UART_WriteUintAsString(uint8_t Number)
{
	char str[4];
	utoa(Number, str, 10);
	UART_WriteString(str);
}

void UART_WriteUint16AsString(uint16_t Number)
{
	char str[8];
	utoa(Number, str, 10);
	UART_WriteString(str);
}

void UART_WriteInt16AsString(int16_t Number)
{
	char str[10];
	itoa(Number, str, 10);
	UART_WriteString(str);
}

void UART_WriteHexByte(uint8_t theByte, uint8_t prefix)
{
	char str[3];
	utoa(theByte, str, 16);
	if (prefix) UART_WriteString("0x");
	UART_WriteString(str);
}

/**
 * @brief	Reads the next value in the RX buffer
 * @param	None
 * @retval	The next value in the buffer that should be removed or [0] if the buffer is empty
 * @none	Before using UART_Read() a call to UART_DataAvailable() should be made to ensure 
 *			there is data to read
 */
uint8_t UART_Read()
{
	if (!circularBuffer_IsEmpty(&_uartBufferRX))
		return circularBuffer_Remove(&_uartBufferRX);
	else
	{
		_uartRXstatus = UART_RX_BUFFER_EMPTY;
		return 0;
	}	
}

/**
 * @brief	Gets the current size of the RX buffer
 * @param	None
 * @retval	The current size of the RX buffer
 */
uint8_t UART_DataAvailable()
{
	return circularBuffer_GetCount(&_uartBufferRX);
}

/**
 * @brief	Gets the current status of the UART
 * @param	None
 * @retval	The current status of the UART
 */
UART_StatusInfo_TypeDef UART_GetStatus()
{
	UART_StatusInfo_TypeDef currentStatus;
	currentStatus.rxStatus = _uartRXstatus;
	currentStatus.txStatus = _uartTXstatus;
	
	return currentStatus;
}

/**
 * @brief	Waits in a while loop until all bytes in the buffer has been transmitted
 * @param	None
 * @retval	None
 */
void UART_WaitForTxComplete()
{
	while (_uartTXstatus == UART_TX_TRANSMITTING);
}

/**
 * @brief	Checks to see if UART has been initialized
 * @param	None
 * @retval	None
 */
uint8_t UART_Initialized()
{
	return _uartInitStatus;
}

/* Interrupt Service Routines ------------------------------------------------*/
/**
 * @brief	Executes when there is unread data present in the receive buffer
 */
ISR(USART1_RX_vect)
{
	uint8_t data = UDR1;
	if (circularBuffer_IsFull(&_uartBufferRX))
		_uartRXstatus = UART_RX_BUFFER_FULL;
	else
	{
		circularBuffer_Insert(&_uartBufferRX, data);
		_uartRXstatus = UART_DATA_IN_RX_BUFFER;
	}
}

/**
 * @brief	Executes when the data register (UDR1) is empty
 */
ISR(USART1_UDRE_vect)
{
	if (circularBuffer_IsEmpty(&_uartBufferTX))
	{
		// No data to transmit so disable "Data Register Empty" interrupt
		UCSR1B &= ~(1 << UDRIE1);
		_uartTXstatus = UART_TX_BUFFER_EMPTY;
	}
	else
	{
		// Data is available so remove it from the buffer and transmit
		_uartTXstatus = UART_TX_TRANSMITTING;
		UDR1 = circularBuffer_Remove(&_uartBufferTX);
	}
}