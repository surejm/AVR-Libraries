/**
 ******************************************************************************
 * @file	BQ32000.h
 * @author	Hampus Sandberg
 * @version	0.1
 * @date	2013-03-05
 * @brief	Contains function prototypes, constants to manage the BQ32000 RTC
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BQ32000_H_
#define BQ32000_H_

/* Includes ------------------------------------------------------------------*/
/* Defines -------------------------------------------------------------------*/
/* Typedefs ------------------------------------------------------------------*/
/**
 * @brief  Weekdays
 */
typedef enum
{
	SUNDAY = 1,
	MONDAY = 2,
	TUESDAY = 3,
	WEDNESDAY = 4,
	THURSDAY = 5,
	FRIDAY = 6,
	SATURDAY = 7
} Weekday_TypeDef;
#define IS_WEEKDAY(WEEKDAY) ((WEEKDAY) >= SUNDAY && (WEEKDAY) <= SATURDAY)

/**
 * @brief  Date and time
 */
typedef struct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	Weekday_TypeDef weekday;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	
} dateAndTime_TypeDef;
#define IS_HOUR(HOUR) ((HOUR) >= 0 && (HOUR) <= 24)
#define IS_MINUTE(MINUTE) ((MINUTE) >= 0 && (MINUTE) <= 59)
#define IS_SECOND(SECOND) ((SECOND) >= 0 && (SECOND) <= 59)
#define IS_DATE(DATE) (1)
#define IS_MONTH(MONTH) ((MONTH) >= 0 && (MONTH) <= 12)
#define IS_YEAR(YEAR) ((YEAR) >= 0)

#define IS_DATE_AND_TIME(DATE_AND_TIME) (IS_HOUR((DATE_AND_TIME).hour) && \
										 IS_MINUTE((DATE_AND_TIME).minute) && \
										 IS_SECOND((DATE_AND_TIME).second) && \
										 IS_WEEKDAY((DATE_AND_TIME).weekday) && \
										 IS_DATE((DATE_AND_TIME).date) && \
										 IS_MONTH((DATE_AND_TIME).month) && \
										 IS_YEAR((DATE_AND_TIME).year))
										 
/* Function prototypes -------------------------------------------------------*/
void BQ32000_Init();
void BQ32000_UpdateDateTime();
void BQ32000_SetDateTime(const dateAndTime_TypeDef newDateTime);
void BQ32000_PrintToUart();

uint8_t BQ32000_ItsNighttime();
uint8_t BQ32000_ItsMidday();

#endif /* BQ32000_H_ */