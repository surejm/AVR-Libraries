/*
 * PCA9633.h
 *
 * Created: 11/23/2012 9:50:45 PM
 *  Author: Hampus
 */ 

#ifndef PCA9633_H_
#define PCA9633_H_

// Why do I have to include these here?
#include <avr/io.h>
#include <COLOR_8_BIT/color_8_bit.h>

#define PCA9633_AUTO_INC_NO				0x00
#define PCA9633_AUTO_INC_ALL			0x80
#define PCA9633_AUTO_INC_PWM			0xA0
#define PCA9633_AUTO_INC_GLOBAL			0xC0
#define PCA9633_AUTO_INC_GLOBAL_AND_PWM	0xE0

#define PCA9633_MODE1		0x00
#define PCA9633_MODE2		0x01
#define PCA9633_PWM0		0x02
#define PCA9633_PWM1		0x03
#define PCA9633_PWM2		0x04
#define PCA9633_PWM3		0x05
#define PCA9633_GRPPWM		0x06
#define PCA9633_GRPFREQ		0x07
#define PCA9633_LEDOUT		0x08
#define PCA9633_SUBADR1		0x09
#define PCA9633_SUBADR2		0x0A
#define PCA9633_SUBADR3		0x0B
#define PCA9633_ALLCALLADR	0x0C

#define PCA9633_REGISTER_COUNT	0x0D

#define PCA9633_OE_MODE_LOW		0x00
#define PCA9633_OE_MODE_HIGH	0x01
#define PCA9633_OE_MODE_HIGH_Z	0x02

#define PCA9633_LEDOUT_OFF				0x00
#define PCA9633_LEDOUT_ON				0x01
#define PCA9633_LEDOUT_PWM				0x02
#define PCA9633_LEDOUT_PWM_AND_GROUP	0x03

#ifndef PCA9633_ADDRESS
#define PCA9633_ADDRESS		0x60
#endif

#ifndef PCA9633_OE
#define PCA9633_OE			PORTB1
#define PCA9633_OE_DDR		DDRB
#define PCA9633_OE_PORT		PORTB
#define PCA9633_OE_PINX		PINB
#endif

void pca9633setup();
void pca9633setOutput(const uint8_t output, const uint8_t value);
void pca9633setAllOutputs(const uint8_t value0, const uint8_t value1, const uint8_t value2, const uint8_t value3);
void pca9633outputOff();
void pca9633outputOn();
uint8_t pca9633outputIsOn();
rgba8 pca9633getRgba();

// Sleep
void pca9633goToSleep();
void pca9633wakeUp();
uint8_t pca9633isSleeping();

void pca9633setLedoutRegister(const uint8_t value0, const uint8_t value1, const uint8_t value2, const uint8_t value3);
uint8_t pca9633outputIsInverted();
void pca9633invertOutputs();
void pca9633nonInvertOutputs();
void pca9633setOeMode(uint8_t mode);

void pca9633test();

#ifdef DEBUG_PCA9633
void pca9633readAllRegisters();
#endif

#endif /* PCA9633_H_ */