/* 
 * lcd_thread.c
 *
 * file to create two simple threads - one to blink an led, and one to display
 * a message on the lcd
 *
 * author:    Alex Shenfield
 * date:      06/09/2018
 * purpose:   55-604481 embedded computer networks : lab 103
 */

// include the basic headers for the hal drivers and cmsis rtos
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "gpio.h"

// include the lcd bsp
#include "stm32746g_discovery_lcd.h"

// HARDWARE DEFINES

// the built in led is on PI1
gpio_pin_t led1 = {PI_1, GPIOI, GPIO_PIN_1};

// LCD DEFINES

// define a simple hello world message
#define BOARDER     "****************************"

const char * message[5] = 
{
  "*     Hello LCD World!     *",
  "*     This is an RTOS!     *",
  "*     Who thought this     *",
  "*     would actually       *",
  "*     work?!               *"
};

// RTOS DEFINES

// declare the thread function prototypes
void led_thread(void const *argument);
void msg_thread(void const *argument);

// declare the thread ids
osThreadId tid_led_thread;
osThreadId tid_lcd_thread;

// define thread priorities
osThreadDef(led_thread, osPriorityNormal, 1, 0);
osThreadDef(msg_thread, osPriorityNormal, 1, 0);

// THREAD INITIALISATION

// create the threads
int init_lcd_and_led_threads(void)
{
  // initialize peripherals (i.e. the led) here
  init_gpio(led1, OUTPUT);
    
  // initialise the lcd
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, SDRAM_DEVICE_ADDR);
  BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);

  // set the background colour to blue and clear the lcd
  BSP_LCD_SetBackColor(LCD_COLOR_BLUE);
  BSP_LCD_Clear(LCD_COLOR_BLUE);
  
  // set the font to use
  BSP_LCD_SetFont(&Font24);
  
  // create the threads and get task ids
  tid_led_thread = osThreadCreate(osThread(led_thread), NULL);
  tid_lcd_thread = osThreadCreate(osThread(msg_thread), NULL);
  
  // check if everything worked ...
  if(!(tid_led_thread && tid_lcd_thread))
  {
    return(-1);
  }
  return(0);
}

// ACTUAL THREADS

// blink led (just so we know everything is alive!)
void led_thread(void const *argument)
{
  while(1)
  {
    // toggle the first led on the gpio pin
    toggle_gpio(led1);
    osDelay(1000);
  }
}

// put lcd message up
void msg_thread(void const *argument)
{
  int counter = 0;
  while(1)
  {
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAtLine(0, (uint8_t *)BOARDER);
    BSP_LCD_DisplayStringAtLine(1, (uint8_t *)message[counter++ % 5]);
    BSP_LCD_DisplayStringAtLine(2, (uint8_t *)BOARDER);     
    
    osDelay(2000);
  }
}


