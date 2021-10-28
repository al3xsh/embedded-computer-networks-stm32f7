/*
 * data_generation_thread.c
 *
 * this is a thread that periodically generates some data and puts it in a
 * mail queue
 *
 * author:    Dr. Alex Shenfield
 * date:      22/10/2021
 * purpose:   55-604481 embedded computer networks - lab 103
 */

// include the c standard io library
#include <stdio.h>

// include cmsis_os for the rtos api
#include "cmsis_os2.h"

// include my rtos objects
#include "rtos_objects.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "random_numbers.h"
#include "gpio.h"

// HARDWARE DEFINES

// led is on PI 1 (this is the inbuilt led)
gpio_pin_t led1 = {PI_1, GPIOI, GPIO_PIN_1};

// ACTUAL THREAD

// data generation thread - create some random data and stuff it in a message
// queue
void data_generation(void const *argument)
{
  // print a status message
  printf("data generation thread up and running ...\r\n");

  // note - generally this (using a shared resource - i.e. the usart for printf
  // - from multiple threads) is a really bad idea due to race conditions etc. 
	// however, we can (probably) get away with it here as we are only doing it 
	// once before the data display thread is properly running.
	//
	// really we should protect access to the usart here with a mutex around the
	// printf statement(s)

  // set up the gpio for the led and initialise the random number generator
  init_gpio(led1, OUTPUT);
  init_random();

  // set up our counter and initialise a message container
  uint32_t i = 0;
  message_t msg;

  // infinite loop generating our fake data (one set of samples per second)
  // we also toggle the led so we can see what is going on ...
  while(1)
  {
    // get a random number
    float random = get_random_float();

    // toggle led
    toggle_gpio(led1);

    // generate our fake data
    i++;
    msg.counter = i;
    msg.current = (1.0f / (random * i));
    msg.voltage = (5.0f / (random * i));

    // put the data in the message queue and delay for one second (note: 
		// osWaitForever here means "wait as long as it takes until there is space
		// in the message queue)
    osMessageQueuePut(m_messages, &msg, osPriorityNormal, osWaitForever);
    osDelay(1000);
  }
}
