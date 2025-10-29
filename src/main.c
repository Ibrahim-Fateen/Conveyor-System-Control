#include "RCC.h"
#include "GPIO.h"
#include "EXTI.h"
#include "SCB.h"
#include "Std_Types.h"
#include "lcd.h"
#include "TIMER.h"
#include "stm32f4xx.h"
#include "ADC.h"
#include "STK.h"

#define EMERGENCY_BUTTON GPIO_A, 2  // set correct line number in exti
#define RESET_BUTTON GPIO_A, 3  // set correct line number in exti

#define OBJ_DETECTION_PIN GPIO_A, 9

#define SPEED_MEASUREMENT_PIN GPIO_A, 0
#define POT_SPEED_CONTROL_PIN GPIO_A, 1

#define PWM_FORWARD_PIN GPIO_A, 6
#define PWM_REVERSE_PIN GPIO_A, 7

#define MOTOR_FORWARD_PIN GPIO_A, 5
#define MOTOR_REVERSE_PIN GPIO_A, 4

#define MOTOR_DIRECTION_FORWARD_PIN GPIO_B, 12
#define MOTOR_DIRECTION_REVERSE_PIN GPIO_B, 14

#define PWM_TIMER TIM_3
#define IC_TIMER TIM_2

#define FORWARD_CHANNEL CHANNEL_1
#define REVERSE_CHANNEL CHANNEL_2

#define TRUE 1
#define FALSE 0

#define R_ARROW 0x7E
#define L_ARROW 0x7F

typedef enum {
    FORWARD,
    REVERSE,
    STOP
} MOTOR_STATE;

// initializers
void init_RCC(void);
void init_GPIO(void);
void init_TIM(void);
void init_ADC(uint8 mode);
void init_EXTI(void);

// program logic
void measure_speed(void);
void control_motor(void);
void poll_for_object(void);

// helpers
void update_speed(uint16 new_speed);
void update_count(void);
void display_information(void);
void display_message(char *message, uint8 row, uint8 col);
void int_to_str(uint32 num, char *buffer);
void find_motor_direction(void);
void read_potentiometer(ADC_Mode mode);
uint32 time_calc_ms(void);

// globals
uint8 emergency = FALSE;
MOTOR_STATE motor_state = FORWARD;
uint16 PWM = 50;
uint16 object_count = 0;
uint32 speed = 0;

uint8 PWM_changed = TRUE;
uint8 object_count_changed = TRUE;
uint8 speed_changed = TRUE;
uint8 direction_changed = TRUE;

uint8 display_refresh_rate = 100;

char buffer[10];

void main (void) {
    init_SysTick();
    init_RCC();
    LCD_Init();
    display_message("Program Starting...", 0, 0);

    init_GPIO();
    init_TIM();
    init_EXTI();
    ADC_Mode adc_mode = ADC_MODE_INTERRUPT;
    init_ADC(adc_mode);

    for (volatile long i = 0; i < 1000000; i++);

    LCD_Clear();
    display_message("SPD:", 0, 0);
    display_message("rpm", 0, 10);
    display_message("CNT:", 1, 0);
    display_message("PWM:", 1, 8);
    display_message("%", 1, 15);

    uint32 last_display_time = millis();

    // control_motor();
    while (1) {
        if (emergency) {
            display_message("EMERGENCY STOP", 0, 0);
        } else {
            measure_speed();
            read_potentiometer(adc_mode);

            EXTI_Disable(2);
            find_motor_direction();
            EXTI_Enable(2);

            if (motor_state == FORWARD) poll_for_object();
            if (millis() > last_display_time + display_refresh_rate) {
                display_information();
                last_display_time = millis();
            }
        }
    }
}

// initializers
void init_RCC(void) {
    Rcc_Init();

    uint8 used_peripherals[] = {RCC_GPIOA, RCC_GPIOB, RCC_TIM2, RCC_SYSCFG, RCC_TIM3, RCC_ADC1};
    for (int i = 0; i < sizeof(used_peripherals); i++) {
        Rcc_Enable(used_peripherals[i]);
    }
}

