/*
 * serial.c
 *
 * provide some implementation functions for our uart (uart 1 in this case).  
 * in this case we are not implementing interrupts at all - just using the 
 * uart to output status messages.
 *
 * the stm32f7 board allows us to send messages over the virtual com port 
 * (i.e. the usb st-link connection).
 * 
 * author:    Dr. Alex Shenfield
 * date:      01/09/2017
 * purpose:   55-604481 embedded computer networks : lab 103
 */

// include the relevant header files
#include "serial.h"

// retarget printf for ease of debugging
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
  
// FUNCTION PROTOTYPES

// configure the hardware part of the uart (e.g. gpio pins and clocks)
void configure_hardware_uart(void);
  
// GLOBAL DECLARATIONS (FOR THIS MODULE)
  
// uart handle object
UART_HandleTypeDef uart_handle;

// METHODS

// uart initialisation
void init_uart(uint32_t baud_rate)
{
  // configure the hardware
  configure_hardware_uart();
  
  // set up the uart structure - uart 1 xxxx-8N1
  uart_handle.Instance          = USART1;
  
  uart_handle.Init.BaudRate     = baud_rate;
  uart_handle.Init.WordLength   = UART_WORDLENGTH_8B;
  uart_handle.Init.StopBits     = UART_STOPBITS_1;
  uart_handle.Init.Parity       = UART_PARITY_NONE;
  uart_handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  uart_handle.Init.Mode         = UART_MODE_TX_RX;
  
  // initialise the uart
  HAL_UART_Init(&uart_handle);  
}

// for the gpio set up stuff we need to look at the alternate function mapping
// table in DM00037051.pdf - pg. 61 onwards

// configure the hardware for the uart (clocks, gpio pins, etc.)
void configure_hardware_uart(void)
{
  // configure the usart 1 clocks (both uart and gpio clocks)
  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  
  // configure the gpio pins
  
  // declare a gpio typedef structure that basically contains all the gpio
  // settings and properties you can use
  GPIO_InitTypeDef gpio_init_structure;
  
  // configure the gpio output on the uart 1  pins (TX=PA9 and RX=PB7)
  gpio_init_structure.Pin       = VCP_TX_Pin;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_LOW;
  gpio_init_structure.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(VCP_TX_GPIO_Port, &gpio_init_structure);
  
  gpio_init_structure.Pin       = VCP_RX_Pin;
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_NOPULL;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_LOW;
  gpio_init_structure.Alternate = GPIO_AF7_USART1;
  HAL_GPIO_Init(VCP_RX_GPIO_Port, &gpio_init_structure);
}

// LOW LEVEL IO

// put a character onto "the wire"
int serial_write(int ch)
{
  // use the hal uart transmit function to send a character
  HAL_UART_Transmit(&uart_handle, (uint8_t *)&ch, 1, 0xFFFF);
  
  // return the character
  return ch;
}

// implementation of putchar to retarget printf (we use putchar_protype here
// because it is easy to make this portable - e.g. by adding conditionals to
// the #define statement)
PUTCHAR_PROTOTYPE
{ 
  // return the character
  return serial_write(ch);
}
