/*
 * stm32f7xx_it.h
 *
 * header file for the interrupt handler routines
 *
 * author:    Alex Shenfield
 * date:      20/09/2015
 * purpose:   16-6498 embedded computer networks : lab 101
 */

// define to prevent recursive inclusion
#ifndef __STM32F7xx_IT_H
#define __STM32F7xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

// exported functions
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F7xx_IT_H */


