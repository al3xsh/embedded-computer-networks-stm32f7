/*
 * main.c
 *
 * this is the main simple touch screen application example
 *
 * author:    Dr. Alex Shenfield
 * date:      05/10/2021
 * purpose:   55-604481 embedded computer networks : lab 102
 */

// include the basic headers and hal drivers
#include "stm32f7xx_hal.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"

#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"

// LCD DEFINES

// define a message boarder (note the lcd is 28 characters wide using Font24)
#define BOARDER     "****************************"

// specify a welcome message
const char * welcome_message[2] =
{
  "*     Hello LCD World!     *",
  "*      Welcome to SHU      *"
};

// CODE

// this is the main method
int main()
{
  // we need to initialise the hal library and set up the SystemCoreClock
  // properly
  HAL_Init();
  init_sysclk_216MHz();

  // initialise the lcd
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

  // set the background colour to black and clear the lcd
  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
  BSP_LCD_Clear(LCD_COLOR_BLACK);

  // draw a blue button
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_FillRect(50, 110, 145, 52);

  // write text on to it
  BSP_LCD_SetFont(&Font20);
  BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
  BSP_LCD_DisplayStringAt(60, 128, (uint8_t *)"Press me!", LEFT_MODE);

  // initialise a toggle state variable for our virtual led
  uint8_t led = 0;

  // draw our virtual led
  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
  BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
  BSP_LCD_DrawCircle(350, 136, 26);

  // initialise the touch screen state structure to store information about
  // the touches
  TS_StateTypeDef TS_State;
  BSP_TS_Init(480, 272);

  // main loop
  while(1)
  {
    // get the state of the touch screen
    BSP_TS_GetState(&TS_State);

    // if a touch is detected ...
    if(TS_State.touchDetected)
    {
      // work out if any of the touches are in the area of the button
      uint8_t idx;
      for(idx = 0; idx < TS_State.touchDetected; idx++)
      {
        uint16_t x = TS_State.touchX[idx];
        uint16_t y = TS_State.touchY[idx];
        if(x > 50 && x < 195 && y > 110 && y < 162)
        {
          led = !led;
        }
      }

      // update the "led"
      if(led)
      {
        // draw a filled "led"
        BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
        BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
        BSP_LCD_FillCircle(350, 136, 26);
      }
      else
      {
        // clear the led
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_FillCircle(350, 136, 26);

        // draw an empty "led"
        BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
        BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
        BSP_LCD_DrawCircle(350, 136, 26);
      }

      // dumb ass delay rather than debouncing because i'm lazy :)
      HAL_Delay(500);
    }

  }
}
