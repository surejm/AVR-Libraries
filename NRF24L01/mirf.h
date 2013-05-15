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

#ifndef _MIRF_H_
#define _MIRF_H_

// Uses SPI
#ifdef SPI_IN_USE
#error "NRF24L01 need SPI. Used somewhere else"
#else
#define SPI_IN_USE
#endif

#include <avr/io.h>

// Mirf settings
#define mirf_CH         2
#define mirf_PAYLOAD    16
#define mirf_CONFIG     ( (1<<MASK_RX_DR) | (1<<EN_CRC) | (0<<CRCO) )


/*
 nRF24L01   AVR ATmega328(p)	Description
 SCK        PB5 (SCK)			SPI clock signal
 MISO       PB4 (MISO)			SPI master in, slave out
 MOSI       PB3 (MOSI)			SPI master out, slave in
 CSN        PORTD3				SPI Chip Select
 CE         PORTD4				Chip Enable Activates RX or TX mode
 IRQ        PORTD2 (INT0)		Interrupt pin. Active low
 */

// Pin definitions for chip select and chip enabled of the MiRF module
#define CE  PORTD4
#define CSN PORTD3

// Definitions for selecting and enabling MiRF module
#define mirf_CSN_hi     PORTD |=  (1<<CSN);
#define mirf_CSN_lo     PORTD &= ~(1<<CSN);
#define mirf_CE_hi      PORTD |=  (1<<CE);
#define mirf_CE_lo      PORTD &= ~(1<<CE);

// Public standart functions
extern void mirf_init();
extern void mirf_config();
extern void mirf_send(uint8_t * value, uint8_t len);
extern void mirf_set_RADDR(uint8_t * adr);
extern void mirf_set_TADDR(uint8_t * adr);
extern uint8_t mirf_data_ready();
extern void mirf_get_data(uint8_t * data);


// Public extended functions
extern void mirf_config_register(uint8_t reg, uint8_t value);
extern void mirf_read_register(uint8_t reg, uint8_t * value, uint8_t len);
extern void mirf_write_register(uint8_t reg, uint8_t * value, uint8_t len);

#endif /* _MIRF_H_ */
