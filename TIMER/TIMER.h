#ifndef TIMER_H
#define TIMER_H

#include "Std_Types.h"

// Timer Channels
#define TIM_CHANNEL_1 1
#define TIM_CHANNEL_2 2
#define TIM_CHANNEL_3 3
#define TIM_CHANNEL_4 4

typedef enum {
    PWM_CHANNEL_1,
    PWM_CHANNEL_2,
    PWM_CHANNEL_3,
    PWM_CHANNEL_4
} PWM_Channel;

// Edge types
#define IC_RISING_EDGE  0
#define IC_FALLING_EDGE 1
#define IC_BOTH_EDGES   2

void Timer2_InputCapture_Init(uint8 Channel, uint8 EdgeSelect);

uint16 Timer2_ReadCapturedValue(uint8 Channel);

uint8 Timer2_HasOverflowed(void);

void Timer2_ClearOverflowFlag(void);


void Timer3_PWM_Init(PWM_Channel channel);

void Timer3_Set_PWM_Duty(PWM_Channel channel, uint8 duty_cycle_percent);

#endif
