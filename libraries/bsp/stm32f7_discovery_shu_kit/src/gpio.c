/*
 * gpio.c
 *
 * this is a simplified wrapper around the gpio configuration functions 
 * provided for the embedded computer networks module at Sheffield Hallam
 * University.
 *
 * author:  Dr. Alex Shenfield
 * date:    01/09/2017
 */

// include the shu library bsp gpio header
#include "gpio.h"

// initialise the gpio (this is called once for every gpio pin we want to use)
void init_gpio(gpio_pin_t pin, int direction)
{
  // declare a gpio typedef structure that basically contains all the gpio
  // settings and properties you can use
  GPIO_InitTypeDef gpio_init_structure;
  
  // enable the clock for the pin
  enable_gpio_clock(pin);

  // configure the gpio settings for the pin
  gpio_init_structure.Pin   = pin.gpio_pin;
  gpio_init_structure.Pull  = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FAST;
  
  // configure output or input
  if(direction == OUTPUT)
  {
    gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
  }
  else if(direction == INPUT)
  {   
    gpio_init_structure.Mode  = GPIO_MODE_INPUT;
  }
  
  // complete initialisation
  HAL_GPIO_Init(pin.gpio_port, &gpio_init_structure);  
  HAL_GPIO_WritePin(pin.gpio_port, pin.gpio_pin, GPIO_PIN_RESET);
}

// the below functions are pretty trivial, but are provided to simplify the set
// of stuff you have to remember for embedded programming with the stm32f7 
// discovery boards

// would be nice to do some checks to see if the pins are configured as gpio
// inputs or outputs before trying to use them - is there a HAL function to 
// check configurations?

// write a digital value to the specified gpio pin
void write_gpio(gpio_pin_t pin, int value)
{
  if(value == LOW)
  {
    HAL_GPIO_WritePin(pin.gpio_port, pin.gpio_pin, GPIO_PIN_RESET);
  }
  else if(value == HIGH)
  {
    HAL_GPIO_WritePin(pin.gpio_port, pin.gpio_pin, GPIO_PIN_SET);
  }
}

// toggle a specified gpio pin
void toggle_gpio(gpio_pin_t pin)
{
  HAL_GPIO_TogglePin(pin.gpio_port, pin.gpio_pin);
}

// read the digital value from the specified gpio pin
uint16_t read_gpio(gpio_pin_t pin)
{
  // return the pin value
  return HAL_GPIO_ReadPin(pin.gpio_port, pin.gpio_pin);
}
