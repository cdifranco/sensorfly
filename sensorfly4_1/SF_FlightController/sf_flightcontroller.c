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
#include "sf_flightcontroller.h"

//-----------------------------------------------------------------------------
// Local declarations
//-----------------------------------------------------------------------------
unsigned int task_flightcontroller_stack[TASK_FLIGHTCONTROLLER_STK_SIZE];
TN_TCB  task_flightcontroller;
void task_flightcontroller_func(void * par);

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
/*! \fn void  sf_flightcontroller_task_init()
    \brief Initialize the flight controller task
    \param none
    \return none
*/
void  sf_flightcontroller_task_init()
{

   //--- Task flight controller
   task_flightcontroller.id_task = 0;
   tn_task_create(&task_flightcontroller,            //-- task TCB
                 task_flightcontroller_func,           //-- task function
                 TASK_FLIGHTCONTROLLER_PRIORITY,       //-- task priority
                 &(task_flightcontroller_stack         //-- task stack first addr in memory
                    [TASK_FLIGHTCONTROLLER_STK_SIZE-1]),
                 TASK_FLIGHTCONTROLLER_STK_SIZE,       //-- task stack size (in int,not bytes)
                 NULL,                           //-- task function parameter
                 TN_TASK_START_ON_CREATION     //-- Creation option
                 );

}

/*! \fn void task_flightcontroller_func(void * par)
    \brief The flight controller task logic.
    \param par Void pointer. For future use.
    \return none
*/
void task_flightcontroller_func(void * par)
{
   /* Prevent compiler warning */
   par = par;
   
   while(1)
   {     
      /* Sleep 100 ticks */
      tn_task_sleep(1);
   }
}
