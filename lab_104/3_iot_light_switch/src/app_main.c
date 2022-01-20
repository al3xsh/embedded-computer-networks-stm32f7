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

// include my gpio library
#include "pinmappings.h"
#include "gpio.h"

// DEFINES

// network initialisation function prototype is defined elsewhere
extern int32_t socket_startup(void);

// push button
gpio_pin_t pb1 = {PI_3, GPIOI, GPIO_PIN_3};

// RTOS DEFINES

// thread(s)

// mqtt thread (with increased stack size)
osThreadId_t mqtt_thread;
extern void mqtt_run_task(void *argument);
static const osThreadAttr_t mqtt_thread_attr =
{
  .name = "mqtt_thread",
  .priority = osPriorityNormal,
  .stack_size = 8192U,
};

// timer

// declare a timer callback and a timer
void test_for_button_press(void * parameters);
osTimerId_t button;
static const osTimerAttr_t pb_timer =
{
  .name = "button_debouncing_timer",
};

// event flag

// declare button pressed event flag
osEventFlagsId_t button_flag;
static const osEventFlagsAttr_t button_flag_attr =
{
  .name = "button_event",
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
  printf("\rMQTT using WiFi\r\n");

  // set up the network connection
  int32_t status = socket_startup();

  // initialise the pushbutton and create the button pressed event flag
  init_gpio(pb1, INPUT);
  button_flag = osEventFlagsNew(&button_flag_attr);

  // create the timer object for button debouncing and start it
  button = osTimerNew(&test_for_button_press, osTimerPeriodic, NULL, &pb_timer);
  osTimerStart(button, 5);

  // if the network connected ok then create the mqtt thread to send and
  // receive packets
  if(status == 0)
  {
    mqtt_thread = osThreadNew(mqtt_run_task, NULL, &mqtt_thread_attr);
  }
}

// BUTTON DEBOUNCING

// button debouncing using pattern matching (implemented as a timer callback)
void test_for_button_press(void * parameters)
{
  // 8 bits of button history
  static uint8_t button_history = 0xFF;

  // every time this timer callback is called we shift the button history
  // across and update the state
  button_history = button_history << 1;
  uint8_t val = read_gpio(pb1);
  button_history = button_history | val;

  // use some simple pattern matching to see if the button has been pressed 
  // and released - if so, reset the button history and set the event flag ...
  if((button_history & 0xC7) == 0x07)
  {
    // reset button history
    button_history = 0xFF;

    // signal the button has been pressed
    osEventFlagsSet(button_flag, 0x01);
  }
}

// THREAD INITIALISATION

// initialise the application (by creating the main thread)
void app_initialize(void)
{
  osThreadNew(app_main, NULL, &app_main_attr);
}
