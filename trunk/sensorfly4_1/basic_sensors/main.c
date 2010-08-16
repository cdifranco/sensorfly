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
#define NDEBUG
#include <assert.h>
#include "../common/sensorfly.h"
#include "../SF_Network/sf_network.h"

//-----------------------------------------------------------------------------
// Local declarations
//-----------------------------------------------------------------------------
#define  TASK_APP_PRIORITY   5
#define  TASK_APP_STK_SIZE  128
#define  MAX_TRACES 32

unsigned int task_app_stack[TASK_APP_STK_SIZE];
TN_TCB  task_app;
void task_app_func(void * par);
char ** start_of_trace_filenames      = 0; // used by trace() , useful for debugging interrupts
int *   start_of_trace_lines          = 0; // used by trace() , useful for debugging interrupts
char ** start_of_trace_checkpoint_msg = 0; // used by trace() , useful for debugging interrupts

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

void
__assert(const char *error, const char *file, int line)
{
  while (1);
}

char* sfitoa(int val, int base){
	static char buf[32] = {0};
	if (0 == val) {
		buf[31] = '0';
		buf[30] = 0;
	}
	else
		buf[31] = 0;
	int i = 30;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return &buf[i+1];
}

/*! \fn char * trace_str (char * filename, int line, char * checkpoint_msg)
    \brief For debugging purposes, enables leaving a crude trace of checkpoints when in an interrupt or other contexts where using printf would be too slow.
    to read the trace, watch the first MAX_TRACES values of globals last_traces_filename, last_traces_line, and last_traces_msg
    MAX_TRACES has to be a power of 2
    \param none
    \return none
*/
void trace (char * filename, int line, char * checkpoint_msg) {
    static char * last_traces_filename       [MAX_TRACES + MAX_TRACES] = {0};
    static int    last_traces_line           [MAX_TRACES + MAX_TRACES] = {0};
    static char * last_traces_checkpoint_msg [MAX_TRACES + MAX_TRACES] = {0};
    static int next_msg_idx = 0;
    int idx_high = 2 * MAX_TRACES - 1 - next_msg_idx;
    int idx_low  =     MAX_TRACES - 1 - next_msg_idx;
    last_traces_checkpoint_msg  [idx_low] = checkpoint_msg;
    last_traces_filename        [idx_low] = filename;
    last_traces_line            [idx_low] = line;
    start_of_trace_checkpoint_msg = last_traces_checkpoint_msg + MAX_TRACES - next_msg_idx - 1;
    start_of_trace_filenames      = last_traces_filename       + MAX_TRACES - next_msg_idx - 1;
    start_of_trace_lines          = last_traces_line           + MAX_TRACES - next_msg_idx - 1;
    last_traces_checkpoint_msg  [idx_high] = checkpoint_msg;
    last_traces_filename        [idx_high] = filename;
    last_traces_line            [idx_high] = line;
    next_msg_idx ++;
    next_msg_idx &= MAX_TRACES - 1;
}

/*! \fn int main(void)
    \brief Main Function. Initializes hardware, starts kernel.
    \param none
    \return none
*/
int main(void) {

   trace(__FILE__,__LINE__,"enter main");

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

   //--- Flight Controller task
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
   trace(__FILE__,__LINE__,"enter task_app_func");
   unsigned short Blink = 1;
   Write_Sf_packet_state ret;

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

      //The code below this line is just testing the radio and left as an example, but it is not critical and can be replaced by any code.

      static char first_time = 1;
      if (first_time == 1)
        {
            ret = sf_network_set_mac_address( 0 );
            assert (acknowledged == ret);
            ret = sf_network_set_destination_mac_address( 1 );
            assert (acknowledged == ret);
            first_time = 0;
        }

      //char * sendstr = "000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1";
      //char * sendstr =   "1---2---3---4---5---6---7---8---9--10--11--12--13--14--15--16--17--18--19--20--21--22--23--24--25--26--27--28--29--30";
      char *sendstr = "abcd";

      ret = sf_network_set_mac_address( 7 );
      tn_task_sleep(5000);
      ret = sf_network_set_destination_mac_address( 2 );
      tn_task_sleep(5000);
      ret = sf_network_send_range_request();
      tn_task_sleep(5000);

      //ret = sf_network_put_radio_in_send_mode ( strlen(sendstr) );
      //sf_network_send_through_radio ( sendstr, strlen(sendstr) ); // this will always return acknowledged

      //for keeping some statistics about how many commands to the radio were successful
      static int good = 0;
      static int bad = 0;
      if (acknowledged == ret)
        good++;
      else
        bad++;
   }
}
