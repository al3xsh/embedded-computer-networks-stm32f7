/*
 * main.c
 *
 * this is the main analog temperature sensor application (this code is 
 * dependent on the extra shu libraries such as the pinmappings list, the
 * clock configuration library, the gpio library, and the adc library)
 *
 * author:    Dr. Alex Shenfield
 * date:      19/10/2017
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

// map the led to GPIO PI1 and the temperature sensor to PA_0
gpio_pin_t led   = {PI_1, GPIOI, GPIO_PIN_1};
gpio_pin_t tmp36 = {PF_6, GPIOF, GPIO_PIN_6};

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
  init_adc(tmp36);
  init_gpio(led, OUTPUT);
  
  // print an initial status message
  printf("we are alive!\r\n");
  
  // loop forever ...
  while(1)
  {
    // toggle the led pin
    toggle_gpio(led);
    
    // read the temperature sensor and echo that value to the terminal
    uint16_t adc_val = read_adc(tmp36);
    printf("analog value = %d\r\n", adc_val);
    
		// convert to actual temperature (measured vcc as 3.26 volts with a 
		// multimeter)
		double voltage = ((adc_val / 4095.0) * 3300.0); 
		double temp = ((voltage - 500.0) / 10.0);
		printf("voltage = %4.2f\r\n", voltage);
		printf("room temperature = %3.2f\r\n", temp);
		printf("\r\n");
		
    // delay for the appropriate time
    HAL_Delay(1500);
  }
}
