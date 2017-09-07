/*
 * random_numbers.c
 *
 * some utility functions for the management of random numbers
 *
 * author:  Dr. Alex Shenfield
 * date:    01/09/2017
 */
 
// include the shu library bsp random_number header
#include "random_numbers.h"

// initialise the random number generator
void init_random(void)
{
  __HAL_RCC_RNG_CLK_ENABLE();
  RNG->CR |= RNG_CR_RNGEN;
}
  
// get a random integer
uint32_t get_random_int(void)
{
  while (!(RNG->SR & (RNG_SR_DRDY)));
  return RNG->DR;
}

// get a random float (between 0 and 1)
float get_random_float(void)
{
  while (!(RNG->SR & (RNG_SR_DRDY)));
  return (RNG->DR / powf(2.0, 32.0));
}
