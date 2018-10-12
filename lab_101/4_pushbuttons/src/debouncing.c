/*
 * main.c
 *
 * this is the main push button application (this code is dependent on the
 * extra shu libraries such as the pinmappings list and the clock configuration
 * library)
 *
 * now including latching and debouncing functionality
 *
 * author:    Alex Shenfield
 * date:      01/09/2018
 * purpose:   55-604481 embedded computer networks : lab 101
 */

// include the hal drivers
#include "stm32f7xx_hal.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "gpio.h"

// INITIALISATION

// map the led to GPIO PI_1 (the inbuilt led) and the push button to PB_8
gpio_pin_t led = {PI_1,  GPIOI, GPIO_PIN_1};
gpio_pin_t pb1 = {PB_14, GPIOB, GPIO_PIN_14};

// declare some variables for button debouncing
uint8_t button_state = 0;
uint8_t last_button_state = 0;
unsigned long last_check = 0;
unsigned long debounce_time = 50;

// declare a state variable for our output
uint8_t output_state = 0;

// CODE

// this is the main method
int main()
{
  // we need to initialise the hal library and set up the SystemCoreClock 
  // properly
  HAL_Init();
  init_sysclk_216MHz();
  
  // initialise the gpio pins
  init_gpio(led, OUTPUT);
  init_gpio(pb1, INPUT);
  
  // loop forever ...
  while(1)
  {
		// read the button
		uint8_t reading = read_gpio(pb1);
		
		// check if the button value has changed and, if so, reset the debouncing
		// timer
		if(reading != last_button_state)
		{
			last_check = HAL_GetTick();
		}
		
		// the reading from the button has been there long enough for it to be the
		// true state of the button
		if((HAL_GetTick() - last_check) > debounce_time)
		{
			// if the button state has changed snce last time we check, then we will
			// change the button state to what we've just read
			if(reading != button_state)
			{
				button_state = reading;
				
				// only toggle the led if the new button state is HIGH
				if(button_state == HIGH)
				{
					toggle_gpio(led);
				}
			}
		}
		
		// now update the last measured button state
		last_button_state = reading;
  }
}
