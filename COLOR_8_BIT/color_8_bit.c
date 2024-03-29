/*
 * color_8_bit.c
 *
 * Created: 2012-09-11 21:13:02
 *  Author: Hampus
 */

#include <avr/io.h>

#include "color_8_bit.h"


void HSBtoRGB(uint16_t hue, uint8_t sat, uint8_t bright, uint8_t *red, uint8_t *green, uint8_t *blue) {
	volatile float saturation = (float) sat / 100.0, brightness = (float) bright / 100.0, h_i, f, p, q, t, R, G, B;
	hue = hue % 360;
	h_i = hue / 60;
	f = (float)(hue) / 60.0 - h_i;
	p = brightness * (1 - saturation);
	q = brightness * (1 - saturation * f);
	t = brightness * (1 - saturation * (1 - f));
	
	if (h_i == 0) {
		R = brightness;
		G = t;
		B = p;
	} else if (h_i == 1) {
		R = q;
		G = brightness;
		B = p;
	} else if (h_i == 2) {
		R = p;
		G = brightness;
		B = t;
	} else if (h_i == 3) {
		R = p;
		G = q;
		B = brightness;
	} else if (h_i == 4) {
		R = t;
		G = p;
		B = brightness;
	} else {
		R = brightness;
		G = p;
		B = q;
	}

	*red = (uint8_t)(R * 255.0);
	*green = (uint8_t)(G * 255.0);
	*blue = (uint8_t)(B * 255.0);
}

void RGBtoHSB(uint8_t red, uint8_t green, uint8_t blue, uint16_t *theHue, uint8_t *theSat, uint8_t *theBri) {
	float r_percent = ((double)red)/255;
	float g_percent = ((double)green)/255;
	float b_percent = ((double)blue)/255;
	
	float max_color = 0;
	if ((r_percent >= g_percent) && (r_percent >= b_percent))
		max_color = r_percent;
	if ((g_percent >= r_percent) && (g_percent >= b_percent))
		max_color = g_percent;
	if ((b_percent >= r_percent) && (b_percent >= g_percent))
		max_color = b_percent;

	float min_color = 0;
	if ((r_percent <= g_percent) && (r_percent <= b_percent))
		min_color = r_percent;
	if ((g_percent <= r_percent) && (g_percent <= b_percent))
		min_color = g_percent;
	if ((b_percent <= r_percent) && (b_percent <= g_percent))
		min_color = b_percent;
	
	float hue, sat, bri;
	
	bri = max_color;				// bri
	float delta = max_color - min_color;

	if (max_color != 0) sat = delta / max_color;	// sat
	else {
		// r = g = b = 0		// sat = 0, bri is undefined
		*theHue = -1;
		*theSat = 0;
		*theBri = -1;
		return;
	}
	
	if (r_percent == max_color) hue = (g_percent - b_percent) / delta;		// between yellow & magenta
	else if (g_percent == max_color) hue = 2 + (b_percent - r_percent) / delta;	// between cyan & yellow
	else hue = 4 + (r_percent - g_percent) / delta;	                        // between magenta & cyan
	
	hue *= 60;				// degrees
	if (hue < 0) hue += 360;
	
	*theHue = (int16_t) hue;
	*theSat = (uint8_t) (sat * 100);
	*theBri = (uint8_t) (bri * 100);
}