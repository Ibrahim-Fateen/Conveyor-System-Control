//
// Created by joody on 6/3/2025.
//
#include "TIMER.h"
#include "TIMER_Private.h"
#include "stm32f4xx.h"
void Timer2_InputCapture_Init(uint8 Channel, uint8 EdgeSelect)
{
    // 2. Basic timer config
    TIM2->PSC = 16000-1; // Prescaler (adjust for slower signal)
    TIM2->EGR |= (0x1UL<<(0U));
    TIM2->SR &= ~((0x1UL<<(0U)));

    TIM2->ARR = 0xFFFF;  // Max count


    // 3. Configure channel as input capture
    switch (Channel)
    {
        case TIM_CHANNEL_1:
            TIM2->CCMR1 &= ~(0x3 << 0);  // CC1S = 01
            TIM2->CCMR1 |= (0x1 << 0);   // Input, mapped on TI1
            break;

        case TIM_CHANNEL_2:
            TIM2->CCMR1 &= ~(0x3 << 8);  // CC2S = 01
            TIM2->CCMR1 |= (0x1 << 8);
            break;

        case TIM_CHANNEL_3:
            TIM2->CCMR2 &= ~(0x3 << 0);  // CC3S = 01
            TIM2->CCMR2 |= (0x1 << 0);
            break;

        case TIM_CHANNEL_4:
            TIM2->CCMR2 &= ~(0x3 << 8);  // CC4S = 01
            TIM2->CCMR2 |= (0x1 << 8);
            break;

        default:
            return;
    }

    // 4. Configure edge
    switch (Channel)
    {
        case TIM_CHANNEL_1:
            if (EdgeSelect == IC_RISING_EDGE)
                TIM2->CCER &= ~(1 << 1);  // CC1P = 0
            else if (EdgeSelect == IC_FALLING_EDGE)
                TIM2->CCER |= (1 << 1);   // CC1P = 1
            else
                TIM2->CCER |= (1 << 3);   // CC1NP = 1 (both edges)
            TIM2->CCER |= (1 << 0);       // CC1E = 1
            break;

        case TIM_CHANNEL_2:
            if (EdgeSelect == IC_RISING_EDGE)
                TIM2->CCER &= ~(1 << 5);
            else if (EdgeSelect == IC_FALLING_EDGE)
                TIM2->CCER |= (1 << 5);
            else
                TIM2->CCER |= (1 << 7);
            TIM2->CCER |= (1 << 4);
            break;

        case TIM_CHANNEL_3:
            if (EdgeSelect == IC_RISING_EDGE)
                TIM2->CCER &= ~(1 << 9);
            else if (EdgeSelect == IC_FALLING_EDGE)
                TIM2->CCER |= (1 << 9);
            else
                TIM2->CCER |= (1 << 11);
            TIM2->CCER |= (1 << 8);
            break;

        case TIM_CHANNEL_4:
            if (EdgeSelect == IC_RISING_EDGE)
                TIM2->CCER &= ~(1 << 13);
            else if (EdgeSelect == IC_FALLING_EDGE)
                TIM2->CCER |= (1 << 13);
            else
                TIM2->CCER |= (1 << 15);
            TIM2->CCER |= (1 << 12);
            break;
    }

    // 5. Enable counter
    TIM2->CR1 |= 1;
}

uint16 Timer2_ReadCapturedValue(uint8 Channel)
{
    switch (Channel)
    {
        case TIM_CHANNEL_1:
            return (uint16)(TIM2->CCR1);
        case TIM_CHANNEL_2:
            return (uint16)(TIM2->CCR2);
        case TIM_CHANNEL_3:
            return (uint16)(TIM2->CCR3);
        case TIM_CHANNEL_4:
            return (uint16)(TIM2->CCR4);
        default:
            return 0;
    }


}

uint8 Timer2_HasOverflowed(void) {
    return (TIM2->SR & (1 << 0)) ? 1 : 0;
}

void Timer2_ClearOverflowFlag(void) {
    TIM2->SR &= ~(1 << 0);
}

void Timer3_PWM_Init(PWM_Channel channel) {
    // Enable TIM3 clock (in RCC before calling this)
    TIM3->PSC = 160000 - 1; // 1 MHz timer clock
    TIM3->EGR |= (0x1UL<<(0U));
    TIM3->SR &= ~((0x1UL<<(0U)));
    TIM3->ARR = 1000 - 1; // 1 kHz PWM frequency

    // Set PWM mode 1 and enable preload
    switch (channel) {
        case PWM_CHANNEL_1:
            TIM3->CCMR1 &= ~TIM_CCMR1_OC1M;
            TIM3->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos); // PWM mode 1
            TIM3->CCMR1 |= TIM_CCMR1_OC1PE; // preload enable
            TIM3->CCER |= TIM_CCER_CC1E;    // enable output
            break;
        case PWM_CHANNEL_2:
            TIM3->CCMR1 &= ~TIM_CCMR1_OC2M;
            TIM3->CCMR1 |= (6 << TIM_CCMR1_OC2M_Pos);
            TIM3->CCMR1 |= TIM_CCMR1_OC2PE;
            TIM3->CCER |= TIM_CCER_CC2E;
            break;
        case PWM_CHANNEL_3:
            TIM3->CCMR2 &= ~TIM_CCMR2_OC3M;
            TIM3->CCMR2 |= (6 << TIM_CCMR2_OC3M_Pos);
            TIM3->CCMR2 |= TIM_CCMR2_OC3PE;
            TIM3->CCER |= TIM_CCER_CC3E;
            break;
        case PWM_CHANNEL_4:
            TIM3->CCMR2 &= ~TIM_CCMR2_OC4M;
            TIM3->CCMR2 |= (6 << TIM_CCMR2_OC4M_Pos);
            TIM3->CCMR2 |= TIM_CCMR2_OC4PE;
            TIM3->CCER |= TIM_CCER_CC4E;
            break;
    }

    TIM3->CR1 |= TIM_CR1_ARPE; // auto-reload preload
    TIM3->EGR = TIM_EGR_UG;    // update registers
    TIM3->CR1 |= TIM_CR1_CEN;  // enable timer
}

void Timer3_Set_PWM_Duty(PWM_Channel channel, uint8 duty_cycle_percent) {
    if (duty_cycle_percent > 100) duty_cycle_percent = 100;
    uint16 compare_value = ((uint32)duty_cycle_percent * (TIM3->ARR + 1)) / 100;

    switch (channel) {
        case PWM_CHANNEL_1: TIM3->CCR1 = compare_value; break;
        case PWM_CHANNEL_2: TIM3->CCR2 = compare_value; break;
        case PWM_CHANNEL_3: TIM3->CCR3 = compare_value; break;
        case PWM_CHANNEL_4: TIM3->CCR4 = compare_value; break;
    }
}
