/*
 * tlc5947.h
 *
 * Created: 11/10/2012 7:10:32 PM
 *  Author: Hampus
 */ 


#ifndef TLC5947_H_
#define TLC5947_H_

#include <avr/io.h>
#include <util/delay.h>
#include <DELAY_VAR/delayVar.h>
#include <SPI/spi.h>
#include <COLOR/color.h>


#define BLANK_PIN	PORTD7
#define OUTPUT_OFF	PORTD |= _BV(BLANK_PIN)
#define OUTPUT_ON	PORTD &= ~_BV(BLANK_PIN)

#define LATCH_PIN	PORTB0
#define LATCH_HIGH	PORTB |= _BV(LATCH_PIN)
#define LATCH_LOW	PORTB &= ~_BV(LATCH_PIN)
#define LATCH_DATA	LATCH_HIGH; LATCH_LOW;

#ifndef NUM_OF_MODULES
#define NUM_OF_MODULES	1
#endif
#define NUM_OF_PIXELS	16
#define TOTAL_OF_PIXELS	(NUM_OF_MODULES*NUM_OF_PIXELS)

uint8_t _tlc5947Data[NUM_OF_MODULES][72];

void tlc5947Setup();

void testPatternAllWhite();
void testPatternHUEAll(const uint16_t delayTime);
void testPatternHUESeperate(uint16_t delayTime);
void testPatternHUESeperateMoving(uint16_t delayTime);
void testPatternRGBSeperate(uint16_t delayTime);
void testPatternRGBAll(uint16_t delayTime);
void testPatternFadeRGBSeparate(uint16_t delayTime);
void testPatternRGBWlines();

void updateOutputs();
void clearAll();
void setPixelRGB(const uint8_t pixel, const uint8_t module, 
					const uint16_t red, const uint16_t green, const uint16_t blue);
void setPixelRGBxy(const uint8_t x, const uint8_t y, const uint8_t module, 
					const uint16_t red, const uint16_t green, const uint16_t blue);
void setAllRGB(const uint16_t red, const uint16_t green, const uint16_t blue);
void setOutputValue(uint8_t output, const uint8_t module, uint16_t value);


#endif /* TLC5947_H_ */