/*
 * repl_thread.c
 *
 * this is where we create and configure the main worker thread(s)
 *
 * in this application we are going to use the uart connection (via a terminal
 * program such as teraterm) to implement a simple read-eval-print loop (repl)
 * shell to accept and process a set of commands
 *
 * author:    Dr. Alex Shenfield
 * date:      22/10/2021
 * purpose:   55-604481 embedded computer networks - lab 103
 */

// include the relevant header files (from the c standard libraries)
#include <stdio.h>
#include <string.h>

// include the basic headers for the hal drivers and cmsis-rtos2
#include "arm_compat.h"
#include "stm32f7xx_hal.h"
#include "cmsis_os2.h"

// include the CMSIS Driver for code for the USART
#include "Driver_USART.h"

// include my rtos objects
#include "rtos_objects.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "gpio.h"

// HARDWARE DEFINES

// define some leds that we can use to indicate system status
gpio_pin_t data_led    = {PB_14, GPIOB, GPIO_PIN_14};
gpio_pin_t error_led   = {PB_15, GPIOB, GPIO_PIN_15};
gpio_pin_t control_led = {PI_1,  GPIOI, GPIO_PIN_1};

// USART DEFINES

// we are using usart 1 which is defined elsewhere
extern ARM_DRIVER_USART Driver_USART1;

// USART CALLBACK HANDLER

// this is the callback which is triggered when a usart event occurs
void my_usart_callback(uint32_t event)
{
  // receive success
  uint32_t recv_mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
                       ARM_USART_EVENT_TRANSFER_COMPLETE ;

  // send success
  uint32_t send_mask = ARM_USART_EVENT_TRANSFER_COMPLETE |
                       ARM_USART_EVENT_SEND_COMPLETE;

  // if our usart receive was a success
  if(event & recv_mask)
  {
    toggle_gpio(data_led);
    osEventFlagsSet(usart_flag, 0x01);
  }

  // if our usart send / transmit was a success
  if(event & send_mask)
  {
    toggle_gpio(data_led);
    osEventFlagsSet(usart_flag, 0x02);
  }

  //
  // error handling ...
  //

  // rx overflow / tx underflow error
  if(event & (ARM_USART_EVENT_RX_OVERFLOW | ARM_USART_EVENT_TX_UNDERFLOW))
  {
    write_gpio(error_led, HIGH);
    __breakpoint(0);
  }
}

// WORKER THREAD

// usart thread function
void my_usart_thread(void *argument)
{
  // create a packet array to use as a buffer to build up our command string
  uint8_t packet[128];
  int i = 0;

  // initialise our leds
  init_gpio(data_led, OUTPUT);
  init_gpio(error_led, OUTPUT);
  init_gpio(control_led, OUTPUT);

  // usart configuration ...

  // get a pointer to the usart driver object
  static ARM_DRIVER_USART * usart_driver = &Driver_USART1;

  // initialise and configure the usart driver (running at 9600bps)
  usart_driver->Initialize(my_usart_callback);
  usart_driver->PowerControl(ARM_POWER_FULL);
  usart_driver->Control(ARM_USART_MODE_ASYNCHRONOUS |
                        ARM_USART_DATA_BITS_8 |
                        ARM_USART_PARITY_NONE |
                        ARM_USART_STOP_BITS_1 |
                        ARM_USART_FLOW_CONTROL_NONE, 9600);

  // set up rx and tx lines
  usart_driver->Control(ARM_USART_CONTROL_TX, 1);
  usart_driver->Control(ARM_USART_CONTROL_RX, 1);

  // initial welcome message ...

  // print a status message
  usart_driver->Send("we are alive ...\r\n", 19);

  // wait for the usart_flag event flag 0x02 (i.e. we have sent the data
  // successfully)
  osEventFlagsWait(usart_flag, 0x02, osFlagsWaitAny, osWaitForever);

  // main thread logic ...

  // main thread loop
  while(1)
  {
    // read a character from the usart and wait for the recv operation to
    // complete successfully (i.e. we get the usart_event flag 0x01)
    char c;
    usart_driver->Receive(&c, 1);
    osEventFlagsWait(usart_flag, 0x01, osFlagsWaitAny, osWaitForever);

    // if the last character we read was a newline, then process the contents
    // of the packet buffer
    if(c == '\n')
    {
      // add string terminator (we need this - otherwise we will keep reading
      // past the end of the last packet received ...)
      packet[i] = '\0';

      // decide whether to turn the led on or off by doing a string
      // comparison ...
      //
      // note: the strcmp function requires an exact match so if you are
      // using teraterm or putty you will need to set the tx options
      // correctly
      //
      // see: http://www.tutorialspoint.com/c_standard_library/c_function_strcmp.htm
      // for strcmp details ...
      if(strcmp((char*)packet, "on-led1\r") == 0)
      {
        // turn the control led on
        write_gpio(control_led, HIGH);

        // display a status message
        usart_driver->Send("led 1 on\r\n", 11);
        osEventFlagsWait(usart_flag, 0x02, osFlagsWaitAny, osWaitForever);
      }
      if(strcmp((char*)packet, "off-led1\r") == 0)
      {
        // turn the control led on
        write_gpio(control_led, LOW);

        // display a status message
        usart_driver->Send("led 1 off\r\n", 12);
        osEventFlagsWait(usart_flag, 0x02, osFlagsWaitAny, osWaitForever);
      }

      //// print debugging message to the uart
      //char debug_msg[150];
      //sprintf(debug_msg, "DEBUGGING: %s\r\n", packet);
      //usart_driver->Send(debug_msg, strlen(debug_msg));
      //osEventFlagsWait(usart_flag, 0x02, osFlagsWaitAny, osWaitForever);

      // zero the packet index
      i = 0;
    }
    else
    {
      // if there is space left in the buffer, then add the character -
      // otherwise print an error message and reset the packet index to start
      // again
      if(i < 128)
      {
        packet[i] = c;
        i++;
      }
      else
      {
        usart_driver->Send("\r\nPACKET OVERFLOW!\r\n", 21);
        osEventFlagsWait(usart_flag, 0x02, osFlagsWaitAny, osWaitForever);
        i = 0;
      }
    }
  }
}
