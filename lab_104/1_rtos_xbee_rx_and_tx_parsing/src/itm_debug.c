/*
 * itm_debug.c
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

#include "stm32f7xx_hal.h"

// define the ITM ports
#define ITM_Port8(n)	(*((volatile uint8_t *)(0xE0000000+4*n)))

// define ITM trace enable register (TER) and trace control register (TCR)
#define ITM_TER       (*((volatile uint32_t *)0xE0000E00))
#define ITM_TCR       (*((volatile uint32_t *)0xE0000E80))

// debugging output
void print_debug(char* s, int length)
{
	// check the itm port is enabled ...
	if((ITM_TCR & ITM_TCR_ITMENA_Msk) && (ITM_TER & (1UL << 0)))
	{
		// iterate over the length of the string character by character
		int i = 0;
		for(i = 0; i < length; i++)
		{
			// put the character on the ITM viewer
			while(ITM_Port8(0) == 0);
			ITM_Port8(0) = (uint8_t)(s[i]);
		}
		
		// do a new line and carriage return
		while(ITM_Port8(0) == 0);
		ITM_Port8(0) = 0x0D;
		while(ITM_Port8(0) == 0);
		ITM_Port8(0) = 0x0A;
	}
}
