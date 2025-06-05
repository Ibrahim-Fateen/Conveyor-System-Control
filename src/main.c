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

#define PWM_FORWARD_PIN GPIO_A, 6
#define PWM_BACK_PIN GPIO_A, 7

#define MOTOR_FORWARD_PIN GPIO_A, 5
#define MOTOR_REVERSE_PIN GPIO_A, 4

#define MOTOR_DIRECTION_FORWARD_PIN GPIO_B, 12
#define MOTOR_DIRECTION_BACK_PIN GPIO_B, 14

#define TRUE 1
#define FALSE 0

typedef enum {
    FORWARD,
    REVERSE,
    STOP
} MOTOR_STATE;

int emergency = FALSE;
int motor_state = FORWARD;
int PWM = 50;

void init_RCC(void);
void init_GPIO(void);
void init_EXTI(void);

void poll_for_object(uint32 *counter);

float time_calc(void);

void control_motor(void);

void find_motor_direction(void);

void display_message(char *message, uint8 row, uint8 col);

void int_to_str(uint32 num, char *buffer);

int main(void) {
    init_RCC();
    init_GPIO();
    init_EXTI();
    LCD_Init();
    Timer2_InputCapture_Init(TIM_CHANNEL_1, IC_RISING_EDGE);
    Timer3_PWM_Init(PWM_CHANNEL_1);
    Timer3_PWM_Init(PWM_CHANNEL_2);

    display_message("Program Starting...", 0, 0);
    for (volatile long i = 0; i < 1000000; i++);

    uint32 obj_count = 0;
    char buffer[10];

    LCD_Clear();
    display_message("Counting...", 0, 0);
    display_message("Objects:", 1, 0);

    while (1) {
        if (!emergency) {
            if (motor_state == FORWARD) poll_for_object(&obj_count);
            int_to_str(obj_count, buffer);
            display_message(buffer, 1, 9);

            float time_s = time_calc();
            if (time_s > 0) {
                char time_buffer[16];
                int_to_str((uint32)(time_s * 1000), time_buffer); // Convert to milliseconds
                display_message(time_buffer, 0, 0); // Overwrite first line
            }

            EXTI_Disable(2);
            find_motor_direction();
            EXTI_Enable(2);
        } else {
            display_message("", 0, 0);
        }
        control_motor();
    }
    return 0;
}

void init_RCC(void) {
    Rcc_Init();

    uint8 used_peripherals[] = {RCC_GPIOA, RCC_GPIOB, RCC_TIM2, RCC_TIM3, RCC_SYSCFG,RCC_TIM3};
    for (int i = 0; i < sizeof(used_peripherals); i++) {
        Rcc_Enable(used_peripherals[i]);
    }
}

void init_GPIO(void) {
    // all pins with their respective properties set
    Gpio_Init(EMERGENCY_BUTTON, GPIO_INPUT, GPIO_PULL_UP);
    Gpio_Init(RESET_BUTTON, GPIO_INPUT, GPIO_PULL_UP);
    Gpio_Init(OBJ_PIN, GPIO_INPUT, GPIO_PULL_UP);
    Gpio_Init(MOTOR_DIRECTION_FORWARD_PIN, GPIO_INPUT, GPIO_PULL_UP);
    Gpio_Init(MOTOR_DIRECTION_BACK_PIN, GPIO_INPUT, GPIO_PULL_UP);

    Gpio_Init(MOTOR_FORWARD_PIN, GPIO_OUTPUT, GPIO_PUSH_PULL);
    Gpio_Init(MOTOR_REVERSE_PIN, GPIO_OUTPUT, GPIO_PUSH_PULL);

    Gpio_Init(INPUT_CAPTURE_PIN, GPIO_AF, GPIO_PUSH_PULL);
    Gpio_Set_AF(INPUT_CAPTURE_PIN, AF_TIM_1_2);

    Gpio_Init(PWM_FORWARD_PIN, GPIO_AF, GPIO_PUSH_PULL);
    Gpio_Set_AF(PWM_FORWARD_PIN, AF_TIM_3_5);

    Gpio_Init(PWM_BACK_PIN, GPIO_AF, GPIO_PUSH_PULL);
    Gpio_Set_AF(PWM_BACK_PIN, AF_TIM_3_5);
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

float time_calc(void) {
    static uint16 firstEdge = 0;
    static uint16 firstCaptured = 0;
    static float time_s = 0;

    uint16 captured = Timer2_ReadCapturedValue(TIM_CHANNEL_1);

    if (!firstCaptured) {
        firstEdge = captured;
        firstCaptured = 1;
    } else {
        uint16 secondEdge = captured;

        uint16 pulseWidth;
        if (secondEdge >= firstEdge)
            pulseWidth = secondEdge - firstEdge;
        else
            pulseWidth = (0xFFFF - firstEdge) + secondEdge + 1;

        time_s = pulseWidth;

        firstCaptured = 0;
    }
    return time_s ;
}

void find_motor_direction(void) {
    if (Gpio_ReadPin(MOTOR_DIRECTION_FORWARD_PIN) == LOW) {
        if (motor_state == REVERSE) {
            motor_state = FORWARD;
            control_motor();
        }
    }
    else if (Gpio_ReadPin(MOTOR_DIRECTION_BACK_PIN) == LOW) {
        if (motor_state == FORWARD) {
            motor_state = REVERSE;
            control_motor();
        }
    }
}

void control_motor(void) {
    switch (motor_state) {
        case FORWARD:
            // disable reverse
            // add dead time
            // enable forward
            Timer3_Set_PWM_Duty(PWM_CHANNEL_2, 0);
            Gpio_WritePin(MOTOR_REVERSE_PIN, LOW);
            for (int i = 0; i < 100000; i++) ;
            Timer3_Set_PWM_Duty(PWM_CHANNEL_1, PWM);
            Gpio_WritePin(MOTOR_FORWARD_PIN, HIGH);
            break;
        case REVERSE:
            Timer3_Set_PWM_Duty(PWM_CHANNEL_1, 0);
            Gpio_WritePin(MOTOR_FORWARD_PIN, LOW);
            for (int i = 0; i < 100000; i++) ;
            Timer3_Set_PWM_Duty(PWM_CHANNEL_2, PWM);
            Gpio_WritePin(MOTOR_REVERSE_PIN, HIGH);
            break;
        case STOP:
            Timer3_Set_PWM_Duty(PWM_CHANNEL_1, 0);
            Timer3_Set_PWM_Duty(PWM_CHANNEL_2, 0);
            Gpio_WritePin(MOTOR_FORWARD_PIN, HIGH);
            Gpio_WritePin(MOTOR_REVERSE_PIN, HIGH);
            break;
        default:
            break;
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
        motor_state = STOP;
        control_motor();
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
