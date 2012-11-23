/*
 * PCA9633.h
 *
 * Created: 11/23/2012 9:50:45 PM
 *  Author: Hampus
 */ 


#ifndef PCA9633_H_
#define PCA9633_H_

#include <avr/io.h>
#include <TWI/twi.h>

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

#ifndef PCA9633_ADDRESS
#define PCA9633_ADDRESS		0x60
#endif

#ifndef PCA9633_OE
#define PCA9633_OE			PORTB1
#define PCA9633_OE_DDR		DDRB
#define PCA9633_OE_PORT		PORTB
#endif

void pca9633setup();
void pca9633setOutput(const uint8_t output, const uint8_t value);
void pca9633setAllOutputs(const uint8_t value1, const uint8_t value2, const uint8_t value3, const uint8_t value4);
void pca9633outputOff();
void pca9633outputOn();

void pca9633test();

#endif /* PCA9633_H_ */