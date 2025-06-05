#include "Std_Types.h"
// #include "Gpio.h"
#include "stm32f4xx.h"
#ifndef ADC_H
#define ADC_H
typedef struct {
    uint32 ADC_SR;
    uint32 ADC_CR[2]; // for 1 & 2
    uint32 ADC_SMPR[2]; //for 1 & 2
    uint32 ADC_JOFR[4]; // for 1 ,2 ,3 ,4
    uint32 ADC_HTR;
    uint32 ADC_LTR;
    uint32 ADC_SQR[3];
    uint32 ADC_JSQR;
    uint32 ADC_JDR[4];
    uint32 ADC_DR;
}ADCType;
#define ADC_per ((ADCType*)0x40012000 )
#define ADC_CCR   (*(uint32*)0x40012304) // for prescaler selection "common reg"

// modes
typedef enum {
    ADC_MODE_POLLING = 0,
    ADC_MODE_INTERRUPT,
    ADC_MODE_MAX
} ADC_Mode;

// max num of modes used
#define ADC_MODE_MAX 2

// resolutions
#define RES_12_BIT 0x00
#define RES_10_BIT 0x01
#define RES_8_BIT 0x02
#define RES_6_BIT 0x03

// prescalers
#define PCLK_2 0x00
#define PCLK_4 0x01
#define PCLK_6 0x02
#define PCLK_8 0x03

typedef uint16 (*ADC_ReadFunc)(void);
extern ADC_ReadFunc adc_modes[ADC_MODE_MAX];

void ADC_Init(uint8 channel, uint32 resolution, uint32 prescaler, ADC_Mode mode);
uint16 ADC_Read_Polling(void);
uint16 ADC_Read_Interrupt(void);
void ADC_Start_Conversion(void);
uint16 ADC_Duty_Cycle(uint16 adc_output, uint8 resolution, uint16 pwm_top) ;

extern volatile uint16 adc_result;
extern volatile uint8 adc_done;
#endif