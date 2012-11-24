/*
 * twi.h
 *
 * Created: 2012-09-22 12:41:29
 *  Author: Hampus
 */ 


#ifndef TWI_H_
#define TWI_H_

#include <avr/io.h>
#include <util/twi.h>

#define TWI_PRESCALER_HEX 0x00    // 0x00 (1) - 0x01 (4) - 0x02 (16) - 0x03 (64)
#define TWI_PRESCALER_DEC 1
#define TWI_FREQUENCY 400000
#define TWI_BIT_RATE_REGISTER (F_CPU/(2*TWI_FREQUENCY)-8)/TWI_PRESCALER_DEC

#ifndef ERROR
#define ERROR 1
#endif
#ifndef SUCCESS
#define SUCCESS 0
#endif

void twiSetup();
void twiStart();
void twiStop();
uint8_t twiReadAck();
uint8_t twiReadNack();
uint8_t twiGetStatus();
void twiWriteRaw(const uint8_t data);

uint8_t twiBeginTransmission(const uint8_t address);
void twiEndTransmission();
uint8_t twiWrite(const uint8_t data);
uint8_t twiRequestFrom(const uint8_t address, uint8_t* storage, const uint8_t byteCount);

#endif /* TWI_H_ */