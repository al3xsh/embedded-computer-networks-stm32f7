/*
 * main.h
 *
 * this is the header for the main rtos mail queue application
 *
 * author:    Alex Shenfield
 * date:      23/09/2016
 * purpose:   16-6498 embedded computer networks : lab 103
 */

// define to prevent recursive inclusion
#ifndef __MAIN_H
#define __MAIN_H

// include the basic headers for the hal drivers and the rtos library
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

// set up our mailbox

// mail data structure
typedef struct 
{
  uint16_t  temperature;
  uint16_t  light;
} 
mail_t;

#endif // MAIN_H
