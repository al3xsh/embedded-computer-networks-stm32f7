/*
 * pinmappings.h
 *
 * this is a simplified way of presenting which pins are actually broken out
 * on the stm32f7 discovery board and is provided for the embedded computer 
 * networks module at Sheffield Hallam University.
 *
 * note, the stm32f7 discovery board only breaks out a very limited range of 
 * pins for general use (though there are a load of specialised pins for some
 * of the peripherals - e.g. Ethernet, LCD, etc.)
 *
 * author:  Dr. Alex Shenfield
 * date:    01/09/2017
 *
 */

// define a symbol to prevent recursive inclusion
#ifndef __PINMAP
#define __PINMAP

// include the basic headers for the hal drivers
#include "stm32f7xx_hal.h"

// create an enumeration for all available pins on our board
typedef enum
{
  PA_0  = 0x00,
  //PA_1  = 0x01,
  //PA_2  = 0x02,
  //PA_3  = 0x03,
  //PA_4  = 0x04,
  //PA_5  = 0x05,
  //PA_6  = 0x06,
  //PA_7  = 0x07,
  PA_8  = 0x08,
  PA_9  = 0x09,
  //PA_10 = 0x0A,
  //PA_11 = 0x0B,
  //PA_12 = 0x0C,
  //PA_13 = 0x0D,
  //PA_14 = 0x0E,
  PA_15 = 0x0F,

  //PB_0  = 0x10,
  //PB_1  = 0x11,
  //PB_2  = 0x12,
  //PB_3  = 0x13,
  PB_4  = 0x14,
  //PB_5  = 0x15,
  //PB_6  = 0x16,
  PB_7  = 0x17,
  PB_8  = 0x18,
  PB_9  = 0x19,
  //PB_10 = 0x1A,
  //PB_11 = 0x1B,
  //PB_12 = 0x1C,
  //PB_13 = 0x1D,
  PB_14 = 0x1E,
  PB_15 = 0x1F,

  //PC_0  = 0x20,
  //PC_1  = 0x21,
  //PC_2  = 0x22,
  //PC_3  = 0x23,
  //PC_4  = 0x24,
  //PC_5  = 0x25,
  PC_6  = 0x26,
  PC_7  = 0x27,
  //PC_8  = 0x28,
  //PC_9  = 0x29,
  //PC_10 = 0x2A,
  //PC_11 = 0x2B,
  //PC_12 = 0x2C,
  //PC_13 = 0x2D,
  //PC_14 = 0x2E,
  //PC_15 = 0x2F,

  /*
  PD_0  = 0x30,
  PD_1  = 0x31,
  PD_2  = 0x32,
  PD_3  = 0x33,
  PD_4  = 0x34,
  PD_5  = 0x35,
  PD_6  = 0x36,
  PD_7  = 0x37,
  PD_8  = 0x38,
  PD_9  = 0x39,
  PD_10 = 0x3A,
  PD_11 = 0x3B,
  PD_12 = 0x3C,
  PD_13 = 0x3D,
  PD_14 = 0x3E,
  PD_15 = 0x3F,

  PE_0  = 0x40,
  PE_1  = 0x41,
  PE_2  = 0x42,
  PE_3  = 0x43,
  PE_4  = 0x44,
  PE_5  = 0x45,
  PE_6  = 0x46,
  PE_7  = 0x47,
  PE_8  = 0x48,
  PE_9  = 0x49,
  PE_10 = 0x4A,
  PE_11 = 0x4B,
  PE_12 = 0x4C,
  PE_13 = 0x4D,
  PE_14 = 0x4E,
  PE_15 = 0x4F,
  */

  //PF_0  = 0x50,
  //PF_1  = 0x51,
  //PF_2  = 0x52,
  //PF_3  = 0x53,
  //PF_4  = 0x54,
  //PF_5  = 0x55,
  PF_6  = 0x56,
  PF_7  = 0x57,
  PF_8  = 0x58,
  PF_9  = 0x59,
  PF_10 = 0x5A,
  //PF_11 = 0x5B,
  //PF_12 = 0x5C,
  //PF_13 = 0x5D,
  //PF_14 = 0x5E,
  //PF_15 = 0x5F,
  
  //PG_0  = 0x60,
  //PG_1  = 0x61,
  //PG_2  = 0x62,
  //PG_3  = 0x63,
  //PG_4  = 0x64,
  //PG_5  = 0x65,
  PG_6  = 0x66,
  PG_7  = 0x67,
  //PG_8  = 0x68,
  //PG_9  = 0x69,
  //PG_10 = 0x6A,
  //PG_11 = 0x6B,
  //PG_12 = 0x6C,
  //PG_13 = 0x6D,
  //PG_14 = 0x6E,
  //PG_15 = 0x6F,
  
  //PH_0  = 0x70,
  //PH_1  = 0x71,
  //PH_2  = 0x72,
  //PH_3  = 0x73,
  //PH_4  = 0x74,
  //PH_5  = 0x75,
  PH_6  = 0x76,
  //PH_7  = 0x77,
  //PH_8  = 0x78,
  //PH_9  = 0x79,
  //PH_10 = 0x7A,
  //PH_11 = 0x7B,
  //PH_12 = 0x7C,
  //PH_13 = 0x7D,
  //PH_14 = 0x7H,
  //PH_15 = 0x7F,

  PI_0  = 0x90,
  PI_1  = 0x91,
  PI_2  = 0x92,
  PI_3  = 0x93,
  //PI_4  = 0x94,
  //PI_5  = 0x95,
  //PI_6  = 0x96,
  //PI_7  = 0x97,
  //PI_8  = 0x98,
  //PI_9  = 0x99,
  //PI_10 = 0x9A,
  PI_11 = 0x9B,
  //PI_12 = 0x9C,
  //PI_13 = 0x9D,
  //PI_14 = 0x9E,
  //PI_15 = 0x9F,
  
  // if we get here, the pin is not available
  NC = (int)0xFFFFFFFF
}
pin_name;

// gpio pin structure
typedef struct
{
  pin_name      pin_id;
  GPIO_TypeDef* gpio_port;
  uint16_t      gpio_pin;
}
gpio_pin_t;

#endif
// __PINMAP
