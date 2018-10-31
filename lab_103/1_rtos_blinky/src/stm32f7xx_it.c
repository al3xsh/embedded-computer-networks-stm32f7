/*
 * stm32f7xx_it.c
 * 
 * interrupt handler routines
 *
 * author:    Alex Shenfield
 * date:      06/09/2017
 * purpose:   55-604481 embedded computer networks : lab 103
 */
 
// includes
#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_it.h"

// GENERIC HARD FAULT HANDLER

// forward declarations for hardfault handler utility functions
void printErrorMsg(const char * errMsg);
void printUsageErrorMsg(uint32_t CFSRValue);
void printBusFaultErrorMsg(uint32_t CFSRValue);
void printMemoryManagementErrorMsg(uint32_t CFSRValue);
void stackDump(uint32_t stack[]);

/**
  * @brief  This function handles a Hard Fault exception
  * @param  None
  * @retval None
  */
void Hard_Fault_Handler(uint32_t stack[])
{
  // this is a generic hardfault handler (from an excellent article at
  // https://blog.feabhas.com/2013/02/developing-a-generic-hard-fault-handler-
  // for-arm-cortex-m3cortex-m4/)
  static char msg[80];
  printErrorMsg("In Hard Fault Handler\n");
  sprintf(msg, "SCB->HFSR = 0x%08x\n", SCB->HFSR);
  printErrorMsg(msg);
  if((SCB->HFSR & (1 << 30)) != 0)
  {
    printErrorMsg("Forced Hard Fault\n");
    sprintf(msg, "SCB->CFSR = 0x%08x\n", SCB->CFSR);
    printErrorMsg(msg);
    if((SCB->CFSR & 0xFFFF0000) != 0)
    {
      printUsageErrorMsg(SCB->CFSR);
    }
    if((SCB->CFSR & 0xFF00) != 0)
    {
      printBusFaultErrorMsg(SCB->CFSR);
    }
    if((SCB->CFSR & 0xFF) != 0)
    {
      printMemoryManagementErrorMsg(SCB->CFSR);
    }
  }
  stackDump(stack);
  __ASM volatile("BKPT #01");

  // go to infinite loop when Hard Fault exception occurs
  while(1)
  {
  }
}

// hard fault handler utility functions ...

// print the error message ...
void printErrorMsg(const char * errMsg)
{
  while(*errMsg != '\0')
  {
    ITM_SendChar(*errMsg);
    ++errMsg;
  }
}

// usage error message
void printUsageErrorMsg(uint32_t CFSRValue)
{
  printErrorMsg("Usage fault: ");
  // right shift to lsb
  CFSRValue >>= 16; 

  if((CFSRValue & (1 << 9)) != 0)
  {
    printErrorMsg("Divide by zero\n");
  }
}

// bus fault error message
void printBusFaultErrorMsg(uint32_t CFSRValue)
{
  printErrorMsg("Bus fault: ");
  // mask and right shift to lsb
  CFSRValue = ((CFSRValue & 0x0000FF00) >> 8); 
}

// memory management error message
void printMemoryManagementErrorMsg(uint32_t CFSRValue)
{
  printErrorMsg("Memory Management fault: ");
  // mask just mem faults
  CFSRValue &= 0x000000FF; 
}

// actually implement our hardfault handler
#if defined(__CC_ARM)
__asm void HardFault_Handler(void)
{
  TST lr, #4
  ITE EQ
  MRSEQ r0, MSP
  MRSNE r0, PSP
  B __cpp(Hard_Fault_Handler)
}
#elif defined(__ICCARM__)
void HardFault_Handler(void)
{
  __asm("TST lr, #4");
  __asm("ITE EQ");
  __asm("MRSEQ r0, MSP");
  __asm("MRSNE r0, PSP");
  __asm("B Hard_Fault_Handler");
}
#else
#warning Not supported compiler type
#endif

enum { r0, r1, r2, r3, r12, lr, pc, psr};

// dump the stack so we can see what has happened ...
void stackDump(uint32_t stack[])
{
  static char msg[80];
  sprintf(msg, "r0  = 0x%08x\n", stack[r0]);
  printErrorMsg(msg);
  sprintf(msg, "r1  = 0x%08x\n", stack[r1]);
  printErrorMsg(msg);
  sprintf(msg, "r2  = 0x%08x\n", stack[r2]);
  printErrorMsg(msg);
  sprintf(msg, "r3  = 0x%08x\n", stack[r3]);
  printErrorMsg(msg);
  sprintf(msg, "r12 = 0x%08x\n", stack[r12]);
  printErrorMsg(msg);
  sprintf(msg, "lr  = 0x%08x\n", stack[lr]);
  printErrorMsg(msg);
  sprintf(msg, "pc  = 0x%08x\n", stack[pc]);
  printErrorMsg(msg);
  sprintf(msg, "psr = 0x%08x\n", stack[psr]);
  printErrorMsg(msg);
}

// end of hardfault handler code ...

// OTHER EXCEPTION HANDLERS

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

// these are all implemented in the rtos rtx kernel ...
//
///**
//  * @brief  This function handles SVCall exception.
//  * @param  None
//  * @retval None
//  */
//void SVC_Handler(void)
//{
//}
//
///**
//  * @brief  This function handles PendSVC exception.
//  * @param  None
//  * @retval None
//  */
//void PendSV_Handler(void)
//{
//}

///**
//  * @brief  This function handles SysTick Handler.
//  * @param  None
//  * @retval None
//  */
//void SysTick_Handler(void)
//{
//  // hal interrupt handler addition to handle the SysTick
//  HAL_IncTick();
//  
//  // this HAL_SYSTICK_IRQHandler() function is only needed if we are going to
//  // use the HAL_SYSTICK_Callback() function to do something in this interrupt
//  HAL_SYSTICK_IRQHandler();
//  
//}

/*****************************************************************************/
/*                 STM32F7xx Peripherals Interrupt Handlers                  */
/*****************************************************************************/

