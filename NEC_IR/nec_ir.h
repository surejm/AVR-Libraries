/*
 * nec_ir.h
 *
 * Created: 2012-10-24 19:23:03
 *  Author: Hampus
 */ 

#ifndef NEC_IR_H
#define NEC_IR_H

#ifndef HELP_DEFINITIONS
#define HELP_DEFINITIONS
#define TRUE	1
#define FALSE	0
#define HIGH	1
#define LOW		0
#endif

volatile uint32_t _microCount;

// Pointer to a function that handles received IR data
void (*_irManageDataFunc)(uint32_t);

void setupMicroCounter();
void startMicroCounter();
void stopMicroCounter();

void necIrSetup(void(*theIrManageDataFunc)(uint32_t));

#endif