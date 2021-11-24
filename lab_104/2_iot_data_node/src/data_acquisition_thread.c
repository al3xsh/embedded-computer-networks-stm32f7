/*
 * data_acquisition_thread.c
 *
 * this is a thread that periodically acquires some data and puts it in a
 * mail queue
 *
 * author:    Dr. Alex Shenfield
 * date:      22/11/2021
 * purpose:   55-604481 embedded computer networks - lab 104
 */

// include the c standard io library
#include <stdio.h>

// include cmsis_os for the rtos api
#include "cmsis_os2.h"

// include my rtos objects
#include "rtos_objects.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "gpio.h"
#include "adc.h"

// HARDWARE DEFINES

// led is on PI 1 (this is the inbuilt led)
gpio_pin_t led1 = {PI_1, GPIOI, GPIO_PIN_1};

// sensors
gpio_pin_t pot_1 = {PA_0,  GPIOA, GPIO_PIN_0};
gpio_pin_t pot_2 = {PF_10, GPIOF, GPIO_PIN_10};

// ACTUAL THREAD

// data acquisition thread - read the adcs and stuff the data in a message
// queue
void data_acquisition(void const *argument)
{
  // print a status message
  printf("\rdata acquisition thread up and running ...\r\n");

  // note - generally this (using a shared resource from multiple threads) is a
  // really bad idea due to race conditions etc. - however, we can (probably)
  // get away with it here as we are only doing it once

  // set up the gpio for the led and initialise the random number generator
  init_gpio(led1, OUTPUT);

  // set up the adc for the temperature and light sensor
  init_adc(pot_1);
  init_adc(pot_2);

  // set up our counter and initialise a message container
  uint32_t i = 0;
  message_t msg;

  // infinite loop generating our fake data (one set of samples per second)
  // we also toggle the led so we can see what is going on ...
  while(1)
  {
    // read the sensors
    uint16_t adc_val_1 = read_adc(pot_1);
    uint16_t adc_val_2 = read_adc(pot_2);

    // toggle led
    toggle_gpio(led1);

    // put our data in the message object
    i++;
    msg.counter     = i;
    msg.adc_1       = adc_val_1;
    msg.adc_2				= adc_val_2;

    // put the data in the message queue and wait for ten seconds
    osMessageQueuePut(m_messages, &msg, osPriorityNormal, osWaitForever);
    osDelay(10000);
  }
}

