/*
 * clock.c
 *
 * some utility functions for clock configuration (both for peripherals and for
 * the system clock)
 *
 * author:  alex shenfield
 * date:    01/09/2017
 */
 
// include the shu library bsp clock header
#include "clock.h"
 
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
  
  // configure the systick appropriately
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  
}
 
// enable the gpio port clock for a specific pin
void enable_gpio_clock(gpio_pin_t pin)
{
  // check which port we need to enable the gpio clock for - in the stm32f7
  // discovery board we only have upto port i
  switch((uint32_t)pin.gpio_port)
  {
    case GPIOA_BASE:
      __GPIOA_CLK_ENABLE();
      break;
    case GPIOB_BASE:
      __GPIOB_CLK_ENABLE();
      break;
    case GPIOC_BASE:
      __GPIOC_CLK_ENABLE();
      break;
    case GPIOD_BASE:
      __GPIOD_CLK_ENABLE();
      break;
    case GPIOE_BASE:
      __GPIOE_CLK_ENABLE();
      break;
    case GPIOF_BASE:
      __GPIOF_CLK_ENABLE();
      break;
    case GPIOG_BASE:
      __GPIOG_CLK_ENABLE();
      break;
    case GPIOH_BASE:
      __GPIOH_CLK_ENABLE();
      break;
    case GPIOI_BASE:
      __GPIOI_CLK_ENABLE();
      break;
  }
}
