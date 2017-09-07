/*
 * gpio.h
 *
 * this is a simplified wrapper around the gpio configuration functions 
 * provided for the embedded computer networks module at Sheffield Hallam
 * University.
 *
 * author:  Dr. Alex Shenfield
 * date:    01/09/2017
 */
 
// define a symbol to prevent recursive inclusion
#ifndef __SHU_GPIO
#define __SHU_GPIO

// include the basic headers for the hal drivers, pin mappings for the stm32f7 
// discovery board and some utility clock functions
#include "stm32f7xx_hal.h"
#include "pinmappings.h"
#include "clock.h"

// define some gpio constants
#define INPUT   1
#define OUTPUT  0
#define HIGH    1
#define LOW     0

// expose the functions of this gpio library

// initialise the gpio (this is called once for every gpio pin we want to use)
void      init_gpio(gpio_pin_t pin, int direction);

// gpio functionality
void      write_gpio(gpio_pin_t pin, int value);
void      toggle_gpio(gpio_pin_t pin);

uint16_t  read_gpio(gpio_pin_t pin);

#endif
// __SHU_GPIO
