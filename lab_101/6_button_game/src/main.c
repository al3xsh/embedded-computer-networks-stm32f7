/*
 * main.c
 *
 * this is the main button game application for two buttons (this code is 
 * dependent on the extra shu libraries such as the pinmappings list and the
 * clock configuration library)
 *
 * author:    Alex Shenfield
 * date:      04/09/2017
 * purpose:   16-6498 embedded computer networks : lab 101
 */

// include the hal drivers
#include "stm32f7xx_hal.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "random_numbers.h"
#include "gpio.h"

// include the serial configuration files
#include "serial.h"

// DECLARATIONS

// leds
gpio_pin_t led1 = {PB_8, GPIOB, GPIO_PIN_8};
gpio_pin_t led2 = {PB_9, GPIOB, GPIO_PIN_9};

// buttons
gpio_pin_t pb1 = {PC_6, GPIOC, GPIO_PIN_6};
gpio_pin_t pb2 = {PC_7, GPIOC, GPIO_PIN_7};

// local game functions
uint8_t get_led(void);
uint8_t get_guess(void);
void clear_leds(void);

// CODE

// this is the main method
int main()
{
  // we need to initialise the hal library and set up the SystemCoreClock 
  // properly
  HAL_Init();
  init_sysclk_216MHz();
  
  // initialise the uart and random number generator
  init_uart(115200);
  init_random();
  
  // set up the gpio
  init_gpio(led1, 0);
  init_gpio(led2, 0);
  init_gpio(pb1, 1);
  init_gpio(pb2, 1);
  
  // print an initial status message
  printf("we are alive!\r\n");
  
  // game variables ...
  
  // initialise time value
  uint32_t current_time = 0;

  // initialise some game variables
  uint32_t  timeout = 3000;
  uint8_t   current_led = 0;
  uint8_t   guessed_led = 0;
  
  // game loop ...
  
  // loop forever ...
  while(1)
  {
    // set the current time and switch on an led
    current_time = HAL_GetTick();
    current_led = get_led();

    // while we've not run out of time ...
    while(HAL_GetTick() < (current_time + timeout))
    {
      // get the current guess
      guessed_led = get_guess();

      // check if we have actually made a guess
      if(guessed_led != 0)
      {
        // were we correct?
        if(guessed_led == current_led)
        {
          // reset the timer and get a new led
          printf("excellent - you win!\r\n");
          current_led = get_led();
          current_time = HAL_GetTick();
        }
        else
        {
          // reset the timer and get a new led
          printf("sorry - you lose!\r\n");
          current_led = get_led();
          current_time = HAL_GetTick();
        }
      }
    }

    // we ran out of time
    printf("you ran out of time :(\r\n");
  }
}

// GAME FUNCTIONS

// get a new led
uint8_t get_led()
{
  // declare a random number variable and a current led variable
  uint32_t random_num = 0;
  uint8_t  current_led = 0;

  // make sure all leds are off
  clear_leds();
  HAL_Delay(500);

  // get a random number
  random_num = get_random_int();
  printf("Random number = %d\r\n", (random_num % 4));

  // use the random number to calculate which led to switch on
  current_led = (random_num % 2) + 1;
  if(current_led == 1)
  {
    write_gpio(led1, HIGH);
  }
  if(current_led == 2)
  {
    write_gpio(led2, LOW);
  }

  // return the chosen led
  return current_led;
}

// get led guess
uint8_t get_guess()
{
  // initialise variables for the button states
  uint8_t button_1_state = 0;
  uint8_t button_2_state = 0;

  // read the buttons
  button_1_state = read_gpio(pb1);
  button_2_state = read_gpio(pb2);
  
  // wait for a short period of time
  HAL_Delay(25);
  
  // read the buttons again
  button_1_state = button_1_state & read_gpio(pb1);
  button_2_state = button_2_state & read_gpio(pb2);

  // return a bit mask representing which led we guessed 
  // (00000010 is led 2)
  return ((button_2_state << 1) | (button_1_state));
}

void clear_leds()
{
  // remember to clear all leds
  write_gpio(led1, LOW);
  write_gpio(led2, LOW);
}
