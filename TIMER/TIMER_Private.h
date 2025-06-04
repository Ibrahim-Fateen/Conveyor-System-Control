//
// Created by joody on 6/3/2025.
//
#ifndef TIMER_PRIVATE_H
#define TIMER_PRIVATE_H

#include "Std_Types.h"

// TIM2 base address (APB1 bus)
#define TIM2_BASE      0x40000000U

// Timer register map
typedef struct {
    volatile uint32 CR1;
    volatile uint32 CR2;
    volatile uint32 SMCR;
    volatile uint32 DIER;
    volatile uint32 SR;
    volatile uint32 EGR;
    volatile uint32 CCMR1;
    volatile uint32 CCMR2;
    volatile uint32 CCER;
    volatile uint32 CNT;
    volatile uint32 PSC;
    volatile uint32 ARR;
    volatile uint32 RESERVED1;
    volatile uint32 CCR1;
    volatile uint32 CCR2;
    volatile uint32 CCR3;
    volatile uint32 CCR4;
    volatile uint32 RESERVED2;
    volatile uint32 DCR;
    volatile uint32 DMAR;
} TIM_TypeDef;

// Pointer to TIM2 register structure
#define TIM2 ((TIM_TypeDef *) TIM2_BASE)

#endif

