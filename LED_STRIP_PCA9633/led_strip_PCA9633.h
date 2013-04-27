/*
 * led_strip_PCA9633.h
 *
 * Created: 11/27/2012 10:26:28 PM
 *  Author: Hampus
 */ 

#ifndef LED_STRIP_PCA9633_H_
#define LED_STRIP_PCA9633_H_

// Why do I have to include these here?
#include <avr/io.h>
#include <COLOR_8_BIT/color_8_bit.h>

#ifndef HELP_DEFINITIONS
#define HELP_DEFINITIONS
#define TRUE	1
#define FALSE	0
#define HIGH	1
#define LOW		0
#endif

#define LED_STRIP_START_HUE		50
#define LED_STRIP_NO_BRIGHT		0
#define LED_STRIP_MIN_BRIGHT	7
#define LED_STRIP_MAX_BRIGHT	100

#define LED_STRIP_HUE_FADE_MAX	60


void LED_STRIP_PCA9633_Init();

void ledStripPca9633setRgbStruct(const rgb8 rgbColor);
void ledStripPca9633setRgb(const uint8_t red, const uint8_t green, const uint8_t blue);
void ledStripPca9633setHsbStruct(const hsb hsbColor);
void ledStripPca9633setHsb(const uint16_t hue, const uint8_t saturation, const uint8_t brightness);
void ledStripPca9633clear();

void ledStripPca9633blinkRgb(const uint8_t red, const uint8_t green, const uint8_t blue, 
							const uint8_t count,  const uint16_t delayTime);

void ledStripPca9633switchOn();
void ledStripPca9633switchOff();
uint8_t ledStripPca9633isOn();
uint8_t ledStripPca9633isOff();

void ledStripPca9633fadeToBrightness(uint8_t newBrightness, uint16_t fadeTime);
void ledStripPca9633fadeToHue(uint16_t newHue, uint16_t fadeTime);

void ledStripPca9633fadeOn(uint16_t fadeTime);
void ledStripPca9633fadeOff(uint16_t fadeTime);
void ledStripPca9633changeBrightness(int8_t change);

rgb8 ledStripPca9633getRgb();
hsb ledStripPca9633getHsb();

#endif /* LED_STRIP_PCA9633_H_ */