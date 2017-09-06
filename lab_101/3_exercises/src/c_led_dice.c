/*
 * c_traffic_lights.c
 *
 * this is the main application skeleton for the student exercises in task 3
 *
 */

// include the hal drivers
#include "stm32f7xx_hal.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "gpio.h"

// map the led to GPIO PA8
gpio_pin_t led = {PI_1, GPIOI, GPIO_PIN_1};

// this is the main method
int main()
{
	// we need to initialise the hal library and set up the SystemCoreClock 
	// properly
	HAL_Init();
	init_sysclk_216MHz();
	
	// initialise the gpio pin
	init_gpio(led, OUTPUT);
	
	// loop forever ...
	while(1)
	{
		// toggle the led on the gpio pin
		toggle_gpio(led);
		
		// wait for 1 second
		HAL_Delay(1000);
	}
}
