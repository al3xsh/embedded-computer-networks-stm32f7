/*
 * data_generation_thread.c
 *
 * this is a thread that periodically generates some data and puts it in a 
 * mail queue
 *
 * author:    Dr. Alex Shenfield
 * date:      06/09/2018
 * purpose:   55-604481 embedded computer networks : lab 103
 */
 
// include cmsis_os for the rtos api
#include "cmsis_os.h"

// include main.h (this is where we initialise the mail type and queue)
#include "main.h"

// include the shu bsp libraries for the stm32f7 discovery board
#include "pinmappings.h"
#include "clock.h"
#include "random_numbers.h"
#include "gpio.h"

// RTOS DEFINES

// declare the thread function prototypes, thread id, and priority
void data_thread(void const *argument);
osThreadId tid_data_thread;
osThreadDef(data_thread, osPriorityNormal, 1, 0);

// set up the mail queue
osMailQDef(mail_box, 16, mail_t);
osMailQId  mail_box;

// HARDWARE DEFINES

// led is on PI 1 (this is the inbuilt led)
gpio_pin_t led1 = {PI_1, GPIOI, GPIO_PIN_1};

// THREAD INITIALISATION

// create the data generation thread
int init_data_thread(void)
{
  // initialize peripherals (i.e. the led and random number generator) here
  init_gpio(led1, OUTPUT);
  init_random();
  
  // create the mailbox
  mail_box = osMailCreate(osMailQ(mail_box), NULL);
  
  // create the thread and get its task id
  tid_data_thread = osThreadCreate(osThread(data_thread), NULL);
  
  // check if everything worked ...
  if(!tid_data_thread)
  {
    return(-1);
  }
  return(0);
}

// ACTUAL THREADS

// data generation thread - create some random data and stuff it in a mail 
// queue
void data_thread(void const *argument)
{
  // set up our counter
  uint32_t i = 0;
  
  // infinite loop generating our fake data (one set of samples per second)
  // we also toggle the led so we can see what is going on ...
  while(1)
  {
    // create our mail (i.e. the message container)   
    mail_t* mail = (mail_t*) osMailAlloc(mail_box, osWaitForever);    
    
    // get a random number 
    float random = get_random_float();
    
    // toggle led
    toggle_gpio(led1);
    
    // generate our fake data
    i++;
    mail->counter = i;
    mail->current = (1.0f / (random * i)); 
    mail->voltage = (5.0f / (random * i));
    
    // put the data in the mail box and wait for one second
    osMailPut(mail_box, mail);
    osDelay(1000);
  }
} 
