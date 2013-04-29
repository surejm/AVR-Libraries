/*
    Copyright (c) 2007 Stefan Engelke <mbox@stefanengelke.de>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

    $Id$
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <NRF24L01/nrf24l01_register_map.h>
#include <atmega328x/spi.h>
#include "mirf.h"

// Defines for setting the MiRF registers for transmitting or receiving mode
#define TX_POWERUP mirf_config_register(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (0<<PRIM_RX) ) )
#define RX_POWERUP mirf_config_register(CONFIG, mirf_CONFIG | ( (1<<PWR_UP) | (1<<PRIM_RX) ) )


// Flag which denotes transmitting mode
volatile uint8_t PTX;


static void spi_transfer_sync(uint8_t *dataout, uint8_t *datain, uint8_t len)
{
	for (uint8_t i = 0; i < mirf_PAYLOAD; i++)
	{
		datain[i] = SPI_WriteRead(dataout[i]);
	}
}

static void spi_transmit_sync(uint8_t * dataout, uint8_t len)
{
	for (uint8_t i = 0; i < mirf_PAYLOAD; i++)
	{
		SPI_WriteRead(dataout[i]);
	}
}

void mirf_init() 
// Initializes pins ans interrupt to communicate with the MiRF module
// Should be called in the early initializing phase at startup.
{
    // Define CSN and CE as Output and set them to default
    DDRD |= ((1 << CSN) | (1 << CE));
    mirf_CE_lo;
    mirf_CSN_hi;

#if defined(__AVR_ATmega8__)
    // Initialize external interrupt 0 (PD2)
    MCUCR = ((1<<ISC11)|(0<<ISC10)|(1<<ISC01)|(0<<ISC00)); // Set external interupt on falling edge
    GICR  = ((0<<INT1)|(1<<INT0));                         // Activate INT0
#endif // __AVR_ATmega8__

#if defined(__AVR_ATmega168__)
    // Initialize external interrupt on port PD6 (PCINT22)
    DDRB &= ~(1<<PD6);
    PCMSK2 = (1<<PCINT22);
    PCICR  = (1<<PCIE2);
#endif // __AVR_ATmega168__

#if defined(__AVR_ATmega328P__)
	PORTD &= ~(1 << PORTD2);
	EICRA &= ~((1 << ISC00) | (1 << ISC01));
	EICRA |= (ISC01); // The falling edge of INT0 generates an interrupt request.
	EIMSK |= (1 << INT0);
	sei();
#endif // __AVR_ATmega328P__

    // Initialize spi module
    SPI_InitTypeDef spiInit;
    spiInit.SPI_Clock = SPI_CLOCK_DIV2;
    SPI_Init(&spiInit);
}


void mirf_config() 
// Sets the important registers in the MiRF module and powers the module
// in receiving mode
{
    // Set RF channel
    mirf_config_register(RF_CH,mirf_CH);

    // Set length of incoming payload 
    mirf_config_register(RX_PW_P0, mirf_PAYLOAD);

    // Start receiver 
    PTX = 0;        // Start in receiving mode
    RX_POWERUP;     // Power up in receiving mode
    mirf_CE_hi;     // Listening for pakets
}

void mirf_set_RADDR(uint8_t * adr) 
// Sets the receiving address
{
    mirf_CE_lo;
    mirf_write_register(RX_ADDR_P0,adr,5);
    mirf_CE_hi;
}

void mirf_set_TADDR(uint8_t * adr)
// Sets the transmitting address
{
    mirf_write_register(TX_ADDR, adr,5);
}

#if defined(__AVR_ATmega8__)
SIGNAL(SIG_INTERRUPT0) 
#endif // __AVR_ATmega8__
#if defined(__AVR_ATmega168__)
SIGNAL(SIG_PIN_CHANGE2) 
#endif // __AVR_ATmega168__
#if defined(__AVR_ATmega328P__)
ISR (INT0_vect)
#endif // __AVR_ATmega328P__
// Interrupt handler 
{
    uint8_t status;   
    // If still in transmitting mode then finish transmission
    if (PTX) {
    
        // Read MiRF status 
        mirf_CSN_lo;                                // Pull down chip select
        status = SPI_WriteRead(NOP);               // Read status register
        mirf_CSN_hi;                                // Pull up chip select

        mirf_CE_lo;                             // Deactivate transreceiver
        RX_POWERUP;                             // Power up in receiving mode
        mirf_CE_hi;                             // Listening for pakets
        PTX = 0;                                // Set to receiving mode

        // Reset status register for further interaction
        mirf_config_register(STATUS,(1<<TX_DS)|(1<<MAX_RT)); // Reset status register
    }
}

extern uint8_t mirf_data_ready() 
// Checks if data is available for reading
{
    if (PTX) return 0;
    uint8_t status;
    // Read MiRF status 
    mirf_CSN_lo;                                // Pull down chip select
    status = SPI_WriteRead(NOP);               // Read status register
    mirf_CSN_hi;                                // Pull up chip select
    return status & (1<<RX_DR);
}

extern void mirf_get_data(uint8_t * data) 
// Reads mirf_PAYLOAD bytes into data array
{
    mirf_CSN_lo;                               // Pull down chip select
    SPI_WriteRead(R_RX_PAYLOAD);            // Send cmd to read rx payload
	
	for (uint8_t i = 0; i < mirf_PAYLOAD; i++)
	{
		data[i] = SPI_WriteRead(data[i]);
	}
	
    spi_transfer_sync(data,data,mirf_PAYLOAD); // Read payload
    mirf_CSN_hi;                               // Pull up chip select
    mirf_config_register(STATUS,(1<<RX_DR));   // Reset status register
}

void mirf_config_register(uint8_t reg, uint8_t value)
// Clocks only one byte into the given MiRF register
{
    mirf_CSN_lo;
    SPI_WriteRead(W_REGISTER | (REGISTER_MASK & reg));
    SPI_WriteRead(value);
    mirf_CSN_hi;
}

void mirf_read_register(uint8_t reg, uint8_t * value, uint8_t len)
// Reads an array of bytes from the given start position in the MiRF registers.
{
    mirf_CSN_lo;
    SPI_WriteRead(R_REGISTER | (REGISTER_MASK & reg));
    spi_transfer_sync(value,value,len);
    mirf_CSN_hi;
}

void mirf_write_register(uint8_t reg, uint8_t * value, uint8_t len) 
// Writes an array of bytes into inte the MiRF registers.
{
    mirf_CSN_lo;
    SPI_WriteRead(W_REGISTER | (REGISTER_MASK & reg));
    spi_transmit_sync(value,len);
    mirf_CSN_hi;
}


void mirf_send(uint8_t * value, uint8_t len) 
// Sends a data package to the default address. Be sure to send the correct
// amount of bytes as configured as payload on the receiver.
{
    while (PTX) {}                  // Wait until last paket is send

    mirf_CE_lo;

    PTX = 1;                        // Set to transmitter mode
    TX_POWERUP;                     // Power up
    
    mirf_CSN_lo;                    // Pull down chip select
    SPI_WriteRead(FLUSH_TX);     // Write cmd to flush tx fifo
    mirf_CSN_hi;                    // Pull up chip select
    
    mirf_CSN_lo;                    // Pull down chip select
    SPI_WriteRead(W_TX_PAYLOAD); // Write cmd to write payload
    spi_transmit_sync(value,len);   // Write payload
    mirf_CSN_hi;                    // Pull up chip select
    
    mirf_CE_hi;                     // Start transmission
}
