/*
 * main.c
 *
 * this is an example of button debouncing in an rtos
 *
 * author:		Alex Shenfield
 * date:			04/02/2019
 * purpose:   55-604481 embedded computer networks
 */

// include the basic headers for the hal drivers and the rtos library
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "gpio.h"

// include the itm debugging
#include "itm_debug.h"

// BUTTON

// define the button
gpio_pin_t pb1 = {PB_8, GPIOB, GPIO_PIN_8};

// declare a timer callback and a timer
void test_for_button_press(void const *arg);
osTimerDef(button, test_for_button_press);

// lets use an led as a message indicator
gpio_pin_t led = {PI_3, GPIOI, GPIO_PIN_3};

// OVERRIDE HAL DELAY

// make HAL_Delay point to osDelay (otherwise any use of HAL_Delay breaks things)
void HAL_Delay(__IO uint32_t Delay)
{
  osDelay(Delay);
}

// CODE	

// this is the main method
int main()
{	
	// initialise the real time kernel
	osKernelInitialize();
	
	// we need to initialise the hal library and set up the SystemCoreClock 
	// properly
	HAL_Init();
	init_sysclk_216MHz();
	
	// note also that we need to set the correct core clock in the rtx_conf_cm.c
	// file (OS_CLOCK) which we can do using the configuration wizard
	
	// initialise our button 
	init_gpio(pb1, INPUT);
	init_gpio(led, OUTPUT);
	
	// start our timer for button debouncing
	osTimerId timer_1 = osTimerCreate(osTimer(button), osTimerPeriodic, NULL);
	osTimerStart(timer_1, 5);
	
	// start everything running
	osKernelStart();
}

// BUTTON

// button debouncer (implemented as a timer callback)
void test_for_button_press(void const *args)
{ 			
	// 8 bits of button history
	static uint8_t button_history = 0xFF;

	// every time this timer callback is called we shift the button history 
	// across and update the state
	button_history = button_history << 1;
	uint8_t val = read_gpio(pb1);
	button_history = button_history | val; 
	
	// use some simple pattern matching to see if the button has been pressed -
	// if so, reset the button history and send a message ...
	if((button_history & 0xC7) == 0x07)
	{ 
		// toggle the led to indicate whats going on
		toggle_gpio(led);
				
		// reset button history
		button_history = 0xFF;		
		print_debug("button pressed", 14);
	}		
}
