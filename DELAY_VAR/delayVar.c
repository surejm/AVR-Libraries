/*
 * delayVar.c
 *
 * Created: 11/10/2012 7:30:18 PM
 *  Author: Hampus
 */ 

#include <util/delay.h>

#include "delayVar.h"

void delay_ms(uint16_t count) {	
	for (uint16_t i = 0; i < count; i++)
	{
		_delay_ms(1);
	}
}

void delay_us(uint16_t count) {
	for (uint16_t i = 0; i < count; i++)
	{
		_delay_us(1);
	}
}