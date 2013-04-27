/*
 * nec_ir.h
 *
 * Created: 2012-10-24 19:23:03
 *  Author: Hampus
 */ 

#ifndef NEC_IR_H
#define NEC_IR_H

// Uses TIMER2, and INT1
#if defined(INT1_IN_USE) || defined(TIMER2_IN_USE)
#error "NEC IR need INT1 and TIMER2. Used somewhere else"
#else
#define INT1_IN_USE
#define TIMER2_IN_USE
#endif


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

void NEC_IR_Init(void(*theIrManageDataFunc)(uint32_t));

#endif