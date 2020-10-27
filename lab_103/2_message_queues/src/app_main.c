/*
 * app_main.c
 *
 * this is where we create the main rtos application thread and kick everything
 * off
 *
 * author:    Dr. Alex Shenfield
 * date:      22/10/2020
 * purpose:   55-604481 embedded computer networks - lab 103
 */

// include the c standard io library
#include <stdio.h>

// include the basic headers for the hal drivers and cmsis-rtos2
#include "stm32f7xx_hal.h"
#include "cmsis_os2.h"

// include my rtos objects
#include "rtos_objects.h"

// RTOS DEFINES

// define the externed function prototypes, thread ids and attributes for each
// of our worker threads

// data generation thread
osThreadId_t gen_thread;
extern void data_generation(void *argument);
static const osThreadAttr_t data_gen_thread_attr =
{
  .name = "data_generation",
  .priority = osPriorityNormal,
};

// data display thread
osThreadId_t display_thread;
extern void data_display(void *argument);
static const osThreadAttr_t data_display_thread_attr =
{
  .name = "data_display",
  .priority = osPriorityNormal,
};

// define the message queue id and attributes

// my message queue
osMessageQueueId_t m_messages;
static const osMessageQueueAttr_t msq_q_attr =
{
  .name = "my_messages",
};

// OTHER DEFINES

// stdout_init is defined in our configuration file (as part of the ARM
// compiler user code template
extern int stdout_init(void);

// APPLICATION MAIN THREAD

// provide an increased stack size to the main application thread (to help deal
// with the uart printf redirection)
static const osThreadAttr_t app_main_attr =
{
  .stack_size = 8192U
};

// application main thread - initialise general peripherals, create the message
// queue, and start the worker threads
void app_main(void *argument)
{
  // initialise the uart
  stdout_init();

  // print a startup message
  printf("we are alive\r\n");

  // create the message queue
  m_messages = osMessageQueueNew(16, sizeof(message_t), &msq_q_attr);

  // create the threads
  gen_thread = osThreadNew(data_generation, NULL, &data_gen_thread_attr);
  display_thread = osThreadNew(data_display, NULL, &data_display_thread_attr);
}

// initialise the application (by creating the main thread)
void app_initialise(void)
{
  osThreadNew(app_main, NULL, &app_main_attr);
}
