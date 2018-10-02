/*
 * main.c
 *
 * this is the main analog blink application (this code is dependent on the
 * extra shu libraries such as the pinmappings list, the clock configuration
 * library, the gpio library, and the adc library)
 *
 * author:    Dr. Alex Shenfield
 * date:      01/09/2018
 * purpose:   55-604481 embedded computer networks : lab 101
 */

// include the hal drivers
#include "stm32f7xx_hal.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "adc.h"
#include "gpio.h"

// include the serial configuration files
#include "serial.h"

// map the led to GPIO PA8 and the potentiometer to PA0
gpio_pin_t led = {PI_1, GPIOI, GPIO_PIN_1};
gpio_pin_t pot = {PF_8, GPIOF, GPIO_PIN_8};

// declare our utility functions
void configure_gpio(void);

// this is the main method
int main()
{
  // we need to initialise the hal library and set up the SystemCoreClock 
  // properly
  HAL_Init();
  init_sysclk_216MHz();
  
  // initialise the uart, adc and gpio pins
  init_uart(9600);
  init_adc(pot);
  init_gpio(led, OUTPUT);
  
  // print an initial status message
  printf("we are alive!\r\n");
  
  // loop forever ...
  while(1)
  {
    // toggle the led pin
    toggle_gpio(led);
    
    // read the potentiometer and echo that value to the terminal
    uint16_t adc_val = read_adc(pot);
    printf("potentiometer value = %d : delay time = %d ms\r\n", 
      adc_val, (adc_val/2));
    
    // delay for the appropriate time
    HAL_Delay(adc_val / 2);
  }
}
