/* 
 * uart_processing_thread.c
 *
 * a thread to handle uart communications 
 *
 * - data is put into the message queue byte by byte as it comes in over the
 *   uart by the irq
 * - the rx thread grabs the available data from the message queue and decides
 *   whether to turn the led on or off
 *
 * author:    Alex Shenfield
 * date:      06/09/2017
 * purpose:   55-604481 embedded computer networks : lab 103
 */

// include the relevant header files (from the c standard libraries)
#include <stdio.h>
#include <string.h>

// include the serial configuration files and the rtos api
#include "serial.h"
#include "cmsis_os.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "gpio.h"

// RTOS DEFINES

// declare the thread function prototypes, thread id, and priority
void uart_rx_thread(void const *argument);
osThreadId tid_uart_rx_thread;
osThreadDef(uart_rx_thread, osPriorityAboveNormal, 1, 0);

// setup a message queue to use for receiving characters from the interrupt
// callback
osMessageQDef(message_q, 128, uint8_t);
osMessageQId msg_q;

// HARDWARE DEFINES

// map the led to GPIO PI2 
gpio_pin_t led = {PI_1, GPIOI, GPIO_PIN_2};

// THREAD INITIALISATION

// create the uart thread(s)
int init_uart_threads(void)
{
  // initialise gpio output for led
  init_gpio(led, OUTPUT);
  
  // set up the uart at 9600 baud and enable the rx interrupts
  init_uart(9600);
  enable_rx_interrupt();
  
  // print a status message
  printf("we are alive!\r\n");

  // create the message queue
  msg_q = osMessageCreate(osMessageQ(message_q), NULL);
  
  // create the thread and get its task id
  tid_uart_rx_thread = osThreadCreate(osThread(uart_rx_thread), NULL);

  // check if everything worked ...
  if(!tid_uart_rx_thread)
  {
    return(-1);
  }
  return(0);
}

// ACTUAL THREADS

// uart receive thread
void uart_rx_thread(void const *argument)
{
  // print some status message ...
  printf("still alive!\r\n");
  
  // create a packet array to use as a buffer to build up our string
  uint8_t packet[128];
  int i = 0;
  
  // infinite loop ...
  while(1)
  {     
    // wait for there to be something in the message queue
    osEvent evt = osMessageGet(msg_q, osWaitForever);
    
    // process the message queue ...
    if(evt.status == osEventMessage)
    {
      // get the message and increment the counter
      uint8_t byte = evt.value.v;
      
      // if we get a new line character ...
      if(byte == '\n')
      {    
        // add string terminator (we need this - otherwise we will keep reading 
        // past the end of the last packet received ...)
        packet[i] = '\0';
        
        // decide whether to turn the led on or off by doing a string 
        // comparison ...
        // note: the strcmp function requires an exact match so if you are 
        // using teraterm or putty you will need to set the tx options 
        // correctly
        // see: http://www.tutorialspoint.com/c_standard_library/c_function_strcmp.htm
        // for strcmp details ...
        if(strcmp((char*)packet, "on-led1\r") == 0)
        {
          write_gpio(led, HIGH);
          printf("led 1 on\r\n");
        }
        if(strcmp((char*)packet, "off-led1\r") == 0)
        {
          write_gpio(led, LOW);
          printf("led 1 off\r\n");
        }
        
        // print debugging message to the uart
        printf("DEBUGGING: %s\r\n", packet);
        
        // zero the packet index
        i = 0;
      }
      else
      {
        packet[i] = byte;
        i++;
      }       
    }
  } 
}
