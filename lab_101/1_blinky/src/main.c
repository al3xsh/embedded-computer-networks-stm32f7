/*
 * main.c
 *
 * this is the main blinky application with no external dependancies apart from
 * the stm32f7xx hal libraries
 *
 * author:    Alex Shenfield
 * date:      01/09/2018
 * purpose:   55-604481 embedded computer networks : lab 101
 */

// include the hal drivers
#include "stm32f7xx_hal.h"

// map the led to GPIO PI1 (this is the inbuilt led by the reset button)
#define LED_PIN           GPIO_PIN_1
#define LED_PORT          GPIOI
#define LED_CLK_ENABLE()  __GPIOI_CLK_ENABLE()

// declare our utility functions
void init_sysclk_216MHz(void);
void configure_gpio(void);

// this is the main method
int main()
{
  // we need to initialise the hal library and set up the SystemCoreClock 
  // properly
  HAL_Init();
  init_sysclk_216MHz();
  
  // initialise the gpio pins
  configure_gpio();
  
  // loop forever ...
  while(1)
  {
    // toggle the led on the gpio pin
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
    
    // wait for 1 second
    HAL_Delay(1000);
  }
}

// CLOCK

// configure the stm32f7 discovery board to operate at 216MHz
void init_sysclk_216MHz(void)
{ 
  // set up initialisation structures for the oscillator and the clock
  RCC_OscInitTypeDef rcc_oscillator_config;
  RCC_ClkInitTypeDef rcc_clock_config;

  // call macros to configure the correct power settings on the chip
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  // configure the oscillator and phase-locked loops so the chip runs at 168MHz
  rcc_oscillator_config.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  rcc_oscillator_config.HSEState        = RCC_HSE_ON;
  rcc_oscillator_config.HSIState        = RCC_HSI_OFF;
  rcc_oscillator_config.PLL.PLLState    = RCC_PLL_ON;
  rcc_oscillator_config.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  rcc_oscillator_config.PLL.PLLM        = 25;
  rcc_oscillator_config.PLL.PLLN        = 432;
  rcc_oscillator_config.PLL.PLLP        = RCC_PLLP_DIV2;
  rcc_oscillator_config.PLL.PLLQ        = 9;
  HAL_RCC_OscConfig(&rcc_oscillator_config);

  // activate the OverDrive to reach the 216 MHz frequency
  HAL_PWREx_EnableOverDrive();
  
  // configure the appropriate clock dividers
  rcc_clock_config.ClockType            = RCC_CLOCKTYPE_HCLK
                                          |RCC_CLOCKTYPE_SYSCLK
                                          |RCC_CLOCKTYPE_PCLK1
                                          |RCC_CLOCKTYPE_PCLK2;
  rcc_clock_config.SYSCLKSource         = RCC_SYSCLKSOURCE_PLLCLK;
  rcc_clock_config.AHBCLKDivider        = RCC_SYSCLK_DIV1;
  rcc_clock_config.APB1CLKDivider       = RCC_HCLK_DIV4;
  rcc_clock_config.APB2CLKDivider       = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&rcc_clock_config, FLASH_LATENCY_7);
  
}

// HARDWARE

// configure the gpio
void configure_gpio()
{
  // declare a gpio typedef structure that basically contains all the gpio
  // settings and properties you can use
  GPIO_InitTypeDef gpio_init_structure;
  
  // enable the clock for the led
  LED_CLK_ENABLE();

  // configure the gpio output on the led pin
  gpio_init_structure.Pin   = LED_PIN;
  gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull  = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FAST;
  
  // complete initialisation
  HAL_GPIO_Init(LED_PORT, &gpio_init_structure);  
  HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
}
