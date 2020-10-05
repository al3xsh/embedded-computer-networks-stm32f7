/*
 * real_time_clock.h
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
 
// define a symbol to prevent recursive inclusion
#ifndef __SHU_RTC
#define __SHU_RTC

// include the basic headers for the hal drivers
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_rtc.h"

// define a structure to hold details about the date for rtc initialisation
typedef struct
{
	uint8_t day;
	uint8_t day_date;
	uint8_t month;
	uint8_t year;
}
rtc_date_t;

// define a structure to hold details about the time for rtc initialisation
typedef struct
{
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}
rtc_time_t;

// initialisation functions
void init_rtc(rtc_date_t date, rtc_time_t time);
void config_datetime(rtc_date_t date, rtc_time_t time);

// get date and time as strings
void get_datetime(uint8_t *time, uint8_t *date);

#endif
// __SHU_RTC
