/*
 * rtos_objects.h
 *
 * this is the header file containing the rtos objects for the rtos mail queue
 * application
 *
 * author:    Dr. Alex Shenfield
 * date:      22/10/2020
 * purpose:   55-604481 embedded computer networks - lab 103
 */

// define to prevent recursive inclusion
#ifndef __RTOS_OBJ_H
#define __RTOS_OBJ_H

// include the header file for basic data types and the cmsis-rtos2 api
#include <stdint.h>
#include "cmsis_os2.h"

// create the objects to use in our rtos applications to pass data

// declare an event flag to use to signal the button has been pressed
extern osEventFlagsId_t button_flag;

#endif // RTOS_OBJ_H
