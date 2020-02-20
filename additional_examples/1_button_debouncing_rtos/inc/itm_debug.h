/*
 * itm_debug.h
 *
 * print debugging information to the itm debug viewer in uvision
 *
 * we use this for debugging because it doesn't interfere with the uart output
 * on the virtual com port.  we can have debugging stuff go to the itm viewer 
 * and program output to the uart.
 *
 * author:    Alex Shenfield
 * date:      08/11/2017
 * purpose:   55-604481 embedded computer networks : lab 104
 */
 
// define to prevent recursive inclusion
#ifndef __ITM_DEBUG_H
#define __ITM_DEBUG_H
 
// simple debug helper function 
void print_debug(char* s, int length);
 
#endif // ITM_DEBUG_H
