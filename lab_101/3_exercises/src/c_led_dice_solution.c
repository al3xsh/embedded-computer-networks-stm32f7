/*
 * c_led_dice_solution.c
 *
 * this is the main led dice application
 *
 */

// include the hal drivers
#include "stm32f7xx_hal.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "gpio.h"

#include "random_numbers.h"

// map the leds to appropriate pins
gpio_pin_t led1 = {PI_1, GPIOI, GPIO_PIN_1};
gpio_pin_t led2 = {PI_1, GPIOI, GPIO_PIN_1};
gpio_pin_t led3 = {PI_1, GPIOI, GPIO_PIN_1};
gpio_pin_t led4 = {PI_1, GPIOI, GPIO_PIN_1};
gpio_pin_t led5 = {PI_1, GPIOI, GPIO_PIN_1};
gpio_pin_t led6 = {PI_1, GPIOI, GPIO_PIN_1};
gpio_pin_t led7 = {PI_1, GPIOI, GPIO_PIN_1};

// let's trigger the dice roll with a button (this time we'll use the user
// button on the board)
gpio_pin_t pb1 = {PI_11, GPIOI, GPIO_PIN_11};


// this is the main method
int main()
{
  // we need to initialise the hal library and set up the SystemCoreClock 
  // properly
  HAL_Init();
  init_sysclk_216MHz();
  
  // initialise the gpio pins for output
  init_gpio(led1, OUTPUT);
  init_gpio(led2, OUTPUT);
  init_gpio(led3, OUTPUT);
  init_gpio(led4, OUTPUT);
  init_gpio(led5, OUTPUT);
  init_gpio(led6, OUTPUT);
  init_gpio(led7, OUTPUT);
  
	// initialise a button for the dice roll trigger
	init_gpio(pb1, INPUT);
	
	// initialise the random number generator
	init_random();
	
  // loop forever ...
  while(1)
  {
		// throw a random number between 1 and 6 ...
		uint32_t roll = (get_random_int() % 6) + 1;
		
		// light up the appropriate leds
		if (roll == 1)
		{
			write_gpio(led7, HIGH);
		}
		else if (roll == 2)
		{
			write_gpio(led3, HIGH);
			write_gpio(led5, HIGH);
		}
		else if (roll == 3)
		{
			write_gpio(led1, HIGH);
			write_gpio(led7, HIGH);
			write_gpio(led6, HIGH);
		}
		else if (roll == 4)
		{
			write_gpio(led1, HIGH);
			write_gpio(led3, HIGH);
			write_gpio(led4, HIGH);
			write_gpio(led6, HIGH);
		}
		else if (roll == 5)
		{
			write_gpio(led1, HIGH);
			write_gpio(led3, HIGH);
			write_gpio(led4, HIGH);
			write_gpio(led6, HIGH);
			write_gpio(led7, HIGH);
		}
		else if (roll == 6)
		{
			write_gpio(led1, HIGH);
			write_gpio(led2, HIGH);
			write_gpio(led3, HIGH);
			write_gpio(led4, HIGH);
			write_gpio(led5, HIGH);
			write_gpio(led6, HIGH);
		}
		
    // wait for 1 second
    HAL_Delay(1000);
	 	
		// clear all leds
		write_gpio(led1, LOW);
		write_gpio(led2, LOW);
		write_gpio(led3, LOW);
		write_gpio(led4, LOW);
		write_gpio(led5, LOW);
		write_gpio(led6, LOW);
		write_gpio(led7, LOW);
  }
}
