/*
 * adc.c
 *
 * this is the simplified wrapper around the adc configuration functions 
 * provided for the embedded computer networks module at Sheffield Hallam
 * University.
 * 
 * for the adc configuration (e.g. which adc, which channel, etc.) we need to
 * look at the stm32f7 discovery user manual - en.DM00166116.pdf
 *
 * on the stm32f746g discovery board there are only 6 analog pins broken out
 * (corresponding to those on the arduino header).
 *
 * author: 	Dr. Alex Shenfield
 * date:		01/09/2017
 */
 
// include the shu library bsp adc header
#include "adc.h"

// get a static instance of the adc handle
static ADC_HandleTypeDef adc_handle;

// provide function prototypes for internal functions
void configure_adc_gpio(gpio_pin_t pin);
uint32_t get_adc_channel(gpio_pin_t pin);

uint16_t read_adc_once(gpio_pin_t pin);

// LIBRARY FUNCTIONS

// initialise the adc (this is called once for every gpio pin we want to use
// with the adc)
void init_adc(gpio_pin_t pin)
{
	// would *love* to have a decent way of testing whether this is a valid adc
	// pin ...
	// ...
	
	// configure the hardware
	configure_adc_gpio(pin);

	// configure the adc clock
	__ADC3_CLK_ENABLE();
	
	// configure adc peripheral
	adc_handle.Instance 									= ADC3;

	// in these settings we have set continuous conversion mode to disable, so we
	// have to start a new reading everytime we want to read the adc. however, 
	// this does appear much more stable than enabling continuous conversion 
	// mode. 
	
	adc_handle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV4;
	adc_handle.Init.Resolution            = ADC_RESOLUTION_12B;
	adc_handle.Init.ScanConvMode          = DISABLE;
	adc_handle.Init.ContinuousConvMode    = DISABLE;
	adc_handle.Init.DiscontinuousConvMode = DISABLE;
	adc_handle.Init.NbrOfDiscConversion   = 0;
	adc_handle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
	adc_handle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
	adc_handle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
	adc_handle.Init.NbrOfConversion       = 1;
	adc_handle.Init.DMAContinuousRequests = DISABLE;
	adc_handle.Init.EOCSelection          = DISABLE;
	
	HAL_ADC_Init(&adc_handle);
}

// read the adc multiple times and average the result
uint16_t read_adc(gpio_pin_t pin)
{
	// average readings over 64 samples to eliminate some of the annoying noise
  uint8_t   nsamples = 64;
  uint32_t  total = 0;
	
	// read n samples (32 / 64 works reasonably well, but any more doesn't really 
	// improve the result)
  int ix;
  for(ix = 0; ix < nsamples; ix++)
  {
    total += read_adc_once(pin);
  }
  
  // return the average
  return (total / nsamples);  	
}

// read the adc once
uint16_t read_adc_once(gpio_pin_t pin)
{	
	// first configure the correct channel ...
	
	// set up configuration structure
	ADC_ChannelConfTypeDef adc_config_ch;

	// configure the adc channel
	adc_config_ch.Channel      = get_adc_channel(pin);
	adc_config_ch.Rank         = 1;
	adc_config_ch.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	adc_config_ch.Offset       = 0;

	HAL_ADC_ConfigChannel(&adc_handle, &adc_config_ch);
	
	// then read the adc ...
	
	HAL_ADC_Start(&adc_handle);
	HAL_ADC_PollForConversion(&adc_handle, 10);
	if(HAL_ADC_GetState(&adc_handle) & HAL_ADC_STATE_EOC_REG)
	{
		return (HAL_ADC_GetValue(&adc_handle));
	}
	else
	{
		return 0;
	}	
}

// LOCAL UTILITY FUNCTIONS 

// configure the adc gpio settings
void configure_adc_gpio(gpio_pin_t pin)
{
	// enable gpio clock
	enable_gpio_clock(pin);
	
	// declare a gpio typedef structure that basically contains all the gpio
	// settings and properties you can use
	GPIO_InitTypeDef gpio_init_structure;

	// configure gpio
	gpio_init_structure.Pin 	= pin.gpio_pin;
	gpio_init_structure.Mode 	= GPIO_MODE_ANALOG;
	gpio_init_structure.Pull 	= GPIO_NOPULL;

	HAL_GPIO_Init(pin.gpio_port, &gpio_init_structure);
}

// get the correct adc channel for this pin
uint32_t get_adc_channel(gpio_pin_t pin)
{
	// get the adc channel for this pin by iterating over all adc pins until
	// we either find it or get to the end of the array
	int i = 0;
	while(stm32f7_adc_pins[i].gpio.pin_id != NC) 
	{		
		if(stm32f7_adc_pins[i].gpio.pin_id == pin.pin_id)
		{
			return stm32f7_adc_pins[i].adc_channel;
		}
		i++;
  }
	
	// return a stupid number if we haven't found an appropriate adc channel
	return 0xFFFFFFFF;
}
