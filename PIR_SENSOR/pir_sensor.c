/*
 * pir_sensor.c
 *
 * Created: 2012-09-24 19:29:38
 *  Author: Hampus
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <MILLIS_COUNT/millis_count.h>

#include "pir_sensor.h"

void PIR_SENSOR_Init(void(*thePirSensorManageDataFunc)(uint8_t))
{
	_pirSensorManageDataFunc = thePirSensorManageDataFunc;
	MILLIS_COUNT_Init();
	_pirMillisLastMotion = millis();
	
	cli();
	PIR_DDR &= ~_BV(PIR_PIN);
	// Rising edge trigger
	EICRA |= _BV(ISC01) | _BV(ISC00);
	EIMSK |= _BV(INT0);
	sei();
}

uint32_t pirSensorGetMillisLastMotion()
{
	return _pirMillisLastMotion;
}

void pirSensorActivate()
{
	_pirMillisLastMotion = millis();
	EIMSK |= _BV(INT0);
}

void pirSensorDeactivate()
{
	EIMSK &= ~_BV(INT0);
}

ISR(INT0_vect)
{
	if (_pirSensorManageDataFunc) _pirSensorManageDataFunc(PIR_MOTION_DETECTED);
	_pirMillisLastMotion = millis();
}