/*
 * pir_sensor.h
 *
 * Created: 2012-09-24 19:29:52
 *  Author: Hampus
 */ 

#ifndef PIR_SENSOR_H_
#define PIR_SENSOR_H_

// Uses INT0
#ifdef INT0_IN_USE
#error "PIR Sensor need INT0. Used somewhere else"
#else
#define INT0_IN_USE
#endif

#define PIR_DDR		DDRD
#define PIR_PORT	PORTD
#define PIR_PIN		PORTD2

#define PIR_MOTION_DETECTED		1

// Pointer to a function that handles when the PIR Sensor is triggered
void (*_pirSensorManageDataFunc)(uint8_t);

void PIR_SENSOR_Init(void(*thePirSensorManageDataFunc)(uint8_t));
uint32_t pirSensorGetMillisLastMotion();
void pirSensorActivate();
void pirSensorDeactivate();

/************************************************************************
	_pirMillisLastMotion variable. DON'T TOUCH ANYWHERE ELSE!
************************************************************************/
volatile uint32_t _pirMillisLastMotion;


#endif /* PIR_SENSOR_H_ */