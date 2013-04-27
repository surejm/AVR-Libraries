/**
 ******************************************************************************
 * @file	com_protocol.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-02-12
 * @brief	Contains the functions to manage the communication protocol
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <assert/assert.h>
#include "com_protocol.h"

// Include the type of I/O you want to use to communicate
#include <atmega328x/uart.h>

/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifndef NUMBER_OF_COMMANDS
#error "Please define NUMBER_OF_COMMANDS in project properties"
#endif
Command_TypeDef _commands[NUMBER_OF_COMMANDS];

/* Private functions ---------------------------------------------------------*/
static uint8_t doCommand(uint16_t CommandId, uint8_t* Data);
static uint8_t getBytesToReceiveForCommand(uint16_t CommandId);

/* Functions -----------------------------------------------------------------*/
/**
 * @brief	...
 * @param	...
 * @retval	...
 */
void COM_PROTOCOL_Init()
{
	// Initialize your communication I/O
	UART_Init_TypeDef my_UART_Init;
	my_UART_Init.UART_BaudRate = UART_BAUD_57600;
	UART_Init(&my_UART_Init);
	UART_WriteString("UART Done\r");
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
void COM_PROTOCOL_CheckForIncomingCommand()
{
	if (UART_DataAvailable() >= 5)
	{
		volatile uint8_t start = UART_Read();
		if (start != START_BYTE_1) return;
		start = UART_Read();
		if (start != START_BYTE_2) return;
		start = UART_Read();
		if (start != START_BYTE_3) return;
		
		volatile uint16_t command = UART_Read();
		command = command << 8;
		command |= UART_Read();
		
		if (!IS_VALID_COMMAND(command)) return;
		
		volatile uint8_t bytesToReceive = getBytesToReceiveForCommand(command);
		
		if (bytesToReceive == 0)
		{
			// Do command
			return;
		}
		
		// Wait for data bytes to be received
		while (UART_DataAvailable() < bytesToReceive);
		
		volatile uint8_t receivedData[bytesToReceive];
		for (uint8_t i = 0; i < bytesToReceive; i++)
		{
			receivedData[i] = UART_Read();
		}
		
		// Do command with data
		doCommand(command, &receivedData);
		
		
		// DEBUG: Send command and data back
// 		UART_Write((command >> 8) & 0xFF);
// 		UART_Write(command & 0xFF);
//
// 		for (uint8_t i = 0; i < bytesToReceive; i++)
// 		{
// 			UART_Write(receivedData[i]);
// 		}
	}
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
uint8_t COM_PROTOCOL_AddCommand(uint16_t CommandId, uint8_t BytesToReceive, task CommandTask)
{
	if (CommandId < NUMBER_OF_COMMANDS)
	{
		Command_TypeDef newCommand;
		newCommand.commandId = CommandId;
		newCommand.bytesToReceive = BytesToReceive;
		newCommand.commandTask = CommandTask;
		
		_commands[CommandId] = newCommand;
		return 1;
	}
	else
		return 0;
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
uint8_t doCommand(uint16_t CommandId, uint8_t* Data)
{
	if (CommandId < NUMBER_OF_COMMANDS)
	{
		_commands[CommandId].commandTask(Data);
		return 1;
	}
	else
		return 0;
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
uint8_t getBytesToReceiveForCommand(uint16_t CommandId)
{
	if (CommandId < NUMBER_OF_COMMANDS)
		return _commands[CommandId].bytesToReceive;
	else
		return 0;
}

/* Interrupt Service Routines ------------------------------------------------*/
/**
 * @brief	...
 */