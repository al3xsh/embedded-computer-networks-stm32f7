/*
 * a_traffic_lights_solution.c
 *
 * this is my simple solution to the traffic lights exercise
 *
 */

// include the hal drivers
#include "stm32f7xx_hal.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "gpio.h"

// map the led to gpio pins
gpio_pin_t led1 = {PA_8,  GPIOA, GPIO_PIN_8};
gpio_pin_t led2 = {PB_15, GPIOB, GPIO_PIN_15};
gpio_pin_t led3 = {PB_14, GPIOB, GPIO_PIN_14};

// this is the main method
int main()
{
  // we need to initialise the hal library and set up the SystemCoreClock 
  // properly
  HAL_Init();
  init_sysclk_216MHz();
	
  // initialise the gpio pin
  init_gpio(led1, OUTPUT);
  init_gpio(led2, OUTPUT);
  init_gpio(led3, OUTPUT);
	
  // loop forever ...
  while(1)
  {
		// state 1 - red
		write_gpio(led1, HIGH);
		write_gpio(led2, LOW);
		write_gpio(led3, LOW);
		HAL_Delay(2000);
		
		// state 2 - red + amber
		write_gpio(led1, HIGH);
		write_gpio(led2, HIGH);
		write_gpio(led3, LOW);
		HAL_Delay(500);
		
		// state 3 - green
		write_gpio(led1, LOW);
		write_gpio(led2, LOW);
		write_gpio(led3, HIGH);
		HAL_Delay(2000);
		
		// state 4 - amber
		write_gpio(led1, LOW);
		write_gpio(led2, HIGH);
		write_gpio(led3, LOW);
		HAL_Delay(500);
  }

}
