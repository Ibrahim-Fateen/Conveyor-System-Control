#ifndef TIMER_H
#define TIMER_H

#include "Std_Types.h"

typedef enum {
    CHANNEL_1,
    CHANNEL_2,
    CHANNEL_3,
    CHANNEL_4
} Channel;

#define TIM_2 0
#define TIM_3 1
#define TIM_4 2

typedef enum {
    INPUT_CAPTURE,
    PWM_OUTPUT
} TIMER_MODE;

typedef enum {
    IC_RISING,
    IC_FALLING,
    IC_BOTH
} EDGE_TRIGGER;

void TIMER_Init(uint8 timer, TIMER_MODE mode, Channel channel, EDGE_TRIGGER edge_trigger);

uint16 ReadCapturedValue(uint8 timer, Channel channel);

uint8 Timer_HasOverflow(uint8 timer);

void Timer_ClearOverflow(uint8 timer);

void Timer_Set_PWM_Duty(uint8 timer, Channel channel, uint8 duty_cycle_percent);

#endif
