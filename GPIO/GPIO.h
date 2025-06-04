#ifndef GPIO_H
#define GPIO_H

#include "Std_Types.h"

/*PortName*/
#define GPIO_A     'A'
#define GPIO_B     'B'
#define GPIO_C     'C'
#define GPIO_D     'D'

//PinMode
#define GPIO_INPUT  0x00
#define GPIO_OUTPUT 0x01
#define GPIO_AF     0x02
#define GPIO_ANALOG 0x03

// DefaultState
#define GPIO_PUSH_PULL  0x00
#define GPIO_OPEN_DRAIN 0x01

#define GPIO_NO_PULL_DOWN 0x00
#define GPIO_PULL_UP      0x01
#define GPIO_PULL_DOWN    0x02

// Data
#define LOW      0x0
#define HIGH     0x1

#define OK  0x0
#define NOK 0x1

// AF Codes
#define AF_SYSTEM 0x0
#define AF_TIM_1_2 0x1
#define AF_TIM_3_5 0x2
#define AF_TIM_9_11 0x3
#define AF_I2C1 0x4
#define AF_SPI_1_4 0x5
#define AF_SPI3 0x6
#define AF_USART_1_2 0x7
#define AF_USART_6 0x8
#define AF_I2C2_3 0x9
#define AF_OTG_FS 0xA
#define AF_11 0xB
#define AF_SDIO 0xC
#define AF_13 0xD
#define AF_14 0xE
#define AF_EVENT_OUT 0xF


void Gpio_Init(uint8 PortName, uint8 PinNumber, uint8 PinMode, uint8 DefaultState);

uint8 Gpio_WritePin(uint8 PortName, uint8 PinNumber, uint8 Data);

uint8 Gpio_ReadPin(uint8 PortName, uint8 PinNumber);

void Gpio_Set_AF(uint8 PortName, uint8 PinNumber, uint8 AF_Code);

#endif //GPIO_H