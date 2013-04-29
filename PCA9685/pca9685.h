/**
 ******************************************************************************
 * @file	pca9685.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-04-04
 * @brief	Contains function prototypes to manage the PCA9685 LED Driver
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PCA9685_H_
#define PCA9685_H_

/* Includes ------------------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/

/* Typedefs ------------------------------------------------------------------*/
/**
 * @brief	PCA9685 Inverted outputs
 */
typedef enum
{
	PCA9685_NotInvOutputs =	0,
	PCA9685_InvOutputs =	1
} PCA9685_InvOutputs_TypeDef;
#define IS_PCA9685_INV_OUTPUTS(INVRT)	(((INVRT) == PCA9685_NotInvOutputs) || \
										((INVRT) == PCA9685_InvOutputs))

/**
 * @brief  PCA9685 Output driver types
 */
typedef enum
{
	PCA9685_OutputDriver_OpenDrain =	0,
	PCA9685_OutputDriver_TotemPole =	1
} PCA9685_OutputDriver_TypeDef;
#define IS_PCA9685_OUTPUT_DRIVER(OUTPUT_DRIVER)	(((OUTPUT_DRIVER) == PCA9685_OutputDriver_OpenDrain) || \
												((OUTPUT_DRIVER) == PCA9685_OutputDriver_TotemPole))

/**
 * @brief	PCA9685 Not enabled LED outputs defines the behaviour of the outputs
 *			when OE is pulled low
 */
typedef enum
{
	PCA9685_OutputNotEn_0 =			0,
	PCA9685_OutputNotEn_OUTDRV =	1,
	PCA9685_OutputNotEn_High_Z1 =	2,
	PCA9685_OutputNotEn_High_Z2 =	3
} PCA9685_OutputNotEn_TypeDef;
#define IS_PCA9685_OUTPUT_NOT_EN(OUTNE)	(((OUTNE) >= PCA9685_OutputNotEn_0) && \
										((OUTNE) <= PCA9685_OutputNotEn_High_Z2))
										
/**
 * @brief	PCA9685 Frequency
 *			Set by prescale = round(25 MHz / (4096 * freq)) - 1
 */
typedef enum
{
	PCA9685_Frequency_200Hz =	30,
	PCA9685_Frequency_100Hz =	60,
	PCA9685_Frequency_60Hz =	100,
	PCA9685_Frequency_50Hz =	121
} PCA9685_Frequency;
#define IS_PCA9685_FREQUENCY(FREQ) ((FREQ) == PCA9685_Frequency_200Hz || \
									(FREQ) == PCA9685_Frequency_100Hz || \
									(FREQ) == PCA9685_Frequency_60Hz || \
									(FREQ) == PCA9685_Frequency_50Hz)

/**
 * @brief	PCA9685 Init structure definition
 */
typedef struct
{
    uint8_t Address;							/** Specifies the address for the PCA9685 */
    PCA9685_InvOutputs_TypeDef InvOutputs;		/** Specifies if the outputs should be inverted
    												This parameter can be any value of PCA9685_InvOutputs_TypeDef */
    PCA9685_OutputDriver_TypeDef OutputDriver;	/** Specifies the output driver 
    												This parameter can be any value of PCA9685_OutputDriver_TypeDef */
	PCA9685_OutputNotEn_TypeDef OutputNotEn;	/** Specifies what the outputs should be when OE=1
    												This parameter can be any value of PCA9685_OutputNotEn_TypeDef */
	PCA9685_Frequency PWMFrequency;				/** Specifies what output frequency to use
    												This parameter can be any value of PCA9685_Frequency */
} PCA9685_Init_TypeDef;


/* Function prototypes -------------------------------------------------------*/
uint8_t PCA9685_Init(PCA9685_Init_TypeDef *PCA9685_InitStruct);

void PCA9685_SetOutput(uint8_t Address, uint8_t Output, uint16_t OnValue, uint16_t OffValue);
void PCA9685_SetDutyCycleForOutput(uint8_t Address, uint8_t Output, uint8_t DutyCycle);

#endif /* PCA9685_H_ */