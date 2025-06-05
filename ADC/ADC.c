#include  "ADC.h"
#include "Gpio.h"
// varriables for interrupt mode
volatile uint16 adc_result = 0;
volatile uint8 adc_done = 0;

ADC_ReadFunc adc_modes[ADC_MODE_MAX] = {
   ADC_Read_Polling,
   ADC_Read_Interrupt
};

void ADC_Init(uint8 channel, uint32 resolution, uint32 prescaler, ADC_Mode mode) {
   // ENABLE THE ADC via RCC
   RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
   // ADC FEATURES :
   // resolution by defalut is 12 bit // takes 15 cycles for conversion
   ADC_per->ADC_CR[0] &= ~(0x03 << 24);
   ADC_per->ADC_CR[0] |= (resolution << 24);
   // sigle channel
   ADC_per->ADC_CR[0] &= ~(0x01<<8); // clear it to be single channel // scan
   // data alignment
   ADC_per->ADC_CR[1] &= ~(0x01<<11); // CLEAR IT TO BE RIGHT //align // we can make it config
   // select the channel
   if (channel <=9) {
      ADC_per->ADC_SMPR[0] |= (0x07 << (3*channel)); // max smaple cycle good for high impedence to make it fully charge the cap in hold circut
   }
   else {
      ADC_per->ADC_SMPR[1] |= (0x07 << (3*(channel-10)));
   }
   // conversion , we have signle channel :
   ADC_per->ADC_SQR[0] &= ~(0xF <<(20)); // this is 1 conversions "we use only pot"
   ADC_per->ADC_SQR[2] &= ~(0x1F << 0);   // Clear bits for SQ1 for channel
   ADC_per->ADC_SQR[2] |= (channel & 0x1F) << 0;

   // set the prescaler : // for  effective power consumption
   ADC_CCR&= ~(0x3 << 16);
   ADC_CCR |= (prescaler << 16);

   // disable external trigger
   ADC_per->ADC_CR[1] &= ~(1 << 20);
   ADC_per->ADC_CR[1] &= ~(0x3<<28);

  // enable EOC interrupt
   if (mode == ADC_MODE_INTERRUPT) {
      ADC_per->ADC_CR[0] |= (1 << 5);     // EOC interrupt enable
      NVIC_EnableIRQ(ADC_IRQn);          // this used for interrupt method to enable from pov NVIC
      NVIC_SetPriority(ADC_IRQn, 1);
      // ADC_per->ADC_CR[1] |= (1 << 1); // CONTINOUS MODE : i think not suitable for power consuming
   }


   ADC_per->ADC_CR[1] |= (1 << 0); // enable adc
   for (int i = 0; i < 1000; i++); // small delay after init to warm up


}

uint16 ADC_Read_Polling(void) { // this is polling read version
   ADC_per->ADC_CR[1] |= (0x01 <<30); // start conversion
   while (!(ADC_per->ADC_SR & (0x1<<1) )) ; //  wait till it became 1 so EOC
   return ADC_per->ADC_DR; // this automatically clear the EOC also
   // we read from data register

}
void ADC_IRQHandler(void) {
   if (ADC_per->ADC_SR & (1 << 1)) { // chec EOC flag
      adc_result = ADC_per->ADC_DR;
      adc_done = 1;
   }

}
uint16 ADC_Read_Interrupt(void) {
   return adc_result;
}


void ADC_Start_Conversion(void) {
   ADC_per->ADC_CR[1] |= (1 << 30); // Start conversion
}
uint16 ADC_Duty_Cycle(uint16 adc_output, uint8 resolution, uint16 pwm_top) {
   uint32 max_adc = (1<<resolution)-1; // 2^ res -1
   return (adc_output * pwm_top) / max_adc;
}
