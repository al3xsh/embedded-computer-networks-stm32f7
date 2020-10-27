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

// include the basic headers for the hal drivers and cmsis-rtos2
#include "stm32f7xx_hal.h"
#include "cmsis_os2.h"

// include my rtos objects
#include "rtos_objects.h"

// RTOS DEFINES

// define the event flag id and attributes

// we are enabling an event flag for notification of usart events
osEventFlagsId_t usart_flag;
static const osEventFlagsAttr_t usart_flag_attr =
{
  .name = "USART_event",
};

// define the externed function prototypes, thread ids and attributes for each
// of our worker threads

// uart terminal thread
osThreadId_t usart_thread;
extern void my_usart_thread(void *argument);
static const osThreadAttr_t usart_thread_attr =
{
  .name = "usart_terminal",
  .priority = osPriorityNormal,
};

// APPLICATION MAIN THREAD

// provide an increased stack size to the main application thread (to help deal
// with the uart printf redirection)
static const osThreadAttr_t app_main_attr =
{
  .stack_size = 8192U
};

// application main thread - create the event flag for uart events and start 
// the worker thread
void app_main(void *argument)
{
  // create the usart event flag
  usart_flag = osEventFlagsNew(&usart_flag_attr);

  // create the thread
  usart_thread = osThreadNew(my_usart_thread, NULL, &usart_thread_attr);
}

// APPLICATION MAIN THREAD INITIALISATION

// initialise the application (by creating the main thread)
void app_initialise(void)
{
  osThreadNew(app_main, NULL, &app_main_attr);
}
