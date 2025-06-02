#include "RCC.h"
#include "GPIO.h"
#include "EXTI.h"
#include "SCB.h"
#include "Std_Types.h"

#define EMERGENCY_BUTTON GPIO_A, 2
#define RESET_BUTTON GPIO_A, 3
#define DUMMY_LED GPIO_A, 4

int state = HIGH;
int emergency = 0;

void init_RCC(void);
void init_GPIO(void);
void init_EXTI(void);

int main(void) {
    init_RCC();
    init_GPIO();
    init_EXTI();

    while (1) {
        if (!emergency) {
            state = !state;
            Gpio_WritePin(DUMMY_LED, state);
            for (volatile long i = 0; i < 1000000; i++);
        }
    }
    return 0;
}

void init_RCC(void) {
    Rcc_Init();

    uint8 used_peripherals[] = {RCC_GPIOA, RCC_SYSCFG};
    for (int i = 0; i < sizeof(used_peripherals); i++) {
        Rcc_Enable(used_peripherals[i]);
    }
}

void init_GPIO(void) {
    // all pins with their respective properties set
    Gpio_Init(EMERGENCY_BUTTON, GPIO_INPUT, GPIO_PULL_UP);
    Gpio_Init(RESET_BUTTON, GPIO_INPUT, GPIO_PULL_UP);

    Gpio_Init(DUMMY_LED, GPIO_OUTPUT, GPIO_PUSH_PULL);
    Gpio_WritePin(DUMMY_LED, state);
}

void init_EXTI(void) {
    EXTI_Init(EMERGENCY_BUTTON, FALLING_EDGE_TRIGGERED);
    EXTI_Enable(2);

    EXTI_Init(RESET_BUTTON, FALLING_EDGE_TRIGGERED);
    EXTI_Enable(3);
}

// emergency button interrupt request handler
// stop motor
// show "EMERGENCY STOP" on LCD
void EXTI2_IRQHandler(void) {
    if (EXTI_REGISTERS->EXTI_PR & (1 << 2)) {
        emergency = 1;
        Gpio_WritePin(DUMMY_LED, HIGH);
        EXTI_ClearPending(2);
    }
}

// reset button interrupt request handler
void EXTI3_IRQHandler(void) {
    if (EXTI_REGISTERS->EXTI_PR & (1 << 3)) {
        SCB_Reset();
    }
}
