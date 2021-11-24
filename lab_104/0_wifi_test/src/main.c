/*
 * main.c
 *
 * this contains the set up code for the wifi debugging application
 *
 * in this file we configure the clock settings for the microcontroller, set up
 * some of the peripherals, create the main thread (defined in a separate
 * file), and then pass control to the RTOS kernel.
 *
 * author:    Dr. Alex Shenfield
 * date:      17/11/2021
 * purpose:   55-604481 embedded computer networks - lab 104
 */

// include our main.h file
#include "main.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "clock.h"
#include "random_numbers.h"

// SYSTICK BASED FUNCTION OVERRIDES FOR RTOS

// override the default HAL_GetTick function (because we don't have access to
// the SysTick timer when running the real time operating system)
uint32_t HAL_GetTick(void)
{
  return ((uint32_t)osKernelGetTickCount());
}

// make HAL_Delay point to osDelay (otherwise any use of HAL_Delay breaks
// things)
void HAL_Delay(__IO uint32_t Delay)
{
  osDelay(Delay);
}

// DEFINES

// define a local function prototype for setting up the memory protection unit
static void init_mpu(void);

// MAIN CODE

// this is the main method
int main(void)
{
  // mpu stuff ...
  init_mpu();

  // enable the cpu cache
  SCB_EnableICache();
  SCB_EnableDCache();

  // we need to initialise the hal library and set up the SystemCoreClock
  // properly
  HAL_Init();
  init_sysclk_216MHz();
  SystemCoreClockUpdate();

  // configure the hardware random number generator to use in entropy
  // calculations
  init_random();

  // initialise the rtos kernel, create the application main thread (defined
  // elsewhere), and set everything off running
  osKernelInitialize();
  app_initialize();
  osKernelStart();

  // that's it - everything is now managed by the rtos!
  while(1)
  {
  }
}

// OTHER PERIPHERALS

// configure the memory protection unit on the stm32f7 discovery board to allow
// write through for the sram
static void init_mpu(void)
{
  // set up initialisation structure for the mpu
  MPU_Region_InitTypeDef mpu_config;

  // disable the mpu temporarily
  HAL_MPU_Disable();

  // set up the mpu configuration information
  mpu_config.Enable           = MPU_REGION_ENABLE;
  mpu_config.BaseAddress      = 0x20000000U;
  mpu_config.Size             = MPU_REGION_SIZE_512KB;
  mpu_config.AccessPermission = MPU_REGION_FULL_ACCESS;
  mpu_config.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
  mpu_config.IsCacheable      = MPU_ACCESS_CACHEABLE;
  mpu_config.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
  mpu_config.Number           = MPU_REGION_NUMBER0;
  mpu_config.TypeExtField     = MPU_TEX_LEVEL0;
  mpu_config.SubRegionDisable = 0x00U;
  mpu_config.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;

  // set the configuration
  HAL_MPU_ConfigRegion(&mpu_config);

  // reenable the mpu
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

