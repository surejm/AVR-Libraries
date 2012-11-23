/*
 * tlc5947.c
 *
 * Created: 11/10/2012 7:10:42 PM
 *  Author: Hampus
 */ 

#include "tlc5947.h"

void tlc5947Setup()
{
	DDRB |= _BV(LATCH_PIN);
	DDRD |= _BV(BLANK_PIN);
	OUTPUT_OFF;
 	spiSetup(SPI_CLOCK_DIV2);
 	LATCH_DATA;
	_delay_ms(1000);
	clearAll();
}

/*=========================================================================================================================*/

void testPatternAllWhite()
{
	setAllRGB(0xFFF, 0xFFF, 0xFFF);
	updateOutputs();
}

void testPatternHUEAll(const uint16_t delayTime)
{
	uint16_t hue, red, green, blue;
	for (hue = 0; hue < 360; hue++) 
	{
		HSBtoRGB(hue, 100, 100, &red, &green, &blue, 12);
		setAllRGB(red, green, blue);
		updateOutputs();
		delay_ms(delayTime);
	}
}

void testPatternHUESeperate(uint16_t delayTime) {
	uint16_t hue, red, green, blue;
	uint8_t module, pixel;
	
	for (module = 0; module < NUM_OF_MODULES; module++) {
		for (pixel = 0; pixel < NUM_OF_PIXELS; pixel++) {
			hue = 360/TOTAL_OF_PIXELS * (module*NUM_OF_PIXELS + pixel);
			HSBtoRGB(hue, 100, 100, &red, &green, &blue, 12);
			setPixelRGB(pixel, module, red, green, blue);
			
			updateOutputs();
			delay_ms(delayTime);
		}
	}
	delay_ms(2000);
	clearAll();
}

void testPatternHUESeperateMoving(uint16_t delayTime) {
	uint16_t hue, red, green, blue;
	uint8_t module, pixel, offset;
	int8_t offsetPixelIndex;
	
	for (offset = 0; offset < TOTAL_OF_PIXELS; offset++) {
		for (module = 0; module < NUM_OF_MODULES; module++) {
			for (pixel = 0; pixel < NUM_OF_PIXELS; pixel++) {
				hue = 360/TOTAL_OF_PIXELS * (module*NUM_OF_PIXELS + pixel);
				HSBtoRGB(hue, 100, 100, &red, &green, &blue, 12);
				
				offsetPixelIndex = pixel - offset;
				if (offsetPixelIndex < 0) offsetPixelIndex += NUM_OF_PIXELS;
				setPixelRGB(offsetPixelIndex, module, red, green, blue);
			}
		}
		updateOutputs();
		delay_ms(delayTime);
	}
}


void testPatternRGBSeperate(uint16_t delayTime) {
	static uint8_t pixel = 0;
	static uint8_t module = 0;
	
	pixel++;
	if (pixel == NUM_OF_PIXELS) 
	{
		pixel = 0;
		module++;
		if (module == NUM_OF_MODULES) module = 0;
	}		
	
	// Go through red, green, blue for every pixel
	setPixelRGB(pixel, module, 0xFFF, 0x000, 0x000);
	updateOutputs();
	delay_ms(delayTime);
	setPixelRGB(pixel, module, 0x000, 0xFFF, 0x000);
	updateOutputs();
	delay_ms(delayTime);
	setPixelRGB(pixel, module, 0x000, 0x000, 0xFFF);
	updateOutputs();
	delay_ms(delayTime);
	setPixelRGB(pixel, module, 0x000, 0x000, 0x000);
	updateOutputs();
}

void testPatternRGBAll(uint16_t delayTime) {
	// Go through red, green, blue
	setAllRGB(0xFFF, 0x000, 0x000);
	updateOutputs();
	delay_ms(delayTime);
	setAllRGB(0x000, 0xFFF, 0x000);
	updateOutputs();
	delay_ms(delayTime);
	setAllRGB(0x000, 0x000, 0xFFF);
	updateOutputs();
	delay_ms(delayTime);
	setAllRGB(0x000, 0x000, 0x000);
	updateOutputs();
}

void testPatternFadeRGBSeparate(uint16_t delayTime) {
	for (int output = 0; output < 48; output++) {
		for (int color = 0; color < 0xFFF; color += 10) {
			setOutputValue(output, 0, color);
			updateOutputs();
			delay_ms(delayTime);
		}
		setOutputValue(output, 0, 0x000);
		updateOutputs();
	}
}

void testPatternRGBWlines()
{
	for (int y = 0; y < 4; y++)
	{
		for (uint8_t module = 0; module < NUM_OF_MODULES; module++)
			setPixelRGBxy(0, y, module, 0xFFF, 0, 0);
	}
	for (int y = 0; y < 4; y++)
	{
		for (uint8_t module = 0; module < NUM_OF_MODULES; module++)
			setPixelRGBxy(1, y, module, 0, 0xFFF, 0);
	}
	for (int y = 0; y < 4; y++)
	{
		for (uint8_t module = 0; module < NUM_OF_MODULES; module++)
			setPixelRGBxy(2, y, module, 0, 0, 0xFFF);
	}
	for (int y = 0; y < 4; y++)
	{
		for (uint8_t module = 0; module < NUM_OF_MODULES; module++)
			setPixelRGBxy(3, y, module, 0xFFF, 0xFFF, 0xFFF);
	}
	updateOutputs();
}

