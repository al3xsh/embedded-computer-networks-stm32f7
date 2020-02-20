/*
 * main.c
 *
 * this is the main rtc calendar application (this code is dependent on the
 * extra shu libraries such as the pinmappings list, the clock configuration
 * library, and the gpio library)
 *
 * author:    Dr. Alex Shenfield
 * date:      19/02/2020
 * purpose:   55-604481 embedded computer networks : additional examples
 */

// include the hal drivers
#include "stm32f7xx_hal.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "adc.h"
#include "gpio.h"

// include the serial configuration files
#include "serial.h"

// FUNCTION PROTOTYPES

// initialise the rtc
void init_rtc(void);
void config_datetime(void);
void get_datetime(uint8_t *time, uint8_t *date);

// DECLARATIONS

// rtc handle instance to store settings
RTC_HandleTypeDef hrtc;

// CODE

// this is the main method
int main()
{
  // we need to initialise the hal library and set up the SystemCoreClock 
  // properly
  HAL_Init();
  init_sysclk_216MHz();
  
  // initialise the uart and rtc
  init_uart(9600);
	init_rtc();
  
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

// initialise the rtc 
void init_rtc(void)
{
	// set up initialisation structures for the oscillator and the peripheral 
	// clock
	RCC_OscInitTypeDef rtcclk_init;
	RCC_PeriphCLKInitTypeDef pclk_init;
	
	// enable the power clock and access to the backup domain
	__HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();
	
	// initialise the lse oscillator
	rtcclk_init.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  rtcclk_init.PLL.PLLState = RCC_PLL_NONE;
  rtcclk_init.LSEState = RCC_LSE_ON;
	HAL_RCC_OscConfig(&rtcclk_init);
	
	// configure the lse oscillator as the rtc clock source
	pclk_init.PeriphClockSelection = RCC_PERIPHCLK_RTC;
	pclk_init.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	HAL_RCCEx_PeriphCLKConfig(&pclk_init);
	
	// enable the rtc clock
	__HAL_RCC_RTC_ENABLE();
	
	// set up the rtc configuration
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 0x7F;
  hrtc.Init.SynchPrediv = 0x00FF;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Instance = RTC;
	
	// initialise the rtc
	HAL_RTC_Init(&hrtc);
	
	// if we've not already got data stored in the backup register, then set the 
	// time and date
	if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x32F2)
  {
    config_datetime();
  }
	
	//// or force reconfiguration
	//config_datetime();
}

// set the rtc time and date
void config_datetime(void)
{
	// initialise structures for the date and time
  RTC_DateTypeDef sdate_structure;
  RTC_TimeTypeDef stime_structure;

  // set the date: Wednesday February 19th 2020
  sdate_structure.Year = 0x20;
  sdate_structure.Month = RTC_MONTH_FEBRUARY;
  sdate_structure.Date = 0x19;
  sdate_structure.WeekDay = RTC_WEEKDAY_WEDNESDAY;  
  HAL_RTC_SetDate(&hrtc,&sdate_structure, RTC_FORMAT_BCD);

  // set time: 16:00:00
  stime_structure.Hours = 0x16;
  stime_structure.Minutes = 0x00;
  stime_structure.Seconds = 0x00;
  stime_structure.TimeFormat = RTC_HOURFORMAT_24;
  stime_structure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stime_structure.StoreOperation = RTC_STOREOPERATION_RESET;
  HAL_RTC_SetTime(&hrtc, &stime_structure, RTC_FORMAT_BCD);

  // put some data in backup register 0 indicating we have successfully updated
	// the time and date
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR0, 0x32F2);
}

// get the date and time from the rtc
void get_datetime(uint8_t* time, uint8_t* date)
{
	// initialise structures for the date and time
  RTC_DateTypeDef sdate_structure;
  RTC_TimeTypeDef stime_structure;

	// set the base centuary
	uint16_t base_year = 2000;
  
	// get the date and time from the rtc
  HAL_RTC_GetTime(&hrtc, &stime_structure, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &sdate_structure, RTC_FORMAT_BIN);
	
	// copy the time and date into the supplied string buffers as formatted 
	// strings
  sprintf((char *)time, "%02d:%02d:%02d", 
					stime_structure.Hours, 
					stime_structure.Minutes, 
					stime_structure.Seconds);
  sprintf((char *)date, "%04d-%02d-%02d", 
					base_year + sdate_structure.Year, 
					sdate_structure.Month, 
					sdate_structure.Date);
}
