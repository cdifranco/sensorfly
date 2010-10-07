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
#include "../common/sensorfly.h"
#include "../SF_Network/sf_network.h"
#include "../SF_FlightController/sf_flightcontroller.h"

//-----------------------------------------------------------------------------
// Local declarations
//-----------------------------------------------------------------------------
#define  TASK_APP_PRIORITY   5
#define  TASK_APP_STK_SIZE  256 //changed...not suppose to...

unsigned int task_app_stack[TASK_APP_STK_SIZE];
TN_TCB  task_app;
void task_app_func(void * par);
extern UARTDRV drvUART0;
int __pkt_rx_flag;
int state;
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

   sf_fly_command = 'f';

   hardware_init();

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
    sf_network_init();

   //--- Task application
   task_app.id_task = 0; /*!< Must be 0 for all tasks */
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
    Packet pkt;

    unsigned short Blink = 1;
    
    /* Prevent compiler warning */
    par = par;
    while(1)
    {
        if (Blink & 1)
        {
           sf_led_on();
        }
        else
        {
           sf_led_off();
        }   
        
        Blink = Blink ^ 1;
        
      //receive from ARM    
      sf_network_pkt_receive();
//      Packet * receivebuf = (Packet *)drvUART0.buf;
//      char * test =(char *)drvUART0.buf;
      
      
      
//      // Create packet
//      pkt.id = 1;
//      pkt.type = PKT_TYPE_DATA;
//      pkt.checksum = 0;
//      pkt.src = 1;
//      pkt.dest = 2;
//      pkt.length = 8;
//      pkt.data[0] = 'x';
//      pkt.data[1] = 'y';
//      
//       
//      // Send pkt
//      sf_network_pkt_send(&pkt);
      
      /* Sleep 1000 ticks */
      tn_task_sleep(5000);
   }
}

