//
// Created by joody on 6/3/2025.
//
#ifndef TIMER_PRIVATE_H
#define TIMER_PRIVATE_H

#include "Std_Types.h"

// TIM2 base address (APB1 bus)
#define TIM2_BASE_ADDR   0x40000000
#define TIM3_BASE_ADDR   0x40000400
#define TIM4_BASE_ADDR   0x40000800


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
    volatile uint32 RCR;
    volatile uint32 CCR1;
    volatile uint32 CCR2;
    volatile uint32 CCR3;
    volatile uint32 CCR4;
    volatile uint32 BDTR;
    volatile uint32 DCR;
    volatile uint32 DMAR;
} TIMx_Registers;



#define TIM2   ((TIMx_Registers *) TIM2_BASE_ADDR)
#define TIM3   ((TIMx_Registers *) TIM3_BASE_ADDR)
#define TIM4   ((TIMx_Registers *) TIM4_BASE_ADDR)




#endif

