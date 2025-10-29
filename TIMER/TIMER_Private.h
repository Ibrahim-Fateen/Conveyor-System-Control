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
    volatile uint32 CR1;      // Control Register 1
volatile uint32 CR2;      // Control Register 2
volatile uint32 SMCR;     // Slave Mode Control Register
volatile uint32 DIER;     // DMA/Interrupt Enable Register
volatile uint32 SR;       // Status Register
volatile uint32 EGR;      // Event Generation Register
volatile uint32 CCMR1;    // Capture/Compare Mode Register 1
volatile uint32 CCMR2;    // Capture/Compare Mode Register 2
volatile uint32 CCER;     // Capture/Compare Enable Register
volatile uint32 CNT;      // Counter Register
volatile uint32 PSC;      // Prescaler Register
volatile uint32 ARR;      // Auto-Reload Register
volatile uint32 RCR;      // Repetition Counter Register
volatile uint32 CCR1;     // Capture/Compare Register 1
volatile uint32 CCR2;     // Capture/Compare Register 2
volatile uint32 CCR3;     // Capture/Compare Register 3
volatile uint32 CCR4;     // Capture/Compare Register 4
volatile uint32 BDTR;     // Break and Dead-Time Register
volatile uint32 DCR;      // DMA Control Register
volatile uint32 DMAR;     // DMA Address Register
} TIMx_Registers;



// #define TIM2   ((TIMx_Registers *) TIM2_BASE_ADDR)
// #define TIM3   ((TIMx_Registers *) TIM3_BASE_ADDR)
// #define TIM4   ((TIMx_Registers *) TIM4_BASE_ADDR)




#endif

