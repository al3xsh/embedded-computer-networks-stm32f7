/*
 * random_numbers.c
 *
 * some utility functions for the management of random numbers - now updated
 * to allow access to the radndom number generator handle (needed for 
 * hardware entropy generation in mbedTLS).
 *
 * author:  Dr. Alex Shenfield
 * date:    30/11/2020
 */
 
// include the shu library bsp random_number header
#include "random_numbers.h"

// initialise the handle to the random number generator
RNG_HandleTypeDef rng_handle;

// initialise the random number generator
void init_random(void)
{
	// enable the rng clock
	__HAL_RCC_RNG_CLK_ENABLE();
	
	// set the rng instance and initialise the peripheral
	rng_handle.Instance = RNG;
  HAL_RNG_Init(&rng_handle);
}
  
// get a random integer
uint32_t get_random_int(void)
{
	uint32_t random_number = 0;
  HAL_RNG_GenerateRandomNumber(&rng_handle, &random_number);
  return RNG->DR;
}

// get a random float (between 0 and 1)
float get_random_float(void)
{
  return (get_random_int() / powf(2.0, 32.0));
}
