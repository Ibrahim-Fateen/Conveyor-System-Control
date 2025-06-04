#include "RCC.h"
#include "GPIO.h"
#include "EXTI.h"
#include "SCB.h"
#include "Std_Types.h"
#include "lcd.h"
#include "TIMER.h"

#define EMERGENCY_BUTTON GPIO_A, 2
#define RESET_BUTTON GPIO_A, 3

#define OBJ_PIN GPIO_A, 9

#define INPUT_CAPTURE_PIN GPIO_A, 0

#define TRUE 1
#define FALSE 0

int emergency = FALSE;

void init_RCC(void);
void init_GPIO(void);
void init_EXTI(void);

void poll_for_object(uint32 *counter);

float time_calc(uint16 firstEdge, uint16 secondEdge , uint16 firstCaptured);

void display_message(char *message, uint8 row, uint8 col);

void int_to_str(uint32 num, char *buffer);

int main(void) {
    init_RCC();
    init_GPIO();
    init_EXTI();
    LCD_Init();
    Timer2_InputCapture_Init(TIM_CHANNEL_1, IC_RISING_EDGE,83);

    uint16 firstEdge = 0, secondEdge = 0;
    uint8 firstCaptured = 0;

    display_message("Program Starting...", 0, 0);
    for (volatile long i = 0; i < 1000000; i++);

    uint32 obj_count = 0;
    char buffer[10];

    LCD_Clear();
    display_message("Counting...", 0, 0);
    display_message("Objects:", 1, 0);

    while (1) {
        if (!emergency) {
            poll_for_object(&obj_count);
            int_to_str(obj_count, buffer);
            display_message(buffer, 1, 9);
        } else {
            display_message("", 0, 0);
        }
    }
    return 0;
}

void init_RCC(void) {
    Rcc_Init();

    uint8 used_peripherals[] = {RCC_GPIOA, RCC_GPIOB, RCC_TIM2, RCC_SYSCFG};
    for (int i = 0; i < sizeof(used_peripherals); i++) {
        Rcc_Enable(used_peripherals[i]);
    }
}

void init_GPIO(void) {
    // all pins with their respective properties set
    Gpio_Init(EMERGENCY_BUTTON, GPIO_INPUT, GPIO_PULL_UP);
    Gpio_Init(RESET_BUTTON, GPIO_INPUT, GPIO_PULL_UP);
    Gpio_Init(OBJ_PIN, GPIO_INPUT, GPIO_PULL_UP);

    Gpio_Init(INPUT_CAPTURE_PIN, GPIO_AF, GPIO_PUSH_PULL);
    Gpio_Set_AF(INPUT_CAPTURE_PIN, AF_TIM_1_2);
}

void init_EXTI(void) {
    EXTI_Init(EMERGENCY_BUTTON, FALLING_EDGE_TRIGGERED);
    EXTI_Enable(2);

    EXTI_Init(RESET_BUTTON, FALLING_EDGE_TRIGGERED);
    EXTI_Enable(3);
}

void poll_for_object(uint32 *counter) {
    static uint8 last_pin_state = HIGH;
    uint8 current_pin_state = Gpio_ReadPin(OBJ_PIN);
    if (last_pin_state == HIGH && current_pin_state == LOW) {
        (*counter)++;
        for (int i = 0; i < 300000; i++) ;
    }
    last_pin_state = current_pin_state;
}

float time_calc(uint16 firstEdge, uint16 secondEdge, uint16 firstCaptured) {
    uint16 captured = Timer2_ReadCapturedValue(TIM_CHANNEL_1);

    if (!firstCaptured) {
        firstEdge = Timer2_ReadCapturedValue(TIM_CHANNEL_1);
        firstCaptured = 1;
    } else {
        secondEdge = Timer2_ReadCapturedValue(TIM_CHANNEL_1);

        uint16 pulseWidth;
        if (secondEdge >= firstEdge)
            pulseWidth = secondEdge - firstEdge;
        else
            pulseWidth = (0xFFFF - firstEdge) + secondEdge + 1;

        float time_us = pulseWidth;
        float time_ms = time_us / 1000.0;
        float time_s = time_us / 1e6;

        firstCaptured = 0;

        return time_s ;
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

void display_message(char *message, uint8 row, uint8 col) {
    static uint8 first_emergency_call = 1;
    if (emergency) {
        if (first_emergency_call) {
            LCD_Clear();
            LCD_SetCursor(0, 0);
            LCD_SendString("EMERGENCY STOP");
            first_emergency_call = 0;
        }
        return;
    }
    LCD_SetCursor(row, col);
    LCD_SendString(message);
}

// emergency button interrupt request handler
void EXTI2_IRQHandler(void) {
    if (EXTI_REGISTERS->EXTI_PR & (1 << 2)) {
        emergency = TRUE;
        EXTI_ClearPending(2);
    }
}

// reset button interrupt request handler
void EXTI3_IRQHandler(void) {
    if (EXTI_REGISTERS->EXTI_PR & (1 << 3)) {
        EXTI_ClearPending(3);
        SCB_Reset();
    }
}
