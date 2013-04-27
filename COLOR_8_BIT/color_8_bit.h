/*
 * color_8_bit.h
 *
 * Created: 2012-09-11 21:12:52
 *  Author: Hampus
 */ 

#ifndef COLOR_8_BIT_H_
#define COLOR_8_BIT_H_

typedef struct  
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} rgb8;

typedef struct
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
} rgba8;

typedef struct  
{
	uint16_t hue;
	uint8_t saturation;
	uint8_t brightness;
} hsb;

void HSBtoRGB(uint16_t hue, uint8_t sat, uint8_t bright, uint8_t *red, uint8_t *green, uint8_t *blue);
void RGBtoHSB(uint8_t red, uint8_t green, uint8_t blue, uint16_t *hue, uint8_t *sat, uint8_t *bright);


#endif /* COLOR_8_BIT_H_ */