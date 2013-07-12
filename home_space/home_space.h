/**
 ******************************************************************************
 * @file	home_space.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-05-30
 * @brief	Contains typedefs and functions prototypes to manage the Home Space
 *			communication protocol
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef HOME_SPACE_H_
#define HOME_SPACE_H_

/* Includes ------------------------------------------------------------------*/
#include "home_space_info.h"

/* Defines -------------------------------------------------------------------*/
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
void HOME_SPACE_Init();
void HOME_SPACE_Check();
uint8_t HOME_SPACE_AddCommand(uint16_t CommandId, uint8_t BytesToReceive, task CommandTask);

#endif /* HOME_SPACE_H_ */