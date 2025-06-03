#include "RCC.h"
#include "GPIO.h"
#include "EXTI.h"
#include "SCB.h"
#include "Std_Types.h"
#include "lcd.h"

// #define EMERGENCY_BUTTON GPIO_A, 2
// #define RESET_BUTTON GPIO_A, 3
// #define DUMMY_LED GPIO_A, 4

// int state = HIGH;
// int emergency = 0;

// void init_RCC(void);
// void init_GPIO(void);
// void init_EXTI(void);

// int main(void) {
//     init_RCC();
//     init_GPIO();
//     init_EXTI();

//     while (1) {
//         if (!emergency) {
//             state = !state;
//             Gpio_WritePin(DUMMY_LED, state);
//             for (volatile long i = 0; i < 1000000; i++);
//         }
//     }
//     return 0;
// }

// void init_RCC(void) {
//     Rcc_Init();

//     uint8 used_peripherals[] = {RCC_GPIOA, RCC_SYSCFG};
//     for (int i = 0; i < sizeof(used_peripherals); i++) {
//         Rcc_Enable(used_peripherals[i]);
//     }
// }

// void init_GPIO(void) {
//     // all pins with their respective properties set
//     Gpio_Init(EMERGENCY_BUTTON, GPIO_INPUT, GPIO_PULL_UP);
//     Gpio_Init(RESET_BUTTON, GPIO_INPUT, GPIO_PULL_UP);

//     Gpio_Init(DUMMY_LED, GPIO_OUTPUT, GPIO_PUSH_PULL);
//     Gpio_WritePin(DUMMY_LED, state);
// }

// void init_EXTI(void) {
//     EXTI_Init(EMERGENCY_BUTTON, FALLING_EDGE_TRIGGERED);
//     EXTI_Enable(2);

//     EXTI_Init(RESET_BUTTON, FALLING_EDGE_TRIGGERED);
//     EXTI_Enable(3);
// }

// // emergency button interrupt request handler
// // stop motor
// // show "EMERGENCY STOP" on LCD
// void EXTI2_IRQHandler(void) {
//     if (EXTI_REGISTERS->EXTI_PR & (1 << 2)) {
//         emergency = 1;
//         Gpio_WritePin(DUMMY_LED, HIGH);
//         EXTI_ClearPending(2);
//     }
// }

// // reset button interrupt request handler
// void EXTI3_IRQHandler(void) {
//     if (EXTI_REGISTERS->EXTI_PR & (1 << 3)) {
//         SCB_Reset();
//     }
// }



// #include "GPIO.h"
// #include "Std_Types.h"
// #include "delay.h" // You must define delay_ms() in this file or link it


void int_to_str(uint32 num, char *buffer);

int main(void)
{
    Rcc_Init();
    Rcc_Enable(RCC_GPIOB);
    LCD_Init();

    for (int i = 0; i < 6; i++) {
        Gpio_Init(GPIO_B, i, GPIO_OUTPUT, GPIO_PUSH_PULL);
    }

    LCD_SetCursor(0, 0);
    LCD_SendString("Hello, Mostafa!");

    uint32 seconds = 0;
    char buffer[10];

    while (1)
    {
        LCD_SetCursor(1, 0);
        LCD_SendString("Seconds: ");
        
        int_to_str(seconds, buffer);         
        LCD_SendString(buffer);              

        for(int i =0 ; i<16000; i++);        
        seconds++;                           
    }
}


void int_to_str(uint32 num, char *buffer)
{
    int i = 0;
    if (num == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return;
    }

    while (num != 0) {
        buffer[i++] = (num % 10) + '0';
        num /= 10;
    }
    buffer[i] = '\0';
    for (int j = 0; j < i / 2; j++) {
        char temp = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp;
    }
}

