/*
 * color.h
 *
 * Created: 2012-09-11 21:12:52
 *  Author: Hampus
 */ 


#ifndef COLOR_H_
#define COLOR_H_

typedef struct  
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} rgb8;

typedef struct
{
	uint16_t red;
	uint16_t green;
	uint16_t blue;
} rgb16;

typedef struct  
{
	uint16_t hue;
	uint8_t saturation;
	uint8_t brightness;
} hsb;

void HSBtoRGB(uint16_t hue, uint8_t sat, uint8_t bright, uint16_t *red, uint16_t *green, uint16_t *blue, uint8_t bitResolution);
void RGBtoHSB(uint8_t red, uint8_t green, uint8_t blue, uint16_t *hue, uint8_t *sat, uint8_t *bright);


#endif /* COLOR_H_ */