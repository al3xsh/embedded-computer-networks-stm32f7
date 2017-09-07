/*
 * random_numbers.h
 *
 * some utility functions for the management of random numbers
 *
 * author:  alex shenfield
 * date:    01/09/2017
 */
 
// define a symbol to prevent recursive inclusion
#ifndef __SHU_RND
#define __SHU_RND

// include the math library
#include <math.h>

// include the basic headers for the hal drivers and pin mappings for the 
// stm32f7 discovery board
#include "stm32f7xx_hal.h"
#include "pinmappings.h"

// expose the functions of this library

// initialise the random number generator
void init_random(void);
  
// get a random integer
uint32_t get_random_int(void);

// get a random float (between 0 and 1)
float get_random_float(void);

#endif 
// __SHU_RND
