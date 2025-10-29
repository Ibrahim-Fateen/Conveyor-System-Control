//
// Created by joody on 6/3/2025.
//
#include "TIMER.h"
#include "TIMER_Private.h"
#include "stm32f4xx.h"

uint32 TIMERs_address_map[] = { TIM2_BASE_ADDR, TIM3_BASE_ADDR, TIM4_BASE_ADDR };

void Timer_InputCapture_Init(uint8 timer, Channel channel, EDGE_TRIGGER edge_trigger) {
    TIMx_Registers * TIMx = (TIMx_Registers *)TIMERs_address_map[timer - TIM_2];

    // 2. Basic timer config
    TIMx->PSC = 16000 - 1; // Prescaler (adjust for slower signal)  (16MHz / (PSC + 1))
    TIMx->EGR |= TIM_EGR_UG;  // Force applying to prescalar // event generation register 
    TIMx->SR &= ~TIM_SR_UIF; //update interrup flag 

    TIMx->ARR = 0xFFFFFFFF;  // Max count


    // 3. Configure channel as input capture
    switch (channel)
    {
        case CHANNEL_1:
            TIMx->CCMR1 &= ~(0x3 << 0);  // CC1S = 01
            TIMx->CCMR1 |= (0x1 << 0);   // Input, mapped on TI1
            break;

        case CHANNEL_2:
            TIMx->CCMR1 &= ~(0x3 << 8);  // CC2S = 01
            TIMx->CCMR1 |= (0x1 << 8);
            break;

        case CHANNEL_3:
            TIMx->CCMR2 &= ~(0x3 << 0);  // CC3S = 01
            TIMx->CCMR2 |= (0x1 << 0);
            break;

        case CHANNEL_4:
            TIMx->CCMR2 &= ~(0x3 << 8);  // CC4S = 01
            TIMx->CCMR2 |= (0x1 << 8);
            break;

        default:
            return;
    }

    // 4. Configure edge
    switch (channel)
    {
        case CHANNEL_1:
            if (edge_trigger == IC_RISING)
                TIMx->CCER &= ~(1 << 1);  // CC1P = 0
            else if (edge_trigger == IC_FALLING)
                TIMx->CCER |= (1 << 1);   // CC1P = 1
            else
                TIMx->CCER |= (1 << 3);   // CC1NP = 1 (both edges)
            TIMx->CCER |= (1 << 0);       // CC1E = 1
            break;

        case CHANNEL_2:
            if (edge_trigger == IC_RISING)
                TIMx->CCER &= ~(1 << 5);
            else if (edge_trigger == IC_FALLING)
                TIMx->CCER |= (1 << 5);
            else
                TIMx->CCER |= (1 << 7);
            TIMx->CCER |= (1 << 4);
            break;

        case CHANNEL_3:
            if (edge_trigger == IC_RISING)
                TIMx->CCER &= ~(1 << 9);
            else if (edge_trigger == IC_FALLING)
                TIMx->CCER |= (1 << 9);
            else
                TIMx->CCER |= (1 << 11);
            TIMx->CCER |= (1 << 8);
            break;

        case CHANNEL_4:
            if (edge_trigger == IC_RISING)
                TIMx->CCER &= ~(1 << 13);
            else if (edge_trigger == IC_FALLING)
                TIMx->CCER |= (1 << 13);
            else
                TIMx->CCER |= (1 << 15);
            TIMx->CCER |= (1 << 12);
            break;
    }

    // 5. Enable counter
    TIMx->CR1 |= 1;
}

