//
// Created by Guest account on 6/9/2025.
//

#ifndef STK_PRIVATE_H
#define STK_PRIVATE_H
#include "Std_Types.h"

#define STK_BASE_ADDRESS 0xE000E010

typedef struct {
    volatile uint32 CTRL;
    volatile uint32 LOAD;
    volatile uint32 VAL;
    volatile uint32 CALIB;
} STK_Registers;

#define SysTick ((volatile STK_Registers *) STK_BASE_ADDRESS)

#endif //STK_PRIVATE_H
