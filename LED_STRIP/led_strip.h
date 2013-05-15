/*
 * led_strip.h
 *
 * Created: 2012-09-02 17:54:24
 *  Author: Hampus
 */ 


#ifndef LED_STRIP_H_
#define LED_STRIP_H_

#include <COLOR/color.h>

#ifndef HELP_DEFINITIONS
#define HELP_DEFINITIONS
#define TRUE	1
#define FALSE	0
#define HIGH	1
#define LOW		0
#endif

/*
	Red (PB1):		OC1A
	Green (PD6):	OC0A
	Blue (PD5):		OC0B
*/

// Led Strip Defines
#define LED_STRIP_RED_DDR		DDRB
#define LED_STRIP_RED_PORT		PORTB
#define LED_STRIP_RED_PIN		PORTB1
#define LED_STRIP_GREEN_DDR		DDRD
#define LED_STRIP_GREEN_PORT	PORTD
#define LED_STRIP_GREEN_PIN		PORTD6
#define LED_STRIP_BLUE_DDR		DDRD
#define LED_STRIP_BLUE_PORT		PORTD
#define LED_STRIP_BLUE_PIN		PORTD5



void initLedStrip();
uint8_t setAndShowLedStripRGB(const uint8_t red, const uint8_t green, const uint8_t blue);
void setAndShowLedStripHSB(const uint16_t hue, const uint8_t saturation, const uint8_t brightness);
void turnOffLedStrip();
void turnOnLedStrip();

void disableLedStripPwm();
void enableLedStripPwm();

uint8_t ledStripIsOn();
void ledStripChangeBrightness(const int8_t theChange);

void testLedStrip();

rgb8 _ledStripRgb8;


#endif /* LED_STRIP_H_ */