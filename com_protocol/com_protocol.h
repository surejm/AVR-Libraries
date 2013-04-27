/**
 ******************************************************************************
 * @file	com_protocol.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-02-12
 * @brief	Contains typedefs and functions prototypes to manage the communication
 *			protocol
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef COM_PROTOCOL_H_
#define COM_PROTOCOL_H_

/* Includes ------------------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
#define START_BYTES					0xAABBCC
#define START_BYTE_1				0xAA
#define START_BYTE_2				0xBB
#define START_BYTE_3				0xCC

#define IS_VALID_COMMAND(COMMAND) ((COMMAND) < NUMBER_OF_COMMANDS)

/* Typedefs ------------------------------------------------------------------*/
/**
 * @brief	Typedef for a pointer to a function that takes a pointer to an array of uint8_t data as an argument
 */
typedef void (*task)(uint8_t*);

/**
 * @brief	Typedef for a command with this information:
 *			-commandId: ID for the command
 *			-bytesToReceive: Amount of byte the command is expecting to receive
 *			-commandTask: Pointer to the task which the command should do
 */
typedef struct
{
	uint16_t commandId;
	uint8_t bytesToReceive;
	task commandTask;
} Command_TypeDef;

/* Function prototypes -------------------------------------------------------*/
void COM_PROTOCOL_Init();
void COM_PROTOCOL_CheckForIncomingCommand();
uint8_t COM_PROTOCOL_AddCommand(uint16_t CommandId, uint8_t BytesToReceive, task CommandTask);

#endif /* COM_PROTOCOL_H_ */