/*
 * a_traffic_lights.c
 *
 * this is my solution to the first part of task 3 - a simple set of traffic 
 * lights using a state-machine type pattern
 *
 * author: alex shenfield
 * date:   01/10/2017
 */

// include the hal drivers
#include "stm32f7xx_hal.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "gpio.h"

// map the lights:
// green led to GPIO PI1
// amber led to GPIO PB14
// red led to GPIO PB15
gpio_pin_t green_led = {PI_1, GPIOI, GPIO_PIN_1};
gpio_pin_t amber_led = {PB_14, GPIOB, GPIO_PIN_14};
gpio_pin_t red_led   = {PB_15, GPIOB, GPIO_PIN_15};

// this is the main method
int main()
{
  // we need to initialise the hal library and set up the SystemCoreClock 
  // properly
  HAL_Init();
  init_sysclk_216MHz();
  
  // initialise the gpio pins
  init_gpio(green_led, OUTPUT);
  init_gpio(amber_led, OUTPUT);
  init_gpio(red_led, OUTPUT);
  
  // loop forever ...
  while(1)
  {
    // state 1: go
		write_gpio(green_led, HIGH);
		write_gpio(amber_led, LOW);
		write_gpio(red_led, LOW);	
    
    // wait for 1 second
    HAL_Delay(1000);
		
    // state 2: stop if safe
		write_gpio(green_led, LOW);
		write_gpio(amber_led, HIGH);
		write_gpio(red_led, LOW);	
    
    // wait for 1 second
    HAL_Delay(1000);
		
    // state 3: stop
		write_gpio(green_led, LOW);
		write_gpio(amber_led, LOW);
		write_gpio(red_led, HIGH);	
    
    // wait for 1 second
    HAL_Delay(1000);
		
    // state 4: prepare to go
		write_gpio(green_led, LOW);
		write_gpio(amber_led, HIGH);
		write_gpio(red_led, HIGH);	
    
    // wait for 0.5 second
    HAL_Delay(500);
  }
}
