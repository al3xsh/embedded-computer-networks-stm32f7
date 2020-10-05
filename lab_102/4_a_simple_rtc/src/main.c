/*
 * main.c
 *
 * this is the main rtc calendar application (this code is dependent on the
 * extra shu libraries such as the pinmappings list, the clock configuration
 * library, the rtc library, and the gpio library)
 *
 * author:    Dr. Alex Shenfield
 * date:      05/10/2020
 * purpose:   55-604481 embedded computer networks : lab 102
 */

// include the c standard io functions
#include <stdio.h>

// include the hal drivers
#include "stm32f7xx_hal.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "adc.h"
#include "gpio.h"
#include "real_time_clock.h"

// DECLARATIONS

// stdout_init is defined in our configuration file (as part of the ARM
// Compiler user code template
extern int stdout_init(void);

// CODE

// this is the main method
int main()
{
  // we need to initialise the hal library and set up the SystemCoreClock
  // properly
  HAL_Init();
  init_sysclk_216MHz();

  // set our current date
  rtc_date_t date;
  date.day = RTC_WEEKDAY_MONDAY;
  date.day_date = 0x05;
  date.month = RTC_MONTH_OCTOBER;
  date.year = 0x20;

  // ... and time
  rtc_time_t time;
  time.hour = 0x10;
  time.minute = 0x52;
  time.second = 0x00;

  // initialise the uart and rtc (and force configuration of date and time)
  stdout_init();
  init_rtc(date, time);
  config_datetime(date, time);

  // print an initial status message
  printf("we are alive!\r\n");

  // loop forever ...
  while(1)
  {
    // get the date and time and print to the terminal
    uint8_t time[50] = {0};
    uint8_t date[50] = {0};
    get_datetime(time, date);
    printf("%s %s\r\n", date, time);

    // pause for ~1 sec
    HAL_Delay(1000);
  }
}


