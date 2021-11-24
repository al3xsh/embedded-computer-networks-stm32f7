/*
 * main.h
 *
 * header file for our wifi enabled mqtt example
 *
 * author:    Dr. Alex Shenfield
 * date:      17/11/2021
 * purpose:   55-604481 embedded computer networks - lab 104
 */

// define to prevent recursive inclusion
#ifndef __MAIN_H
#define __MAIN_H

// includes
#include "stm32f7xx_hal.h"
#include "RTE_Components.h"
#include "cmsis_os2.h"

// main thread initialisation function
extern void app_initialize(void);

#endif

