/*
 * main.c
 *
 * this is the main rtos uart based application
 *
 * author:    Alex Shenfield
 * date:      06/09/2018
 * purpose:   55-604481 embedded computer networks : lab 103
 */

// include the basic headers for the hal drivers and the rtos library
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

// include the clock configuration functions
#include "clock.h"

// declare the extern methods that set everything up for us 
extern int init_lcd_and_led_threads(void);
extern int init_uart_threads(void);

// OVERRIDE HAL DELAY

// make HAL_Delay point to osDelay (otherwise any use of HAL_Delay breaks things)
void HAL_Delay(__IO uint32_t Delay)
{
  osDelay(Delay);
}

// this is the main method
int main()
{ 
  // initialise the real time kernel
  osKernelInitialize();
  
  // we need to initialise the hal library and set up the SystemCoreClock 
  // properly
  HAL_Init();
  init_sysclk_216MHz();
  
  // at this stage the microcontroller clock setting is already configured. 
  // this is done through SystemInit() function which is called from the 
  // startup file (startup_stm32f7xx.s) before to branch to application main.
  // to reconfigure the default setting of SystemInit() function, refer to
  // system_stm32f7xx.c file
  //
  // note also that we need to set the correct core clock in the RTX_Conf_CM.c
  // file (OS_CLOCK) which we can do using the configuration wizard
    
  // initialise our threads
  init_lcd_and_led_threads();
  init_uart_threads();
  
  // note: the stack size for "main" has to be reasonably large (currently 1024
  // seems to work) because the uart set up and initial status message display
  // is effective done in "main" (i.e. before handing control over to the other
  // threads
    
  // start everything running
  osKernelStart();
}
