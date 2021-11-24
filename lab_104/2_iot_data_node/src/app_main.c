/*
 * app_main.c
 *
 * this is where we create the main application thread(s) and kick everything
 * off
 *
 * author:    Dr. Alex Shenfield
 * date:      17/11/2021
 * purpose:   55-604481 embedded computer networks - lab 104
 */

// include functions from the c standard libraries
#include <stdint.h>
#include <stdio.h>

// include cmsis-rtos version 2
#include "cmsis_os2.h"

// include my rtos objects
#include "rtos_objects.h"

// DEFINES

// network initialisation function prototype is defined elsewhere
extern int32_t socket_startup(void);

// RTOS DEFINES

// data acquisition thread
osThreadId_t acq_thread;
extern void data_acquisition(void *argument);
static const osThreadAttr_t data_acq_thread_attr =
{
  .name = "data_acquisition",
  .priority = osPriorityNormal,
};

// mqtt thread (with increased stack size)
osThreadId_t mqtt_thread;
extern void mqtt_run_task(void *argument);
static const osThreadAttr_t mqtt_thread_attr =
{
  .name = "mqtt_thread",
  .priority = osPriorityNormal,
  .stack_size = 8192U,
};

// my message queue
osMessageQueueId_t m_messages;
static const osMessageQueueAttr_t msq_q_attr =
{
  .name = "my_messages",
};

// MAIN THREAD

// provide an increased stack size to the main thread
static const osThreadAttr_t app_main_attr =
{
  .stack_size = 8192U
};

// main thread
static void app_main(void *argument)
{
  // print a startup message
	printf("\r\n");
  printf("\rMQTT over TLS using WiFi\r\n");

  // set up the network connection
  int32_t status = socket_startup();

  // create the message queue
  m_messages = osMessageQueueNew(16, sizeof(message_t), &msq_q_attr);
	
  // if the network connected ok then create the mqtt thread to send and
  // receive packets
  if(status == 0)
  {
		acq_thread  = osThreadNew(data_acquisition, NULL, &data_acq_thread_attr);
    mqtt_thread = osThreadNew(mqtt_run_task, NULL, &mqtt_thread_attr);
  }
}

// THREAD INITIALISATION

// initialise the application (by creating the main thread)
void app_initialize(void)
{
  osThreadNew(app_main, NULL, &app_main_attr);
}
