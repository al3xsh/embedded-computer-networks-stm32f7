/*
 * adc.h
 *
 * this is the simplified wrapper around the adc configuration functions 
 * provided for the embedded computer networks module at Sheffield Hallam
 * University.
 * 
 * for the adc configuration (e.g. which adc, which channel, etc.) we need to
 * look at the stm32f7 discovery user manual - en.DM00166116.pdf
 *
 * on the stm32f746g discovery board there are only 6 analog pins broken out
 * (corresponding to those on the arduino header).
 *
 * author:  Dr. Alex Shenfield
 * date:    01/09/2017
 */
 
// define a symbol to prevent recursive inclusion
#ifndef __SHU_ADC
#define __SHU_ADC

// include the basic headers for the hal drivers, pin mappings for the stm32f7 
// discovery board and some utility clock functions
#include "stm32f7xx_hal.h"
#include "pinmappings.h"
#include "clock.h"

// adc pin structure
typedef struct
{
  gpio_pin_t    gpio;
  ADC_TypeDef*  adc;
  uint32_t      adc_channel;
}
adc_pin_t;

// adc pin mapping containing configuration information for all available 
// (external) ADC pins on the stm32f7 discovery board
//
// note: pa_0 can be used with adc 1 and 2 as well ...
static const adc_pin_t stm32f7_adc_pins[] = 
{
  {{PA_0, GPIOA, GPIO_PIN_0}, ADC3, ADC_CHANNEL_0},
  
  {{PF_6, GPIOF, GPIO_PIN_6}, ADC3, ADC_CHANNEL_4},
  {{PF_7, GPIOF, GPIO_PIN_7}, ADC3, ADC_CHANNEL_5},
  {{PF_8, GPIOF, GPIO_PIN_8}, ADC3, ADC_CHANNEL_6},
  {{PF_9, GPIOF, GPIO_PIN_9}, ADC3, ADC_CHANNEL_7},
  {{PF_10, GPIOF, GPIO_PIN_10}, ADC3, ADC_CHANNEL_8},
  
  // must be last element of our adc pin configuration array ...
  {{NC, 0, 0}, 0, 0}
};

// expose the functions of this adc library

// initialise the adc (this is called once for every gpio pin we want to use
// with the adc)
void init_adc(gpio_pin_t pin);

// read the adc
uint16_t read_adc(gpio_pin_t pin);

#endif
// __SHU_ADC
