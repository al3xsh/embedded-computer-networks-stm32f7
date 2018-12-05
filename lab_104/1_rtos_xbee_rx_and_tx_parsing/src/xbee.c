/*
 * xbee.c
 *
 * provide some implementation functions for our xbee uart (uart 6 in this  
 * case).
 *
 * this version of xbee.c is both rtos aware and interrupt aware. it reads 
 * data from the uart using interrupts and sends them to a separate thread via
 * a message queue.
 * 
 * author:    Alex Shenfield
 * date:      08/11/2018
 * purpose:   55-604481 embedded computer networks : lab 104
 */

// include the relevant header files
#include "xbee.h"
#include "cmsis_os.h"

#include "itm_debug.h"
  
// FUNCTION PROTOTYPES

// configure the hardware part of the uart (e.g. gpio pins and clocks)
void configure_hardware_xbee(void);
  
// GLOBAL DECLARATIONS (FOR THIS MODULE)

// get an instance of the uart handle
UART_HandleTypeDef xbee_handle;
uint8_t c;

// RTOS DEFINE

// the message queue we are using to pass characters around is defined 
// elsewhere (in this case in the xbee_processing_thread.c file)
extern osMessageQId msg_q;

// METHODS

// uart initialisation
void init_xbee(uint32_t baud_rate)
{
  // configure the hardware
  configure_hardware_xbee();
  
  // set up the uart structure - uart 6 xxxx-8N1
  xbee_handle.Instance          = USART6;
  
  xbee_handle.Init.BaudRate     = baud_rate;
  xbee_handle.Init.WordLength   = UART_WORDLENGTH_8B;
  xbee_handle.Init.StopBits     = UART_STOPBITS_1;
  xbee_handle.Init.Parity       = UART_PARITY_NONE;
  xbee_handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  xbee_handle.Init.Mode         = UART_MODE_TX_RX;
  
  // initialise the uart
  HAL_UART_Init(&xbee_handle);  
}

// for the gpio set up stuff we need to look at the alternate function mapping
// table in DM00037051.pdf - pg. 61 onwards

// configure the hardware for the xbee uart (clocks, gpio pins, etc.)
void configure_hardware_xbee(void)
{
  // configure the usart 6 clocks (both uart and gpio clocks)
  __HAL_RCC_USART6_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
  // configure the gpio pins
  
  // declare a gpio typedef structure that basically contains all the gpio
  // settings and properties you can use
  GPIO_InitTypeDef gpio_init_structure;
  
  // configure the gpio output on the uart 6 pins (TX=PC6 and RX=PC7)
	//
	// note to self: the configuration of GPIO_PULLUP should (hopefully) reduce
	// some of the random noise that occasionally happens on the uart during 
	// microcontroller reset
  gpio_init_structure.Pin       = XBEE_TX_Pin;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_LOW;
  gpio_init_structure.Alternate = GPIO_AF8_USART6;
  HAL_GPIO_Init(XBEE_TX_GPIO_Port, &gpio_init_structure);
  
  gpio_init_structure.Pin       = XBEE_RX_Pin;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_LOW;
  gpio_init_structure.Alternate = GPIO_AF8_USART6;
  HAL_GPIO_Init(XBEE_RX_GPIO_Port, &gpio_init_structure);  
  
  // set up the nested vector interrupt controller
  HAL_NVIC_SetPriority(USART6_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(USART6_IRQn);
}

// LOW LEVEL IO

// put a character onto "the wire"
int xbee_write(int ch)
{	
  // use the hal uart transmit function to send a character
  HAL_UART_Transmit(&xbee_handle, (uint8_t *)&ch, 1, 0xFFFF);
  
  // return the character
  return ch;
}

// XBEE TRANSMIT

// send a message character by character on the xbee uart
int send_xbee(volatile uint8_t* s, int length)
{	
  int i = 0;
	for(i = 0; i < length; i++)
	{		
		// put the character on the wire
		xbee_write(s[i]);
	}	
	return length;
}

// XBEE RECEIVE

// ENABLE THE RX INTERRUPT FOR XBEE UART

// uart 6 rx interrupt enable ...
void enable_rx_interrupt(void)
{ 
  // enable receive interrupt (single character at a time)
  HAL_UART_Receive_IT(&xbee_handle, &c, 1);
}

// UART IRQ CALLBACKS

// uart receive callback
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * xbee_handle)
{ 
  // stuff characters into the message queue (and return immediately - as this
  // is basically an isr ...)
  osMessagePut(msg_q, c, 0);
  
  // enable the interrupt again ...
  HAL_UART_Receive_IT(xbee_handle, &c, 1);  
}