/*-------------------------------------------------------------------------------------------------------------------------
  Sets the RGB values when the TLC5947 is used with RBG-LEDs.
  The channels now consist of 3 outputs. Example: channel 1: output 3,4,5
  -------------------------------------------------------------------------------------------------------------------------*/
void updateOutputs() {
	uint8_t byte, module;
	for (module = 0; module < NUM_OF_MODULES; module++) {
		for (byte = 0; byte < 72; byte++) {    
			spiWrite(_tlc5947Data[module][byte]);
		}
	}
	OUTPUT_OFF;
	LATCH_DATA;
	OUTPUT_ON;
}

/*-------------------------------------------------------------------------------------------------------------------------
  Clears all the outputs back to 0x000
  -------------------------------------------------------------------------------------------------------------------------*/
void clearAll() {
  setAllRGB(0x000, 0x000, 0x000);
  updateOutputs();
}
/*=========================================================================================================================*/


/*-------------------------------------------------------------------------------------------------------------------------
  Sets the RGB values when the TLC5947 is used with RBG-LEDs.
  The channels now consist of 3 outputs. Example: channel 1: output 3,4,5
  -------------------------------------------------------------------------------------------------------------------------*/
void setPixelRGB(const uint8_t pixel, const uint8_t module, const uint16_t red, const uint16_t green, const uint16_t blue)
{
  uint8_t baseOutput = pixel * 3;
  
  #ifdef TLC5947_BRG_ORDER
	  setOutputValue(baseOutput, module, blue);
	  setOutputValue(baseOutput + 1, module, red);
	  setOutputValue(baseOutput + 2, module, green);
  #elif defined TLC5947_GRB_ORDER
 	  setOutputValue(baseOutput, module, green);
 	  setOutputValue(baseOutput + 1, module, red);
 	  setOutputValue(baseOutput + 2, module, blue);
  #else
	  setOutputValue(baseOutput, module, red);
	  setOutputValue(baseOutput + 1, module, green);
	  setOutputValue(baseOutput + 2, module, blue);
  #endif
}

/*-------------------------------------------------------------------------------------------------------------------------
  -------------------------------------------------------------------------------------------------------------------------*/
void setPixelRGBxy(const uint8_t x, const uint8_t y, const uint8_t module, 
					const uint16_t red, const uint16_t green, const uint16_t blue)
{
	uint8_t pixelTransform[4][4] = { {0, 1, 2, 3}, {7, 6, 5, 4}, {8, 9, 10, 11}, {15, 14, 13, 12} };
	setPixelRGB(pixelTransform[x][y], module, red, green, blue);
}



/*=========================================================================================================================*/

/*-------------------------------------------------------------------------------------------------------------------------
  Sets the RGB values of all pixels when the TLC5947 is used with RBG-LEDs.
  -------------------------------------------------------------------------------------------------------------------------*/
void setAllRGB(const uint16_t red, const uint16_t green, const uint16_t blue) {
  uint8_t pixel, module;
  
  for (module = 0; module < NUM_OF_MODULES; module++) {
    for (pixel = 0; pixel < NUM_OF_PIXELS ; pixel++) {
      setPixelRGB(pixel, module, red, green, blue);
    }
  }
}

/*=========================================================================================================================*/

/*-------------------------------------------------------------------------------------------------------------------------
  Sets the correct value in the array based on which output should be changed.
  Outputs are 12bit values but the array only holds 8bit values so we need to move stuff around a bit
  -------------------------------------------------------------------------------------------------------------------------*/
void setOutputValue(uint8_t output, const uint8_t module, uint16_t value) {
  // Inverse the output
  output = 47 - output;
  
  /* #1 Example: output = 3: 3*12=36, 36/8=4.5=4 -> Byte no 4 is the start
        Example: output = 2: 2*12=24, 24/8=3 -> Byte no 3 is the start
  */
    uint8_t positionInArray = output*12/8;
  
  /* If the output is uneven the 4 MSB-bits starts on the current byte */
  if (output % 2) {    
    // #2 Put the 8 LSB-bits in the whole next position in the array
    _tlc5947Data[module][positionInArray+1] = value & 0xFF;
    // #3 Shift forward to the 4 MSB-bits
    value = value >> 8;
    /* #4 Put the 4 MSB-bits in half the calculated position in the array
          They must be in the 4 LSB-bits in this byte so no shift required
          The oldData must be combined with the new, so extract the old and then combine
    */
    uint8_t oldData =  _tlc5947Data[module][positionInArray];
    _tlc5947Data[module][positionInArray] = (oldData & 0xF0) | (value & 0xF);
  }
  /* If the output is even the 4 LSB-bits starts on the next byte */
  else {    
    /* #2 Put the 4 LSB-bits in half the next position in the array.
          They must be in the 4 MSB-bits in this byte so bitshift left 4 steps
    */
    uint8_t oldData =  _tlc5947Data[module][positionInArray+1];
    _tlc5947Data[module][positionInArray+1] = ((value & 0xF) << 4) | (oldData & 0xF);
    // #3 Shift forward to the 8 MSB-bits
    value = value >> 4;
    // #4 Put the 8 MSB-bits in the whole calculated position in the array
    _tlc5947Data[module][positionInArray] = value & 0xFF;
  }
}