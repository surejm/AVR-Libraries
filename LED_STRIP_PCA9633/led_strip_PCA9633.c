/*
 * led_strip_PCA9633.c
 *
 * Created: 11/27/2012 10:26:21 PM
 *  Author: Hampus
 */

#include <avr/io.h>
#include <util/delay.h>
#include <PCA9633/PCA9633.h>
#include <COLOR_8_BIT/color_8_bit.h>
#include <DELAY_VAR/delayVar.h>

#include "led_strip_PCA9633.h"

rgb8 ledStripPca9633rgb8;
hsb _currentHSB;

/************************************************************************
	Setup the LED-strip with PCA9633
************************************************************************/
void LED_STRIP_PCA9633_Init()
{
	pca9633setup();
	pca9633invertOutputs();
	pca9633setOeMode(PCA9633_OE_MODE_HIGH_Z);
	pca9633setLedoutRegister(PCA9633_LEDOUT_PWM, PCA9633_LEDOUT_PWM, PCA9633_LEDOUT_PWM, PCA9633_LEDOUT_PWM);
	
	ledStripPca9633clear();
	
	const uint16_t delayTime = 500;
	
	ledStripPca9633switchOn();
	ledStripPca9633setHsb(0, 100, 0);
	ledStripPca9633fadeToBrightness(LED_STRIP_MAX_BRIGHT/4, delayTime);
	ledStripPca9633fadeToBrightness(LED_STRIP_NO_BRIGHT, delayTime);
	ledStripPca9633setHsb(120, 100, 0);
	ledStripPca9633fadeToBrightness(LED_STRIP_MAX_BRIGHT/4, delayTime);
	ledStripPca9633fadeToBrightness(LED_STRIP_NO_BRIGHT, delayTime);
	ledStripPca9633setHsb(240, 100, 0);
	ledStripPca9633fadeToBrightness(LED_STRIP_MAX_BRIGHT/4, delayTime);
	ledStripPca9633fadeToBrightness(LED_STRIP_NO_BRIGHT, delayTime);

	ledStripPca9633setHsb(LED_STRIP_START_HUE, 100, 0);
	ledStripPca9633fadeToBrightness(LED_STRIP_MAX_BRIGHT, 2000);
}

void ledStripPca9633setRgbStruct(const rgb8 rgbColor)
{
	if (ledStripPca9633isOn()) 
	{
		pca9633setAllOutputs(rgbColor.red, rgbColor.green, rgbColor.blue, 255);
		RGBtoHSB(rgbColor.red, rgbColor.green, rgbColor.blue, 
				&_currentHSB.hue, &_currentHSB.saturation, &_currentHSB.brightness);
	}
}

void ledStripPca9633setRgb(const uint8_t red, const uint8_t green, const uint8_t blue)
{
	if (ledStripPca9633isOn()) 
	{
		pca9633setAllOutputs(red, green, blue, 255);
		RGBtoHSB(red, green, blue,
				&_currentHSB.hue, &_currentHSB.saturation, &_currentHSB.brightness);
	}		
}

void ledStripPca9633setHsbStruct(const hsb hsbColor)
{
	if (ledStripPca9633isOn())
	{
		rgb8 rgbColor;
		HSBtoRGB(hsbColor.hue, hsbColor.saturation, hsbColor.brightness, &rgbColor.red, &rgbColor.green, &rgbColor.blue);
		_currentHSB = hsbColor;
		pca9633setAllOutputs(rgbColor.red, rgbColor.green, rgbColor.blue, 255);
	}	
}

void ledStripPca9633setHsb(const uint16_t hue, const uint8_t saturation, const uint8_t brightness)
{
	if (ledStripPca9633isOn())
	{
		uint8_t red, green, blue;
		HSBtoRGB(hue, saturation, brightness, &red, &green, &blue);
		_currentHSB.hue = hue;
		_currentHSB.saturation = saturation;
		_currentHSB.brightness = brightness;
		pca9633setAllOutputs(red, green, blue, 255);
	}
}

void ledStripPca9633clear() { pca9633setAllOutputs(0, 0, 0, 255); }
	
