/**
 ******************************************************************************
 * @file	timer.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-02-14
 * @brief	Contains functions to manage the TIMER-peripheral on ATmega328x
 *			- Initialization
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <assert/assert.h>
#include "timer.h"

/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Functions -----------------------------------------------------------------*/
/**
 * @brief	Initializes the UART peripheral according to the specified parameters in the UART_InitStruct.
 * @param	UART_InitStruct: pointer to a UART_Init_TypeDef structure that contains
 *			the configuration information for the UART peripheral.
 * @retval	None
 */
void TIMER_Init(TIMER_TypeDef TIMERx, TIMER_Init_TypeDef *TIMER_InitStruct)
{
	// Check parameters
	assert_param(IS_TIMER(TIMERx));
	assert_param(IS_TIMER_MODE(TIMER_InitStruct->mode));
}

/* Interrupt Service Routines ------------------------------------------------*/