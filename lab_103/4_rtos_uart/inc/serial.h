/*
 * serial.h
 *
 * provide some implementation functions for our uart (uart 1 in this case).  
 * in this case we are not implementing interrupts at all - just using the 
 * uart to output status messages.
 *
 * the stm32f7 board allows us to send messages over the virtual com port 
 * (i.e. the usb st-link connection).
 *
 * this version of serial.c is both rtos aware and interrupt aware. it reads 
 * data from the uart using interrupts and sends them to a separate thread via
 * a message queue.
 * 
 * author:    Alex Shenfield
 * date:      06/09/2017
 * purpose:   55-604481 embedded computer networks : lab 103
 */

// include the basic headers for the hal drivers
#include "stm32f7xx_hal.h"

// include the standard c io library
#include <stdio.h>

// define the virtual com port gpio pins
#define VCP_RX_Pin        GPIO_PIN_7
#define VCP_RX_GPIO_Port  GPIOB
#define VCP_TX_Pin        GPIO_PIN_9
#define VCP_TX_GPIO_Port  GPIOA

// declare the serial initialisation method
void init_uart(uint32_t baud_rate);
int serial_write(int ch);

// enable the uart rx interrupt
void enable_rx_interrupt(void);
