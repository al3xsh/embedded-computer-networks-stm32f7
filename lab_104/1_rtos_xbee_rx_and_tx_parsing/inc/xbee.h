/*
 * xbee.h
 *
 * provide some implementation functions for our xbee uart (uart 6 in this  
 * case).
 *
 * this version of xbee.c is both rtos aware and interrupt aware. it reads 
 * data from the uart using interrupts and sends them to a separate thread via
 * a message queue.
 * 
 * author:    Alex Shenfield
 * date:      08/11/2017
 * purpose:   55-604481 embedded computer networks : lab 104
 */

// define to prevent recursive inclusion
#ifndef __XBEE_H
#define __XBEE_H

// include the basic headers for the hal drivers
#include "stm32f7xx_hal.h"

// include the standard c io library
#include <stdio.h>

// define the virtual com port gpio pins
#define XBEE_RX_Pin        GPIO_PIN_6
#define XBEE_RX_GPIO_Port  GPIOC
#define XBEE_TX_Pin        GPIO_PIN_7
#define XBEE_TX_GPIO_Port  GPIOC

// declare the serial initialisation method and the data transmission method
void init_xbee(uint32_t baud_rate);
int  send_xbee(volatile uint8_t* s, int length);

// enable the uart / xbee rx interrupt
void enable_rx_interrupt(void);

#endif // XBEE_H
