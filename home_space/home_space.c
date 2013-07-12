/**
 ******************************************************************************
 * @file	home_space.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-30
 * @brief	Contains the functions to manage the Home Space Communication Protocol
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <NRF24L01/nrf24l01.h>
#include <board.h>
#include "home_space.h"
#include "home_space_info.h"

/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
Command_TypeDef _commands[NUMBER_OF_COMMANDS];

/* Private functions ---------------------------------------------------------*/
static uint8_t doCommand(uint16_t CommandId, uint8_t* Data);
static uint8_t getBytesToReceiveForCommand(uint16_t CommandId);

static void sendDownToPipe(uint8_t* InfoBytes, uint8_t* DataBytes, uint8_t Pipe);
static void sendUp(uint8_t* InfoBytes, uint8_t* DataBytes);

/* Functions -----------------------------------------------------------------*/
/**
 * @brief	...
 * @param	...
 * @retval	...
 */
void HOME_SPACE_Init()
{
	NRF24L01_Init();
	NRF24L01_SetAddressWidth(HOME_SPACE_ADDRESS_WIDTH);
	NRF24L01_SetRFChannel(HOME_SPACE_RF_CHANNEL);
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
void HOME_SPACE_Check()
{
	for (uint8_t i = 0; i < 6; i++)
	{
		if (NRF24L01_GetAvailableDataForPipe(i) >= HOME_SPACE_MIN_BYTES)
		{
			// Process
			uint8_t infoBytes[HOME_SPACE_MIN_BYTES];
			NRF24L01_GetDataFromPipe(i, infoBytes, HOME_SPACE_MIN_BYTES);
			
			uint32_t destination = ((uint32_t)infoBytes[0] << 16) | ((uint32_t)infoBytes[1] << 8) | ((uint32_t)infoBytes[2]);
			uint32_t source = ((uint32_t)infoBytes[3] << 16) | ((uint32_t)infoBytes[4] << 8) | ((uint32_t)infoBytes[5]);
			uint16_t command = ((uint16_t)infoBytes[6] << 8) | ((uint16_t)infoBytes[7]);
			uint8_t dataCount = infoBytes[8];
			
			/* Wait for data */
			while (NRF24L01_GetAvailableDataForPipe(i) < dataCount);
			uint8_t dataBytes[dataCount];
			NRF24L01_GetDataFromPipe(i, dataBytes, dataCount);
			
			/* Case A:
			 * The destination is this board's address -> process the command
			 */
			if (destination == BOARD_ADDRESS)
			{
				
			}
			/* Case B:
			 * Destination depth is the same as this board's -> Send up
			 */
			else if (ADDRESS_DEPTH(destination) == ADDRESS_DEPTH(BOARD_ADDRESS))
			{
				sendUp(infoBytes, dataBytes);
			}
			/* Case C:
			 * Destination is 1 step deeper -> Send down
			 */
			else if (ADDRESS_DEPTH(destination) == ADDRESS_DEPTH(BOARD_ADDRESS) - 1)
			{
				uint8_t pipeToSendTo = ADDRESS_WIDTH(destination) - (ADDRESS_DEPTH(BOARD_ADDRESS) - 1) * 5;
				sendDownToPipe(infoBytes, dataBytes, pipeToSendTo);
			}
			/* Case D:
			 * Destination is 2 steps deeper -> Send down
			 */
			else if (ADDRESS_DEPTH(destination) == ADDRESS_DEPTH(BOARD_ADDRESS) - 2)
			{
				uint8_t pipeToSendTo = (ADDRESS_WIDTH(destination) - 1) / 5 + 1;
				sendDownToPipe(infoBytes, dataBytes, pipeToSendTo);
			}
			/* Case E:
			 * Destination is deeper but wrong width for this board to handle -> Send up
			 */
			else if (ADDRESS_WIDTH(destination) > ADDRESS_WIDTH(BOARD_ADDRESS) &&
					(ADDRESS_DEPTH(destination) > ADDRESS_DEPTH(BOARD_ADDRESS) * 5 || ADDRESS_DEPTH(destination) <= ADDRESS_DEPTH(BOARD_ADDRESS) * 5 - 5))
			{
				sendUp(infoBytes, dataBytes);
			}
			/* Case Error:
			 * This should not happen!
			 */
			else
			{
				
			}
		}
	}
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
uint8_t HOME_SPACE_AddCommand(uint16_t CommandId, uint8_t BytesToReceive, task CommandTask)
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
static uint8_t doCommand(uint16_t CommandId, uint8_t* Data)
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
static uint8_t getBytesToReceiveForCommand(uint16_t CommandId)
{
	if (CommandId < NUMBER_OF_COMMANDS)
		return _commands[CommandId].bytesToReceive;
	else
		return 0;
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
static void sendDownToPipe(uint8_t* InfoBytes, uint8_t* DataBytes, uint8_t Pipe)
{
	
}

/**
 * @brief	...
 * @param	...
 * @retval	...
 */
static void sendUp(uint8_t* InfoBytes, uint8_t* DataBytes)
{
	
}


/* Interrupt Service Routines ------------------------------------------------*/
/**
 * @brief	...
 */