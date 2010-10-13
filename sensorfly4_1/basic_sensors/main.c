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

// Pin 0.21
#define CTS_MASK (1<<21)
// Pin 0.20
#define RTS_MASK (1<<20)

#define SENDER 1
//#define RECEIVER 1

unsigned int task_app_stack[TASK_APP_STK_SIZE];
TN_TCB  task_app;
void task_app_func(void * par);
extern UARTDRV drvUART0;
int state;
int counter = 0;
float pkt_rx_percent = 0;
int total_pkt = 0;
int pkt_id = 101;
int receive_pkt_num = 0;
int rx_flag = 0;
//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

/*! \fn int main(void)
    \brief Main Function. Initializes hardware, starts kernel.
    \param none
    \return none
*/
int main(void){

   //sf_fly_command = 'f';

   hardware_init();
   sf_network_init();
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
    sf_flightcontroller_task_init();

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

    unsigned short Blink = 1;
    Packet pkt;


    pkt.id = 0;
    pkt.type = PKT_TYPE_SETTING;
    pkt.checksum = 7;
    pkt.src = 8;
    pkt.data[0] = 'x';
    pkt.data[1] = 'y';
    sf_network_pkt_send(&pkt); 


    int i;
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
#ifdef RECEIVER
      Packet * pkt_rx = sf_network_pkt_receive();
     
      /** calculate the rate of packet received*/      
      if (pkt_rx != NULL)
      {
          // get percentage of the packet received
          if (pkt_rx->id != pkt_id) 
          {
             receive_pkt_num++;
             rx_flag++; 
             pkt_id = pkt_rx->id;
          }
          pkt_rx_percent =  receive_pkt_num/1000.00;
           sf_network_pkt_release();
      }
#endif
      /** change the pkt and resend it*/
//      uint8_t temp = pkt_rx->src;
//      pkt_rx->src = pkt_rx->dest;
//      pkt_rx->dest = temp;
//      pkt_rx->data[0] = 'a';
//      pkt_rx->type = 'd';
//      sf_network_pkt_send(pkt_rx);    
//      sf_network_pkt_release();

      /** test to send the pkt back*/
      /** create a packet to store the received packet*/
#ifdef SENDER
      /** Create packet for sending test*/
      pkt.id = counter;
      pkt.type = PKT_TYPE_DATA;
      pkt.checksum = 0;
      pkt.src = 11;
      pkt.dest = 22;
      pkt.data[0] = 'x';
      pkt.data[1] = 'y';
      pkt.data[2] = '\0';
      pkt.length = sizeof(Packet); 
      if (total_pkt<1000)
      {
          sf_network_pkt_send(&pkt); 
      }
      total_pkt++;
      counter++;
      if (counter == 100) counter = 0;

#endif

      /* Sleep 5000 ticks */
      tn_task_sleep(2000);

   }
}

