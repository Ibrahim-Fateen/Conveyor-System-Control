#include "lcd.h"
#include "GPIO.h"
// #include "delay.h" // You must implement delay_ms(uint32)

static void LCD_EnablePulse(void);
static void LCD_Send4Bits(uint8 data);
static void LCD_SendCommand(uint8 cmd);

static void LCD_Write(uint8 port, uint8 pin, uint8 val) {
    Gpio_WritePin(port, pin, val);
}

void delay_ms(uint32 ms) {
    for (volatile uint32 i = 0; i < ms * 8000; i++); // crude loop delay
}

void LCD_Init(void) {
    // Initialize all pins as output push-pull
    Gpio_Init(LCD_RS_PORT, LCD_RS_PIN, GPIO_OUTPUT, GPIO_PUSH_PULL);
    Gpio_Init(LCD_EN_PORT, LCD_EN_PIN, GPIO_OUTPUT, GPIO_PUSH_PULL);
    Gpio_Init(LCD_D4_PORT, LCD_D4_PIN, GPIO_OUTPUT, GPIO_PUSH_PULL);
    Gpio_Init(LCD_D5_PORT, LCD_D5_PIN, GPIO_OUTPUT, GPIO_PUSH_PULL);
    Gpio_Init(LCD_D6_PORT, LCD_D6_PIN, GPIO_OUTPUT, GPIO_PUSH_PULL);
    Gpio_Init(LCD_D7_PORT, LCD_D7_PIN, GPIO_OUTPUT, GPIO_PUSH_PULL);

    delay_ms(50);

    LCD_Send4Bits(0x03);
    delay_ms(5);
    LCD_Send4Bits(0x03);
    delay_ms(5);
    LCD_Send4Bits(0x03);
    delay_ms(1);
    LCD_Send4Bits(0x02); // Set 4-bit mode

    LCD_SendCommand(0x28); // 4-bit, 2 line, 5x8 font
    LCD_SendCommand(0x0C); // Display ON, Cursor OFF
    LCD_SendCommand(0x06); // Entry mode
    LCD_SendCommand(0x01); // Clear
    delay_ms(2);
}

void LCD_Clear(void) {
    LCD_SendCommand(0x01);
    delay_ms(2);
}

void LCD_SetCursor(uint8 row, uint8 col) {
    uint8 addr = (row == 0) ? 0x80 : 0xC0;
    LCD_SendCommand(addr + col);
}

void LCD_SendString(char *str) {
    while (*str) {
        LCD_SendChar(*str++);
    }
}

void LCD_SendChar(char data) {
    LCD_Write(LCD_RS_PORT, LCD_RS_PIN, HIGH);
    LCD_Send4Bits(data >> 4);
    LCD_Send4Bits(data & 0x0F);
}

static void LCD_SendCommand(uint8 cmd) {
    LCD_Write(LCD_RS_PORT, LCD_RS_PIN, LOW);
    LCD_Send4Bits(cmd >> 4);
    LCD_Send4Bits(cmd & 0x0F);
}

static void LCD_Send4Bits(uint8 data) {
    LCD_Write(LCD_D4_PORT, LCD_D4_PIN, (data >> 0) & 0x01);
    LCD_Write(LCD_D5_PORT, LCD_D5_PIN, (data >> 1) & 0x01);
    LCD_Write(LCD_D6_PORT, LCD_D6_PIN, (data >> 2) & 0x01);
    LCD_Write(LCD_D7_PORT, LCD_D7_PIN, (data >> 3) & 0x01);
    LCD_EnablePulse();
}

static void LCD_EnablePulse(void) {
    LCD_Write(LCD_EN_PORT, LCD_EN_PIN, HIGH);
    delay_ms(1);
    LCD_Write(LCD_EN_PORT, LCD_EN_PIN, LOW);
    delay_ms(1);
}
