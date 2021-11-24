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

// DEFINES

// network setup stuff
extern int32_t scan_wifi(void);
extern int32_t socket_startup(void);

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
  printf("\rWiFi test code\r\n");

  // set up the network connection
  int32_t test = scan_wifi();
  int32_t status = socket_startup();

}

// THREAD INITIALISATION

// initialise the application (by creating the main thread)
void app_initialize(void)
{
  osThreadNew(app_main, NULL, &app_main_attr);
}
