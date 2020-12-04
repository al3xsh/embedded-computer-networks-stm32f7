/**
 * entropy_hardware_poll.c
 *
 * override the software entropy generation (which only provides a weak entropy source)
 * with the built in hardware RNG on the discovery board (which provides a much stronger
 * source of entropy).
 *
 * this is heavily based on the example from https://www2.keil.com/iot/32f746gdiscovery
 * (just with more commenting so I actually understand what's going on!)
 *
 * author:    Dr. Alex Shenfield
 * date:      17/11/2020
 * purpose:   55-604481 embedded computer networks - lab 104
 */

// includes
#include <string.h>
#include "stm32f7xx_hal.h"

// include the random number generator functionality
#include "random_numbers.h"

// we define "MBEDTLS_ENTROPY_HARDWARE_ALT" in the mbedTLS_config.h file and then use the
// STM32F7 HAL hardware random number generator to provide an entropy source for the
// TLS function

// this is our function prototype
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen);

// override the entropy poll callback in entropy_poll_hw.c to use a hardware source
// e.g. the hardware RNG built in to the stm32f7-discovery board (this is a stronger
// source of entropy than the weak entropy source defined in software)
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
  HAL_StatusTypeDef status = HAL_OK;
  uint32_t random_number = 0;

  // generate our random number
  status = HAL_RNG_GenerateRandomNumber(&rng_handle, &random_number);
  ((void) data);
  *olen = 0;

  // if the rng hasn't returned properly then just return 0
  if((len < sizeof(uint32_t)) || (HAL_OK != status))
  {
    return 0;
  }

  // otherwise, copy the random number into the output to use as part of the entropy
  // source and return
  memcpy(output, &random_number, sizeof(uint32_t));
  *olen = sizeof(uint32_t);

  return 0;
}

