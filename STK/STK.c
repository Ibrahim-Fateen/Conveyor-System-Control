//
// Created by Guest account on 6/9/2025.
//
#include "Std_Types.h"
#include "STK.h"
#include "STK_private.h"

volatile uint32 ms_ticks = 0;

void init_SysTick (void) {
    uint32 ticks_per_ms = 16000000 / 1000; // not sure, need to check

    SysTick->LOAD = ticks_per_ms - 1; // counts down from this value, will trigger an interrupt once the counter reaches 0
    SysTick->VAL = 0;
    SysTick->CTRL |= (1 << 2); // set clock frequency as processor clock (16 MHz)
    SysTick->CTRL |= (1 << 1); // enable SysTick interrupt
    SysTick->CTRL |= (1 << 0); // enable timer
}

void delay_ms (uint8 ms) {
    uint32 start = millis();
    while ((millis() - start) < ms) {}
}

uint32 millis (void) {
    return ms_ticks;
}

void SysTick_Handler (void) {
    ms_ticks++;
}
