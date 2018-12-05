/*
 * main.c
 *
 * this is the main rtos xbee / uart based application
 *
 * author:		Alex Shenfield
 * date:			08/11/2018
 * purpose:   55-604481 embedded computer networks : lab 104
 */

// include the basic headers for the hal drivers and the rtos library
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "gpio.h"

// include the xbee tx and rx functionality
#include "xbee.h"

// include the itm debugging
#include "itm_debug.h"

// RTOS

// declare the extern methods that set everything up for us
extern int init_xbee_threads(void);

// OVERRIDE HAL DELAY

// make HAL_Delay point to osDelay (otherwise any use of HAL_Delay breaks things)
void HAL_Delay(__IO uint32_t Delay)
{
	osDelay(Delay);
}

// XBEE

// xbee configuration packets

// set up adc on dio 0 on all xbees connected to the WPAN - temperature
uint8_t init_adc_0[]         = {0x7E, 0x00, 0x10, 0x17, 0x01, 0x00, 0x00, 0x00, 
                                0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFE, 0x02, 
	                              0x44, 0x30, 0x02, 0x74
                               };

// set up adc on dio 1 on all xbees connected to the WPAN - light
uint8_t init_adc_1[]         = {0x7E, 0x00, 0x10, 0x17, 0x02, 0x00, 0x00, 0x00, 
                                0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFE, 0x02, 
	                              0x44, 0x31, 0x02, 0x72
                               };

// configure all radios in the WPAN to do automatic sampling every 10 seconds
uint8_t configure_sampling[] = {0x7E, 0x00, 0x11, 0x17, 0x01, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFE, 0x02, 
	                              0x49, 0x52, 0x27, 0x10, 0x18
                               };

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

	// set up the xbee uart at 9600 baud and enable the rx interrupts
	init_xbee(9600);
	enable_rx_interrupt();

	// print debugging message
	osDelay(50);
	print_debug("initialising xbee thread", 24);

	// initialise our threads
	init_xbee_threads();

	// wait for the coordinator xbee to settle down, and then send the
	// configuration packets
	print_debug("sending configuration packets", 29);
	osDelay(1000);

	// initialise the adc on pin dio0
	send_xbee(init_adc_0, 20);
	osDelay(1000);

	// initialise the adc on pin dio1
	send_xbee(init_adc_1, 20);
	osDelay(1000);

	// set up the periodic sampling of the xbee pins (every 10s)
	send_xbee(configure_sampling, 21);
	print_debug("... done!", 9);

	// start everything running
	osKernelStart();
}
