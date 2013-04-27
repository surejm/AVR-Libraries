/**
 ******************************************************************************
 * @file	millis_count.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-03-14
 * @brief	Contains function prototypes to manage a millisecond counter
 * @note	Relies on TIMER1 so it should not be used anywhere else
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MILLIS_COUNT_H_
#define MILLIS_COUNT_H_

/* Includes ------------------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
#ifdef TIMER1_IN_USE
#error "Milliscount need TIMER1. Used somewhere else"
#else
#define TIMER1_IN_USE
#endif

/* Typedefs ------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/
void MILLIS_COUNT_Init();
uint32_t millis();
uint16_t millis16bit();
uint8_t MILLIS_COUNT_Initialized();

#endif /* MILLIS_COUNT_H_ */