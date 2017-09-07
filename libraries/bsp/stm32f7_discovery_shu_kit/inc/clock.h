/*
 * clock.h
 *
 * some utility functions for clock configuration (both for peripherals and for
 * the system clock)
 *
 * author:  Dr. Alex Shenfield
 * date:    01/09/2017
 */
 
// define a symbol to prevent recursive inclusion
#ifndef __SHU_CLOCK
#define __SHU_CLOCK

// include the basic headers for the hal drivers and pin mappings for the 
// stm32f7 discovery board
#include "stm32f7xx_hal.h"
#include "pinmappings.h"

// expose the functions of this library

// configure the stm32f7 discovery board to operate at 216MHz
void init_sysclk_216MHz(void);

// enable the gpio clock on a specific pin
void enable_gpio_clock(gpio_pin_t pin);

#endif
// __SHU_CLOCK