void Timer_PWM_Init(uint8 timer, Channel channel) {
    TIMx_Registers * TIMx = (TIMx_Registers *)TIMERs_address_map[timer - TIM_2];

    TIMx->PSC = 16 - 1;
    TIMx->EGR|= TIM_EGR_UG;
    TIMx->SR &= ~TIM_SR_UIF;
    TIMx->ARR = 1000 - 1;  // period (in terms of ticks)

    // Set PWM mode 1 and enable preload
    switch (channel) {
        case CHANNEL_1:
            TIMx->CCMR1 &= ~TIM_CCMR1_OC1M;
            TIMx->CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos); // PWM mode 1
            TIMx->CCMR1 |= TIM_CCMR1_OC1PE; // preload enable
            TIMx->CCER |= TIM_CCER_CC1E;    // enable output
            break;
        case CHANNEL_2:
            TIMx->CCMR1 &= ~TIM_CCMR1_OC2M;
            TIMx->CCMR1 |= (6 << TIM_CCMR1_OC2M_Pos);
            TIMx->CCMR1 |= TIM_CCMR1_OC2PE;
            TIMx->CCER |= TIM_CCER_CC2E;
            break;
        case CHANNEL_3:
            TIMx->CCMR2 &= ~TIM_CCMR2_OC3M;
            TIMx->CCMR2 |= (6 << TIM_CCMR2_OC3M_Pos);
            TIMx->CCMR2 |= TIM_CCMR2_OC3PE;
            TIMx->CCER |= TIM_CCER_CC3E;
            break;
        case CHANNEL_4:
            TIMx->CCMR2 &= ~TIM_CCMR2_OC4M;
            TIMx->CCMR2 |= (6 << TIM_CCMR2_OC4M_Pos);
            TIMx->CCMR2 |= TIM_CCMR2_OC4PE;
            TIMx->CCER |= TIM_CCER_CC4E;
            break;
    }

    TIMx->CR1 |= TIM_CR1_ARPE; // auto-reload preload
    TIMx->EGR = TIM_EGR_UG;    // update registers
    TIMx->CR1 |= TIM_CR1_CEN;  // enable timer
}

void TIMER_Init(uint8 timer, TIMER_MODE mode, Channel channel, EDGE_TRIGGER edge_trigger) {
    switch (mode) {
        case INPUT_CAPTURE:
            Timer_InputCapture_Init(timer, channel, edge_trigger);
            break;
        case PWM_OUTPUT:
            Timer_PWM_Init(timer, channel);
            break;
        default:
            return;
    }
}

uint16 ReadCapturedValue(uint8 timer, Channel channel) {
    TIMx_Registers * TIMx = (TIMx_Registers *)TIMERs_address_map[timer - TIM_2];
    switch (channel) {
        case CHANNEL_1:
            return (uint16) (TIMx->CCR1);
        case CHANNEL_2:
            return (uint16) (TIMx->CCR2);
        case CHANNEL_3:
            return (uint16) (TIMx->CCR3);
        case CHANNEL_4:
            return (uint16) (TIMx->CCR4);
        default:
            return 0;
    }
}

uint8 Timer_HasOverflow(uint8 timer) {
    TIMx_Registers * TIMx = (TIMx_Registers *)TIMERs_address_map[timer - TIM_2];
    return (TIMx->SR & (1 << 0)) ? 1 : 0;
}

void Timer_ClearOverflow(uint8 timer) {
    TIMx_Registers * TIMx = (TIMx_Registers *)TIMERs_address_map[timer - TIM_2];
    TIMx->SR &= ~(1 << 0);
}

void Timer_Set_PWM_Duty(uint8 timer, Channel channel, uint8 duty_cycle_percent) {
    if (duty_cycle_percent > 100) duty_cycle_percent = 100;
    TIMx_Registers * TIMx = (TIMx_Registers *)TIMERs_address_map[timer - TIM_2];

    uint16 compare_value = ((uint32)duty_cycle_percent * (TIMx->ARR + 1)) / 100;

    switch (channel) {
        case CHANNEL_1: TIMx->CCR1 = compare_value; break;
        case CHANNEL_2: TIMx->CCR2 = compare_value; break;
        case CHANNEL_3: TIMx->CCR3 = compare_value; break;
        case CHANNEL_4: TIMx->CCR4 = compare_value; break;
    }
}
