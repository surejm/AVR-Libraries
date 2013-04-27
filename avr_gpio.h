/**
 ******************************************************************************
 * @file    avr_gpio.h
 * @author  Hampus Sandberg
 * @version 0.1
 * @date    2013-02-10
 * @brief   Contains various structs to handle GPIO on AVR microcontrollers
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef AVR_GPIO_h
#define AVR_GPIO_h

/* Includes ------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/**
 * @brief  GPIO structure with port, pin number, pin and ddr
 */
typedef struct
{
    uint8_t GPIO_PinNumber;
    uint8_t GPIO_Port;
    uint8_t GPIO_Ddr;
    uint8_t GPIO_Pin;
} GPIO_TypeDef;

/* Defines -------------------------------------------------------------------*/
/* Function prototypes -------------------------------------------------------*/

#endif /* AVR_GPIO_h */