void init_GPIO(void) {
    Gpio_Init(EMERGENCY_BUTTON, GPIO_INPUT, GPIO_PULL_UP);
    Gpio_Init(RESET_BUTTON, GPIO_INPUT, GPIO_PULL_UP);
    Gpio_Init(OBJ_DETECTION_PIN, GPIO_INPUT, GPIO_PULL_UP);
    Gpio_Init(MOTOR_DIRECTION_FORWARD_PIN, GPIO_INPUT, GPIO_PULL_UP);
    Gpio_Init(MOTOR_DIRECTION_REVERSE_PIN, GPIO_INPUT, GPIO_PULL_UP);

    Gpio_Init(MOTOR_FORWARD_PIN, GPIO_OUTPUT, GPIO_PUSH_PULL);
    Gpio_Init(MOTOR_REVERSE_PIN, GPIO_OUTPUT, GPIO_PUSH_PULL);

    Gpio_Init(POT_SPEED_CONTROL_PIN, GPIO_ANALOG,GPIO_NO_PULL_DOWN);

    Gpio_Init(SPEED_MEASUREMENT_PIN, GPIO_AF, GPIO_PUSH_PULL);
    Gpio_Set_AF(SPEED_MEASUREMENT_PIN, AF_TIM_1_2);

    Gpio_Init(PWM_FORWARD_PIN, GPIO_AF, GPIO_PUSH_PULL);
    Gpio_Set_AF(PWM_FORWARD_PIN, AF_TIM_3_5);

    Gpio_Init(PWM_REVERSE_PIN, GPIO_AF, GPIO_PUSH_PULL);
    Gpio_Set_AF(PWM_REVERSE_PIN, AF_TIM_3_5);
}

void init_TIM(void) {
    // add prescaler and PWM frequency to the parameters???
    TIMER_Init(IC_TIMER, INPUT_CAPTURE, CHANNEL_1, IC_RISING);
    TIMER_Init(PWM_TIMER, PWM_OUTPUT, CHANNEL_1, IC_RISING);
    TIMER_Init(PWM_TIMER, PWM_OUTPUT, CHANNEL_2, IC_RISING);
}

void init_ADC(uint8 mode) {
    ADC_Init(1, RES_12_BIT,PCLK_4, mode);
    ADC_Start_Conversion();
}

void init_EXTI(void) {
    EXTI_Init(EMERGENCY_BUTTON, FALLING_EDGE_TRIGGERED);
    EXTI_Enable(2);

    EXTI_Init(RESET_BUTTON, FALLING_EDGE_TRIGGERED);
    EXTI_Enable(3);
}


// program logic
void measure_speed(void) {
    uint32_t pulse_width_ms = time_calc_ms();
    if (pulse_width_ms != 0) {
        uint32_t new_speed = 60000UL / pulse_width_ms; // to be in min *60 , as it i m so *1000  
        if (new_speed != speed) {
            update_speed(new_speed);
        }
    }
}

void control_motor(void) {
    switch (motor_state) {
        case FORWARD:
            Timer_Set_PWM_Duty(PWM_TIMER, REVERSE_CHANNEL, 0);
            Gpio_WritePin(MOTOR_REVERSE_PIN, LOW);

            for (volatile int i = 0; i < 100000; i++);

            Timer_Set_PWM_Duty(PWM_TIMER, FORWARD_CHANNEL, PWM);
            Gpio_WritePin(MOTOR_FORWARD_PIN, HIGH);
            break;
        case REVERSE:
            Timer_Set_PWM_Duty(PWM_TIMER, FORWARD_CHANNEL, 0);
            Gpio_WritePin(MOTOR_FORWARD_PIN, LOW);

            for (volatile int i = 0; i < 100000; i++);

            Timer_Set_PWM_Duty(PWM_TIMER, REVERSE_CHANNEL, PWM);
            Gpio_WritePin(MOTOR_REVERSE_PIN, HIGH);
            break;
        case STOP:
            Timer_Set_PWM_Duty(PWM_TIMER, FORWARD_CHANNEL, 0);
            Timer_Set_PWM_Duty(PWM_TIMER, REVERSE_CHANNEL, 0);
            Gpio_WritePin(MOTOR_FORWARD_PIN, HIGH);
            Gpio_WritePin(MOTOR_REVERSE_PIN, HIGH);
    }
}

