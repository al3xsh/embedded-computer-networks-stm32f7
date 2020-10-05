/*
 * real_time_clock.c
 *
 * this is the simplified wrapper around the rtc configuration functions 
 * provided for the embedded computer networks module at Sheffield Hallam
 * University.
 *
 * note - time is in 24 hr clock & we assume we are not going to worry about 
 * british summer time in our simple applications
 *
 * author: 	Dr. Alex Shenfield
 * date:		05/10/2020
 */

// include the c standard io functions
#include <stdio.h>
 
// include the shu library bsp adc header
#include "real_time_clock.h"

// rtc handle instance to store settings
RTC_HandleTypeDef hrtc;
 
// initialise the rtc 
void init_rtc(rtc_date_t date, rtc_time_t time)
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
    config_datetime(date, time);
  }
}

// set the rtc time and date
void config_datetime(rtc_date_t date, rtc_time_t time)
{
	// initialise structures for the date and time
  RTC_DateTypeDef sdate_structure;
  RTC_TimeTypeDef stime_structure;

  // set the date
  sdate_structure.Year = date.year;
  sdate_structure.Month = date.month;
  sdate_structure.Date = date.day_date;
  sdate_structure.WeekDay = date.day;  
  HAL_RTC_SetDate(&hrtc,&sdate_structure, RTC_FORMAT_BCD);

  // set time
  stime_structure.Hours = time.hour;
  stime_structure.Minutes = time.minute;
  stime_structure.Seconds = time.second;
  stime_structure.TimeFormat = RTC_HOURFORMAT_24;
  stime_structure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
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
