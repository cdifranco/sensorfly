/*============================================================================
  main.c
*===========================================================================*/

#include <stdio.h>
#include <string.h>
#include "../common/LPC214x.h"
#include "../TNKernel/tn.h"
#include "../TNKernel/tn_port.h"

#define LED_SEL 0x800000

//----------- Tasks ----------------------------------------------------------

#define  TASK_PROCESSING_PRIORITY   5
#define  TASK_PROCESSING_STK_SIZE  128
unsigned int task_processing_stack[TASK_PROCESSING_STK_SIZE];
TN_TCB  task_processing;
void task_processing_func(void * par);


//----------------------------------------------------------------------------
int main(void)
{
   hardware_init();

   // Set IO directions
   rIO0DIR |= 0x800000;	
   rIO0CLR |= 0x800000;	
   rIO1DIR |= 0x200000;	
   rIO1CLR |= 0x200000;

   tn_start_system(); //-- Never returns

   return 1;
}

//----------------------------------------------------------------------------
void  tn_app_init()
{

   //--- Task processing

   task_processing.id_task = 0;
   tn_task_create(&task_processing,            //-- task TCB
                 task_processing_func,           //-- task function
                 TASK_PROCESSING_PRIORITY,       //-- task priority
                 &(task_processing_stack         //-- task stack first addr in memory
                    [TASK_PROCESSING_STK_SIZE-1]),
                 TASK_PROCESSING_STK_SIZE,       //-- task stack size (in int,not bytes)
                 NULL,                           //-- task function parameter
                 TN_TASK_START_ON_CREATION     //-- Creation option
                 );

 }
//----------------------------------------------------------------------------
void task_processing_func(void * par)
{
   unsigned short Blink = 1;

   /* Prevent compiler warning */
   par = par;
   
   while(1)
   {
      if (Blink & 1)
      {
          // P0.10 & P0.11 output 
         rIO0CLR = LED_SEL;
      }
      else
      {
         rIO0SET = LED_SEL;
      }   
      
      Blink = Blink ^ 1;
      
      /* Sleep 100 ticks */
      tn_task_sleep(100);
   }
}
//----------------------------------------------------------------------------



