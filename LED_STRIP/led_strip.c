/*
 * led_strip.c
 *
 * Created: 2012-09-02 17:54:42
 *  Author: Hampus
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "led_strip.h"

#define _ledStripRedColor		OCR1A
#define _ledStripGreenColor		OCR0A
#define _ledStripBlueColor		OCR0B

#define TIMER0_PRESCALING	_BV(CS00)
#define TIMER1_PRESCALING	_BV(CS10)

/************************************************************************
	Initialize the LED-strip
************************************************************************/
void initLedStrip()
{
	LED_STRIP_RED_DDR |= _BV(LED_STRIP_RED_PIN);
	LED_STRIP_GREEN_DDR |= _BV(LED_STRIP_GREEN_PIN);
	LED_STRIP_BLUE_DDR |= _BV(LED_STRIP_BLUE_PIN);
	
	LED_STRIP_RED_PORT &= ~_BV(LED_STRIP_RED_PIN);
	LED_STRIP_GREEN_PORT &= ~_BV(LED_STRIP_GREEN_PIN);
	LED_STRIP_BLUE_PORT &= ~_BV(LED_STRIP_BLUE_PIN);
	
	/*
		Note:
		Fast PWM does not turn of the output when OCRx is 0.
		Phase Correct does.
	*/
	
	// Timer 1: Red(A) - PWM, Phase Correct, 8-bit
	// No prescaling => Frequency = 8 MHz / 1 / 255 / 2 = 15.686 Hz
	TCCR1A = _BV(COM1A1) | _BV(WGM10);
	TCCR1B = TIMER1_PRESCALING;
	_ledStripRedColor = 0;	
	
	// Timer 0: Green(A), Blue(B) - PWM, Phase Correct
	// No prescaling => Frequency = 8 MHz / 1 / 255 / 2 = 15.686 Hz
	TCCR0A = _BV(COM0A1) | _BV(COM0B1) | _BV(WGM00);
	TCCR0B = TIMER0_PRESCALING;
	_ledStripGreenColor = 0;
	_ledStripBlueColor = 0;
}

/************************************************************************
	Set the data for the different colors and show it
************************************************************************/
uint8_t setAndShowLedStripRGB(const uint8_t red, const uint8_t green, const uint8_t blue)
{
	if (ledStripIsOn() && red < 256 && green < 256 && blue < 256)
	{
		_ledStripRedColor = red;
		_ledStripGreenColor = green;
		_ledStripBlueColor = blue;
		return 1;
	}
	else
		return 0;
}

/************************************************************************
	Set the data from HSB by converting it to RGB
************************************************************************/
void setAndShowLedStripHSB(const uint16_t hue, const uint8_t saturation, const uint8_t brightness)
{
	uint8_t red, green, blue;
	HSBtoRGB(hue, saturation, brightness, &red, &green, &blue, 8);
	setAndShowLedStripRGB(red, green, blue);
}

/************************************************************************
	Turn off the LED-strip
************************************************************************/
void turnOffLedStrip()
{
	if (ledStripIsOn()) disableLedStripPwm();
}

/************************************************************************
	Turn on the LED-strip
************************************************************************/
void turnOnLedStrip()
{
	if (!ledStripIsOn()) enableLedStripPwm();
}

/************************************************************************
	Disable the timers to turn off PWM
************************************************************************/
void disableLedStripPwm()
{
	// Disconnect outputs
	TCCR0A &= ~(_BV(COM0A1) | _BV(COM0B1));
	TCCR1A &= ~_BV(COM1A1);
	
	// Stop timers
	TCCR0B &= ~(_BV(CS02) | _BV(CS01) | _BV(CS00));
	TCCR1B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10));
}

/************************************************************************
	Enable the timers to turn on PWM
************************************************************************/
void enableLedStripPwm()
{
	// Connect outputs
	TCCR0A |= _BV(COM0A1) | _BV(COM0B1);
	TCCR1A |= _BV(COM1A1);
	
	// Start timers
	TCCR0B |= TIMER0_PRESCALING;
	TCCR1B |= TIMER1_PRESCALING;
}


/************************************************************************
	Return true if the LED-strip is on
************************************************************************/
uint8_t ledStripIsOn()
{
	return (TCCR0A & (_BV(COM0A1) | _BV(COM0B1)) &&
			TCCR1A & _BV(COM1A1));
}


/************************************************************************
	Change the brightness
************************************************************************/
void ledStripChangeBrightness(const int8_t theChange)
{
	uint16_t hue;
	uint8_t sat, bright;
	
	RGBtoHSB(_ledStripRedColor, _ledStripGreenColor, _ledStripBlueColor, &hue, &sat, &bright);
	
	if (bright + theChange <= 100 && bright + theChange > 0)
	{
		bright += theChange + 1;
		if (bright > 100) bright = 100;
		setAndShowLedStripHSB(hue, sat, bright);
	}
}

/************************************************************************
	Do some test patterns
************************************************************************/
void testLedStrip()
{
	for (int i = 0; i < 256; i++)
	{
		setAndShowLedStripRGB(i, 0, 0);
		_delay_ms(25);
	}
	_delay_ms(100);
	
	for (int i = 0; i < 256; i++)
	{
		setAndShowLedStripRGB(0, i, 0);
		_delay_ms(25);
	}
	_delay_ms(100);
	
	for (int i = 0; i < 256; i++)
	{
		setAndShowLedStripRGB(0, 0, i);
		_delay_ms(25);
	}
	_delay_ms(100);
	
	for (int i = 0; i < 256; i++)
	{
		setAndShowLedStripRGB(i, i, i);
		_delay_ms(25);
	}
	_delay_ms(100);
	
	turnOffLedStrip();	
}