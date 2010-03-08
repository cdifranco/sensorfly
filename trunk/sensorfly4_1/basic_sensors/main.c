/******************************************************************************
*  SensorFly, a controlled-mobile sensor networking platform.
*  Copyright (C) 2010, Carnegie Mellon University
*  All rights reserved.
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, version 2.0 of the License.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "../common/sensorfly.h"
#include "../SF_FlightController/sf_flightcontroller.h"

#define LED_SEL 0x800000 

//-----------------------------------------------------------------------------
// Local declarations
//-----------------------------------------------------------------------------
#define  TASK_APP_PRIORITY   5
#define  TASK_APP_STK_SIZE  128

unsigned int task_app_stack[TASK_APP_STK_SIZE];
TN_TCB  task_app;
void task_app_func(void * par);

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

/*! \fn int main(void)
    \brief Main Function. Initializes hardware, starts kernel.
    \param none
    \return none
*/
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

/*! \fn void  tn_app_init()
    \brief Initialize all tasks for the application
    \param none
    \return none
*/
void  tn_app_init()
{
   //--- Task flight APP
   sf_flightcontroller_task_init();

   //--- Task application
   task_app.id_task = 1;
   tn_task_create(&task_app,            //-- task TCB
                 task_app_func,           //-- task function
                 TASK_APP_PRIORITY,       //-- task priority
                 &(task_app_stack         //-- task stack first addr in memory
                    [TASK_APP_STK_SIZE-1]),
                 TASK_APP_STK_SIZE,       //-- task stack size (in int,not bytes)
                 NULL,                           //-- task function parameter
                 TN_TASK_START_ON_CREATION     //-- Creation option
                 );
 }

/*! \fn void task_app_func(void * par)
    \brief The application task logic 
    \param par Void pointer
    \return none
*/
void task_app_func(void * par)
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