void poll_for_object(void) {
    static uint8 last_pin_state = HIGH;
    uint8 current_pin_state = Gpio_ReadPin(OBJ_DETECTION_PIN);
    if (last_pin_state == HIGH && current_pin_state == LOW) {
        update_count();
        delay_ms(30);
    }
    last_pin_state = current_pin_state;
}


// interrupts
void EXTI2_IRQHandler(void) {
        emergency = TRUE;
        motor_state = STOP;
        control_motor();
        EXTI_ClearPending(2);
}

void EXTI3_IRQHandler(void) {
        EXTI_ClearPending(3);
        SCB_Reset();
}


// helpers
void update_speed(uint16 new_speed) {
    speed = new_speed;
    speed_changed = TRUE;
}

void update_direction(void) {
    direction_changed = TRUE;
    control_motor();
}

void update_count(void) {
    object_count++;
    object_count_changed = TRUE;
}

void update_PWM(uint16 new_PWM) {
    PWM = new_PWM;
    control_motor();
    PWM_changed = TRUE;
}

void read_potentiometer(ADC_Mode mode) {
    if (adc_done) {
        adc_done = 0;
        uint16 adc_value = adc_modes[mode]();
        uint16 new_PWM = ADC_Duty_Cycle(adc_value, 12, 100);
        if (PWM != new_PWM) {
            update_PWM(new_PWM);
        }
        PWM = ADC_Duty_Cycle(adc_value,12,100);
        ADC_Start_Conversion();
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

void find_motor_direction(void) {
    if (Gpio_ReadPin(MOTOR_DIRECTION_FORWARD_PIN) == LOW) {
        if (motor_state == REVERSE) {
            motor_state = FORWARD;
            update_direction();
        }
    }
    else if (Gpio_ReadPin(MOTOR_DIRECTION_REVERSE_PIN) == LOW) {
        if (motor_state == FORWARD) {
            motor_state = REVERSE;
            update_direction();
        }
    }
}

uint32 time_calc_ms(void) {
    static uint16 firstEdge = 0;
    static uint8 firstCaptured = 0;
    static uint32 pulseWidth = 0;

    uint16 captured = ReadCapturedValue(IC_TIMER, CHANNEL_1);
    if (!firstCaptured) {
        firstEdge = captured;
        firstCaptured = 1;
        return 0;
    } else {
        uint16 secondEdge = captured;

        if (Timer_HasOverflow(IC_TIMER)) {
            pulseWidth = (0xFFFFffff - firstEdge) + secondEdge + 1;
            Timer_ClearOverflow(IC_TIMER);
        } else
            pulseWidth = secondEdge - firstEdge;

        firstCaptured = 0;
        return pulseWidth;
    }
}

void display_information(void) {
    // display speed (in rpm) up to 3 figures???
    if (speed_changed) {
        int_to_str(speed, buffer);
        display_message("      ", 0, 4);
        display_message(buffer, 0, 4);
        speed_changed = FALSE;
    }

    if (direction_changed) {
        char direction_buffer[1];
        if (motor_state == FORWARD)
            direction_buffer[0] = R_ARROW;
        else
            direction_buffer[0] = L_ARROW;
        display_message(direction_buffer, 0, 15);
        direction_changed = FALSE;
    }

    if (object_count_changed) {
        int_to_str(object_count, buffer);
        display_message(buffer, 1, 4);
        object_count_changed = FALSE;
    }

    if (PWM_changed) {
        int_to_str(PWM, buffer);
        display_message("   ", 1, 12);
        display_message(buffer, 1, 12);
        PWM_changed = FALSE;
    }
}
