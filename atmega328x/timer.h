/**
 ******************************************************************************
 * @file	timer.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-02-14
 * @brief	Contains function prototypes, constants to manage the Timer-peripheral
 *			on ATmega328x
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TIMER_H_
#define TIMER_H_

/* Includes ------------------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/**
 * @brief	Timer peripheral
 * @note	TIMER_0 & TIMER_2 is 8-bit, TIMER_1 is 16-bit
 */						
typedef enum
{
	TIMER_0 =	0x00,
	TIMER_1 =	0x01,
	TIMER_2 =	0x02
} TIMER_TypeDef;
#define IS_TIMER(TIMER) (((TIMER) == TIMER_0) || ((TIMER) == TIMER_1) || ((TIMER) == TIMER_2))

/**
 * @brief  Timer Init structure definition
 */
typedef enum
{
	TIMER_NORMAL_MODE =						0x00,
	TIMER_PWM_PHASE_CORRECT_0xFF_TOP_MODE =	0x01,
	TIMER_CTC_MODE =						0x02,
	TIMER_FAST_PWM_0xFF_TOP_MODE =			0x03,
	TIMER_PWM_PHASE_CORRECT_OCRA_TOP_MODE =	0x05,
	TIMER_FAST_PWM_OCRA_TOP_MODE =			0x07
	
} TIMER_Mode_TypeDef;
#define IS_TIMER_MODE(MODE) (((MODE) == TIMER_NORMAL_MODE) || ((MODE) == TIMER_PWM_PHASE_CORRECT_0xFF_TOP_MODE) || \
							((MODE) == TIMER_CTC_MODE) || ((MODE) == TIMER_FAST_PWM_0xFF_TOP_MODE) || \
							((MODE) == TIMER_PWM_PHASE_CORRECT_OCRA_TOP_MODE) || ((MODE) == TIMER_FAST_PWM_OCRA_TOP_MODE))

/**
 * @brief  Timer Init structure definition
 */
typedef struct
{
	TIMER_Mode_TypeDef mode;	/** Specifies which mode the timer should be used in
									This parameter can be any value of TIMER_Mode_TypeDef */
} TIMER_Init_TypeDef;

/* Function prototypes -------------------------------------------------------*/
void TIMER_Init(TIMER_TypeDef TIMERx, TIMER_Init_TypeDef *TIMER_InitStruct);

#endif /* TIMER_H_ */