/*
 * blinky_thread.c
 *
 * simple threads to handle blinking some leds on the discovery board - updated
 * for the stm32f7xx hal libraries and cmsis-rtos2
 *
 * to get the thread execution information in debugging mode:
 *
 * - view -> analysis windows -> system analyzer (thread execution times)
 * - view -> watch windows -> rtx rtos (thread behaviour - e.g. waiting on
 *   delay)
 *
 * author:    Dr. Alex Shenfield
 * date:      12/10/2021
 * purpose:   55-604481 embedded computer networks - lab 103
 */

// include the basic headers for the hal drivers
#include "arm_acle.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os2.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "clock.h"
#include "gpio.h"
#include "pinmappings.h"

// HARDWARE DEFINES

// specify some leds
gpio_pin_t led1 = {PB_14, GPIOB, GPIO_PIN_14};
gpio_pin_t led2 = {PB_15, GPIOB, GPIO_PIN_15};
gpio_pin_t led3 = {PA_8,  GPIOA, GPIO_PIN_8};

// RTOS DEFINES

// declare the thread ids
osThreadId_t tid_led_1_thread;
osThreadId_t tid_led_2_thread;
osThreadId_t tid_led_3_thread;

// OTHER FUNCTIONS

// function prototype for our dumb delay function
void dumb_delay(uint32_t delay);

// ACTUAL WORKER THREADS

// THREAD 1

// led thread 1 attributes
static const osThreadAttr_t thread_1_attr =
{
  .name = "led_1",
  .priority = osPriorityNormal,
};

// blink led 1
void led_1_thread(void *argument)
{
  while(1)
  {
    // toggle the first led
    toggle_gpio(led1);
    dumb_delay(500);
  }
}

// THREAD 2

// led thread 2 attributes
static const osThreadAttr_t thread_2_attr =
{
  .name = "led_2",
  .priority = osPriorityNormal,
};

// blink led 2
void led_2_thread(void *argument)
{
  while(1)
  {
    // toggle the second led
    toggle_gpio(led2);
    dumb_delay(1000);
  }
}

// THREAD 3

// led thread 3 attributes
static const osThreadAttr_t thread_3_attr =
{
  .name = "led_3",
  .priority = osPriorityNormal,
};

// blink led 3
void led_3_thread(void *argument)
{
  while(1)
  {
    // toggle the third led
    toggle_gpio(led3);
    dumb_delay(1500);
  }
}

// APPLICATION MAIN THREAD

// application main thread - initialise peripherals and start the worker 
// threads
void app_main(void *argument)
{
  // initialise peripherals here
  init_gpio(led1, OUTPUT);
  init_gpio(led2, OUTPUT);
  init_gpio(led3, OUTPUT);

  // create the threads
  tid_led_1_thread = osThreadNew(led_1_thread, NULL, &thread_1_attr);
  tid_led_2_thread = osThreadNew(led_2_thread, NULL, &thread_2_attr);
  tid_led_2_thread = osThreadNew(led_3_thread, NULL, &thread_3_attr);
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