void ledStripPca9633blinkRgb(const uint8_t red, const uint8_t green, const uint8_t blue, 
							const uint8_t count, const uint16_t delayTime)
{
	for (uint8_t i = 0; i < count; i++)
	{
		ledStripPca9633switchOff();
		delay_ms(delayTime);
		ledStripPca9633switchOn();
		ledStripPca9633setRgb(red, green, blue);
		delay_ms(delayTime);
	}
}

void ledStripPca9633switchOn() { pca9633outputOn(); }
void ledStripPca9633switchOff() { pca9633outputOff(); }
uint8_t ledStripPca9633isOn() { return pca9633outputIsOn(); }
uint8_t ledStripPca9633isOff() { return !pca9633outputIsOn(); }

void ledStripPca9633fadeToBrightness(uint8_t newBrightness, uint16_t fadeTime)
{	
	if (fadeTime)
	{
		int8_t brightChange = newBrightness - _currentHSB.brightness;
		int16_t iterations;
		if (brightChange < 0) iterations = -brightChange;
		else iterations = brightChange;
		
		uint16_t delayTime = fadeTime / iterations;
		hsb newHsb;
		newHsb.hue = _currentHSB.hue;
		newHsb.saturation = _currentHSB.saturation;
		newHsb.brightness = _currentHSB.brightness;
		
		for (uint8_t i = 0; i < iterations; i++)
		{
			newHsb.brightness += brightChange/iterations;
			ledStripPca9633setHsbStruct(newHsb);
			delay_ms(delayTime);
		}
	}
	
	ledStripPca9633setHsb(_currentHSB.hue, _currentHSB.saturation, newBrightness);
}

void ledStripPca9633fadeToHue(uint16_t newHue, uint16_t fadeTime)
{
	int16_t hueChange = newHue - _currentHSB.hue;
	if (fadeTime && hueChange < LED_STRIP_HUE_FADE_MAX && hueChange > -LED_STRIP_HUE_FADE_MAX)
	{
		int16_t iterations;
		if (hueChange < 0) iterations = -hueChange;
		else iterations = hueChange;
		
		uint16_t delayTime = fadeTime / iterations;
		hsb newHsb;
		newHsb.hue = _currentHSB.hue;
		newHsb.saturation = _currentHSB.saturation;
		newHsb.brightness = _currentHSB.brightness;
		
		for (uint16_t i = 0; i < iterations; i++)
		{
			newHsb.hue += hueChange/iterations;
			ledStripPca9633setHsbStruct(newHsb);
			delay_ms(delayTime);
		}
	}
	
	ledStripPca9633setHsb(newHue, _currentHSB.saturation, _currentHSB.brightness);
}

void ledStripPca9633fadeOn(uint16_t fadeTime)
{
	ledStripPca9633switchOn();
	uint8_t brightness = _currentHSB.brightness;
	ledStripPca9633setHsb(_currentHSB.hue, _currentHSB.saturation, LED_STRIP_NO_BRIGHT);
	ledStripPca9633fadeToBrightness(brightness, fadeTime);
}

void ledStripPca9633fadeOff(uint16_t fadeTime)
{
	uint8_t brightness = _currentHSB.brightness;
	ledStripPca9633fadeToBrightness(LED_STRIP_NO_BRIGHT, fadeTime);
	ledStripPca9633setHsb(_currentHSB.hue, _currentHSB.saturation, brightness);
	ledStripPca9633switchOff();
}

void ledStripPca9633changeBrightness(int8_t change)
{
	uint8_t newBrightness = _currentHSB.brightness;
	if (change + _currentHSB.brightness <= LED_STRIP_MIN_BRIGHT)
		newBrightness = LED_STRIP_MIN_BRIGHT;
	else if (change + _currentHSB.brightness > LED_STRIP_MAX_BRIGHT)
		newBrightness = LED_STRIP_MAX_BRIGHT;
	else
		newBrightness += change;
	
	ledStripPca9633fadeToBrightness(newBrightness, 75);
}

rgb8 ledStripPca9633getRgb()
{
	rgba8 rgbaValues = pca9633getRgba();
	rgb8 rgbValues = {rgbaValues.red, rgbaValues.green, rgbaValues.blue};
	return rgbValues;
}

hsb ledStripPca9633getHsb()
{
	return _currentHSB;
}