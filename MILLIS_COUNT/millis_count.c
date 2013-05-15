/**
 ******************************************************************************
 * @file	millis_count.c
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-03-14
 * @brief	Contains functions to manage a millis counter
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "millis_count.h"

/* Private defines -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint32_t _millisCounter;
uint8_t _milliCountInitStatus;
/* Private functions ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

/**
 * @brief	Initializes the millisecond counter
 * @param	None
 * @retval	None
 */
void MILLIS_COUNT_Init()
{
	_millisCounter = 0;
	TCCR1B = (1 << CS11) | (1 << WGM12);	// clk/8
	TIMSK1 = (1 << OCIE1A);
		
	// 1 ms per interrupt
	#if F_CPU == 8000000
	OCR1A = 1000 - 1;
	#elif F_CPU == 16000000
	OCR1A = 2000 - 1;
	#endif
		
	sei();
	_milliCountInitStatus = 1;
}

/**
 * @brief	Returns the current value of the millisecond counter
 * @param	None
 * @retval	Current value of _millisCounter
 */
uint32_t millis()
{
	uint32_t counterCopy;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		counterCopy = _millisCounter;
	}
	return counterCopy;
}

/**
 * @brief	Returns the 2 LSB bytes of the current value of the millisecond counter
 * @param	None
 * @retval	2 LSB bytes of current value of _millisCounter
 */
uint16_t millis16bit()
{
	return (uint16_t)millis();
}

/**
 * @brief	Checks to see if MILLIS_COUNT has been initialized
 * @param	None
 * @retval	1 if initialized
 * @retval	0 if uninitialized
 */
uint8_t MILLIS_COUNT_Initialized()
{
	return _milliCountInitStatus;
}


/* Interrupt Service Routines ------------------------------------------------*/
/**
 * @brief	Will increase the counter by 1 every millisecond
 */
ISR(TIMER1_COMPA_vect)
{
	_millisCounter++;
}