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
#include <atmega328x/spi.h>
#include <COLOR _16_BIT/color_16_bit.h>

#if !defined(BLANK_PIN) || !defined(BLANK_PORT) || !defined(BLANK_DDR)
#error "Please define BLANK_PIN, BLANK_PORT and BLANK_DDR"
#endif
#define OUTPUT_OFF	BLANK_PORT |= _BV(BLANK_PIN)
#define OUTPUT_ON	BLANK_PORT &= ~_BV(BLANK_PIN)

#if !defined(LATCH_PIN) || !defined(LATCH_PORT) || !defined(LATCH_DDR)
#error "Please define LATCH_PIN, LATCH_PORT and LATCH_DDR"
#endif
#define LATCH_HIGH	LATCH_PORT |= _BV(LATCH_PIN)
#define LATCH_LOW	LATCH_PORT &= ~_BV(LATCH_PIN)
#define LATCH_DATA	LATCH_HIGH; LATCH_LOW;

#ifndef NUM_OF_MODULES
#define NUM_OF_MODULES	1
#endif
#define NUM_OF_PIXELS	16
#define TOTAL_OF_PIXELS	(NUM_OF_MODULES*NUM_OF_PIXELS)

uint8_t _tlc5947Data[NUM_OF_MODULES][72];

void tlc5947Setup();

void tlc5947clearAll();
void tlc5947setPixelRGB(const uint8_t pixel, const uint8_t module, 
						const uint16_t red, const uint16_t green, const uint16_t blue);
void tlc5947setPixelRGBxy(const uint8_t x, const uint8_t y, const uint8_t module, 
						const uint16_t red, const uint16_t green, const uint16_t blue);
void tlc5947setAllRGB(const uint16_t red, const uint16_t green, const uint16_t blue);

void tlc5947testAllWhite();
void tlc5947testHUEAll(const uint16_t delayTime);
void tlc5947testHUESeperate(uint16_t delayTime);
void tlc5947testHUESeperateMoving(uint16_t delayTime);
void tlc5947testRGBSeperate(uint16_t delayTime);
void tlc5947testRGBAll(uint16_t delayTime);
void tlc5947testFadeRGBSeparate(uint16_t delayTime);
void tlc5947testRGBWlines();


#endif /* TLC5947_H_ */