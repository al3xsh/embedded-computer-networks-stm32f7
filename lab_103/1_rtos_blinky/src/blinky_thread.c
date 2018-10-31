/* 
 * blinky_thread.c
 *
 * simple thread to handle blinking some leds on the discovery board - updated
 * for the stm32f7xx hal libraries
 *
 * author:    Dr. Alex Shenfield
 * date:      01/09/2017
 * purpose:   55-604481 embedded computer networks : lab 103
 */

// include the basic headers for the hal drivers
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "clock.h"
#include "gpio.h"
#include "pinmappings.h"

// HARDWARE DEFINES

// specify some leds
gpio_pin_t led1 = {PF_6, GPIOF, GPIO_PIN_6};
gpio_pin_t led2 = {PF_7, GPIOF, GPIO_PIN_7};
gpio_pin_t led3 = {PF_8, GPIOF, GPIO_PIN_8};

// RTOS DEFINES

// declare the thread function prototypes
void led_1_thread(void const *argument);
void led_2_thread(void const *argument);
void led_3_thread(void const *argument);

// declare the thread ids
osThreadId tid_led_1_thread;
osThreadId tid_led_2_thread;
osThreadId tid_led_3_thread;

// define thread priorities
osThreadDef(led_1_thread, osPriorityNormal, 1, 0);
osThreadDef(led_2_thread, osPriorityNormal, 1, 0);
osThreadDef(led_3_thread, osPriorityNormal, 1, 0);

// OTHER FUNCTIONS

// function prototype for our dumb delay function
void dumb_delay(uint32_t delay);

// THREAD INITIALISATION

// create the threads
int init_thread(void)
{
  // initialize peripherals here
  init_gpio(led1, OUTPUT);
  init_gpio(led2, OUTPUT);
  init_gpio(led3, OUTPUT);
  
  // create the thread and get its taks id
  tid_led_1_thread = osThreadCreate(osThread(led_1_thread), NULL);
  tid_led_2_thread = osThreadCreate(osThread(led_2_thread), NULL);
  tid_led_3_thread = osThreadCreate(osThread(led_3_thread), NULL);
  
  // check if everything worked ...
  if(!(tid_led_1_thread && tid_led_2_thread && tid_led_3_thread))
  {
    return(-1);
  }
  return(0);
}

// ACTUAL THREADS

// blink led 1
void led_1_thread(void const *argument)
{
  while(1)
  {
    // toggle the first led on the gpio pin
    toggle_gpio(led1);
    dumb_delay(500);
  }
}

// blink led 2
void led_2_thread(void const *argument)
{
  while(1)
  {
    // toggle the second led on the gpio pin
    toggle_gpio(led2);
    dumb_delay(200);
  }
}

// blink led 3
void led_3_thread(void const *argument)
{
  while(1)
  {
    // toggle the second led on the gpio pin
    toggle_gpio(led3);
    dumb_delay(1000);
  }
}

// OTHER FUNCTIONS

// dumb delay function
void dumb_delay(uint32_t delay)
{
  // just spin through processor cycles to introduce a delay 
  long delaytime;
  for(delaytime = 0; delaytime < (delay * 10000); delaytime++)
  {
    __nop();
  }
}


