/*
 * main.c
 *
 * this is the main rtos application
 *
 * this is deliberately written to be as generic as possible so we can reuse
 * it - we will have all our thread specific stuff in a separate file
 *
 * author:    Dr. Alex Shenfield
 * date:      22/10/2021
 * purpose:   55-604481 embedded computer networks - lab 103
 */

// include the basic headers for the hal drivers and cmsis-rtos2
#include "stm32f7xx_hal.h"
#include "cmsis_os2.h"

// include the clock configuration file from the shu bsp library
#include "clock.h"

// EXTERNAL DECLARATIONS

// this is our main application thread initialisation function - it is defined
// elsewhere (in our app_main.c file)
extern void app_initialise(void);

// CODE

// this is the main method
int main()
{
  // we need to initialise the hal library and set up the SystemCoreClock
  // properly
  HAL_Init();
  init_sysclk_216MHz();

  // at this stage the microcontroller clock setting is already configured.
  // this is done through SystemInit() function which is called from the
  // startup file (startup_stm32f7xx.s) before to branch to application main.
  // to reconfigure the default setting of SystemInit() function, refer to
  // system_stm32f7xx.c file

  // initialise the real time kernel
  osKernelInitialize();

  // create application main thread (defined elsewhere)
  app_initialise();

  // start everything running
  osKernelStart();
}
