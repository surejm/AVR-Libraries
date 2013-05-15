/**
 ******************************************************************************
 * @file	conversion.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-02-23
 * @brief	Contains function prototypes for various kinds of data conversions
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef CONVERSION_H_
#define CONVERSION_H_

/* Includes ------------------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
#define stringToUint32(str, end, base) (strtoul(str, end, base))	// str has to be 16 characters => 4 byte
#define IS_VALID_HEX(hex) (((hex) >= '0' && (hex) <= '9') || \
							((hex) >= 'A' && (hex) <= 'F') || \
							((hex) >= 'a' && (hex) <= 'f'))
/* Typedefs ------------------------------------------------------------------*/
/**
 * @brief  template
 */

/* Function prototypes -------------------------------------------------------*/

#endif /* CONVERSION_H_ */