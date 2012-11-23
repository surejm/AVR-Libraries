/*
 * uart.h
 *
 * Created: 2012-09-14 22:58:47
 *  Author: Hampus
 */ 


#ifndef UART_H_
#define UART_H_

#define BAUD_9600	51
#define BAUD_19200	25
#define BAUD_38400	12
#define BAUD_57600	16
#define BAUD_115200	8

#include <avr/io.h>

// Pointer to a function that handles received UART data
void (*_uartManageDataFunc)(uint8_t);

void uartSetup(const uint8_t theBaudRate, void(*theUartManageFunc)(uint8_t));
void uartTransmit(const uint8_t theData);
void uartTransmitString(const char* theString);


#endif /* UART_H_ */