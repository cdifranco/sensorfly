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
#define  TASK_APP_STK_SIZE  256

// Pin 0.21
#define CTS_MASK (1<<21)
// Pin 0.20
#define RTS_MASK (1<<20)
// Source Address
#define SRC_ADDR 1

// #define ANCHOR 1
//#define NODE 1
#define BASE 1


unsigned int task_app_stack[TASK_APP_STK_SIZE];
TN_TCB  task_app;
void task_app_func(void * par);
int state = 0;
extern UARTDRV drvUART0;
extern TN_EVENT eventReset;
extern uint32_t eventPattern;
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

#ifndef BASE
   hardware_init();
   tn_start_system(); //-- Never returns
   return 1;
#endif

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
    /* Sleep 2000 ticks */
    tn_task_sleep(4000);
    unsigned short Blink = 1;
    Packet pkt_reset;
    // para: Packet *pkt, uint8_t id, uint8_t type, uint8_t checksum, uint8_t dest, uint8_t src
    sf_network_pkt_gen(&pkt_reset, 1, PKT_TYPE_SETTING, 0, 0, SRC_ADDR);
    sf_network_pkt_send(&pkt_reset); 

    /* Prevent compiler warning */
    par = par;
    int flag = 1;
    while(1)
    {
        // check if AVR restarted
        int reset;
        reset = tn_event_wait_polling(&eventReset,0x00000001,TN_EVENT_WCOND_OR,&eventPattern);
        if (reset == TERR_NO_ERR)
        {
            sf_network_pkt_send(&pkt_reset); 
            tn_event_clear(&eventReset,0x00000000);
        }

        if (Blink & 1)
        {
           sf_led_on();
        }
        else
        {
           sf_led_off();
        }      
        Blink = Blink ^ 1;

#ifdef ANCHOR
      // waiting for command
      Packet * pkt_rx = sf_network_pkt_receive();

      Packet pkt;
      uint8_t ranging_dest = (uint8_t)pkt_rx->data_int[0];
      Packet * pkt_ranging_result = NULL;
      // execute command     
      while (1)
      {
          // para: Packet *pkt, uint8_t id, uint8_t type, uint8_t checksum, uint8_t dest, uint8_t src
          sf_network_pkt_gen(&pkt, 12, PKT_TYPE_RANGING, 0, ranging_dest, SRC_ADDR);
          // send ranging packet
          // TODO: add time out part
          sf_network_pkt_send(&pkt);       
          pkt_ranging_result = sf_network_pkt_receive();
          if (pkt_ranging_result == NULL)
          {
              continue;
          }
          else if (pkt_ranging_result->data_double[1] != 0)
          {
              continue;
          }
          else
          {
              break; 
          }
      }
      // get ranging result packet
      pkt_ranging_result->src = SRC_ADDR;
      pkt_ranging_result->dest = 1;
      sf_network_pkt_send(pkt_ranging_result);       
  
#endif

#ifdef NODE
      Packet * pkt_rx = sf_network_pkt_receive();

      Packet pkt;
      // check if it is the right order
      // get compass reading 
      uint16_t heading;
      uint16_t pitch; 
      uint16_t roll;
      while((GetCompassHeading(&heading, &pitch, &roll)));
      // generate packet and send back
      // para: Packet *pkt, uint8_t id, uint8_t type, uint8_t checksum, uint8_t dest, uint8_t src
      sf_network_pkt_gen(&pkt, 12, PKT_TYPE_DATA, 0, 1, SRC_ADDR);
      // put data into packet
      pkt.data_int[0] = heading;
      pkt.data_int[1] = pitch;
      pkt.data_int[2] = roll;
      sf_network_pkt_send(&pkt);
#endif
      /* Sleep 2000 ticks */
      tn_task_sleep(2000);

   }
}

