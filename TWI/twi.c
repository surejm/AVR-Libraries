/*
 * twi.c
 *
 * Created: 2012-09-22 12:41:40
 *  Author: Hampus
 */ 

#include "twi.h"

/************************************************************************
	Core functions
************************************************************************/
void twiSetup() {
  // Set SCL-period
  TWSR = TWI_PRESCALER_HEX;
  TWBR = TWI_BIT_RATE_REGISTER;
    
  // Enable TWI
  TWCR = _BV(TWEN);
}

// Send start signal
void twiStart() {
  /*
    The application writes the TWSTA bit to one when it desires to become a Master on the 2-wire Serial Bus.
    The TWI hardware checks if the bus is available, and generates a START condition on the bus if it is free.
    However, if the bus is not free, the TWI waits until a STOP condition is detected, and then generates a 
    new START condition to claim the bus Master status.
    TWSTA must be cleared by software when the START condition has been transmitted.
  */
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
  
  // TWINT-bit is set to one by hardware when the TWI has finished its current job and expects application software response
  while (!(TWCR & _BV(TWINT)));
}

// Send stop signal
void twiStop() {
  // Writing the TWSTO bit to one in Master mode will generate a STOP condition on the 2-wire Serial Bus
  TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

uint8_t twiReadAck() {
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
	
	while (!(TWCR & _BV(TWINT)));
	return TWDR;
}

uint8_t twiReadNack() {
	TWCR = _BV(TWINT) | _BV(TWEN);
	
	while (!(TWCR & _BV(TWINT)));
	return TWDR;
}

uint8_t twiGetStatus() {
	// Mask status
	return TWSR & 0xF8;
}

void twiWriteRaw(const uint8_t data) {
	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN);
	
	while (!(TWCR & _BV(TWINT)));
}

/************************************************************************
	User functions
************************************************************************/
uint8_t twiBeginTransmission(const uint8_t address)
{
	twiStart();
	if (twiGetStatus() != TW_START) return 0; // Check if a START condition has been transmitted
	twiWriteRaw(address << 1); // Send the address + Write bit (0)
	if (twiGetStatus() != TW_MT_SLA_ACK) return 0; // Check if SLA+W has been transmitted
	return 1;
}

void twiEndTransmission()
{
	twiStop();
}

uint8_t twiWrite(const uint8_t data)
{
	twiWriteRaw(data);
	// Check if Data byte has been transmitted
	if (twiGetStatus() != TW_MT_DATA_ACK) return 0;
	return 1;
}

uint8_t twiRequestFrom(const uint8_t address, uint8_t* storage, const uint8_t byteCount)
{
	twiStart();
	if (twiGetStatus() != TW_START) return 0; // Check if a START condition has been transmitted
	twiWriteRaw(address << 1 | 0x1); // Send the address + Read bit (1)
	if (twiGetStatus() != TW_MR_SLA_ACK) return 0; // Check if SLA+W has been transmitted
	
	for (uint8_t i = 0; i < byteCount - 1; i++)
		storage[i] = twiReadAck();
	storage[byteCount - 1] = twiReadNack();
	
	twiStop();
	return 1;
}

// TODO: twiReadACK(), twiReadNACK(), how to get received data back to main?