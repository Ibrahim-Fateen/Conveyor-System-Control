#ifndef LCD_H
#define LCD_H

#include "Std_Types.h"

// LCD pin config (adjust as needed)
#define LCD_RS_PORT  GPIO_B
#define LCD_RS_PIN   0

#define LCD_EN_PORT  GPIO_B
#define LCD_EN_PIN   1

#define LCD_D4_PORT  GPIO_B
#define LCD_D4_PIN   2

#define LCD_D5_PORT  GPIO_B
#define LCD_D5_PIN   3

#define LCD_D6_PORT  GPIO_B
#define LCD_D6_PIN   4

#define LCD_D7_PORT  GPIO_B
#define LCD_D7_PIN   5

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8 row, uint8 col);
void LCD_SendString(char *str);
void LCD_SendChar(char data);

#endif
