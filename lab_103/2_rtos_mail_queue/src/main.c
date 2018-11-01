/*
 * main.c
 *
 * this is the main rtos mail queue application
 *
 * author:    Dr. Alex Shenfield
 * date:      06/09/2018
 * purpose:   55-604481 embedded computer networks : lab 103
 */

// include the basic headers for the hal drivers
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

// include the clock configuration file from the shu bsp library
#include "clock.h"

// declare the extern methods that set the threads up for us 
extern int init_data_thread(void);
extern int init_display_thread(void);

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
  init_data_thread();
  init_display_thread();
  
  // start everything running
  osKernelStart();
}
