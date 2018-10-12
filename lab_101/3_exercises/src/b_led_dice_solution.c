/*
 * b_led_dice_solution.c
 *
 * this is my simplified version of the led dice exercise
 *
 * here i am just using three leds and generating a random integer between 
 * 0 and 2
 *
 */

// include the hal drivers
#include "stm32f7xx_hal.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "gpio.h"
#include "random_numbers.h"

// map the led to gpio pins
gpio_pin_t led1 = {PA_8,  GPIOA, GPIO_PIN_8};
gpio_pin_t led2 = {PB_14, GPIOB, GPIO_PIN_14};
gpio_pin_t led3 = {PB_15, GPIOB, GPIO_PIN_15};

// this is the main method
int main()
{
  // we need to initialise the hal library and set up the SystemCoreClock 
  // properly
  HAL_Init();
  init_sysclk_216MHz();
  
	// initialise the random number generator
	init_random();
	
  // initialise the gpio pin
  init_gpio(led1, OUTPUT);
  init_gpio(led2, OUTPUT);
  init_gpio(led3, OUTPUT);
  
	// stick all the leds in an array for ease of addressing
	gpio_pin_t leds[3] = {led1, led2, led3}; 
	
  // loop forever ...
  while(1)
  {
		// reset all the leds
		for (int i = 0; i < 3; i++)
		{
			write_gpio(leds[i], LOW);
		}
		
		// generate a random number between 0 and 2
		uint32_t rnd = get_random_int() % 3;
		
		// light up the appropriate led
		write_gpio(leds[rnd], HIGH);		
    
    // wait for 1 second
    HAL_Delay(1000);
  }
}
