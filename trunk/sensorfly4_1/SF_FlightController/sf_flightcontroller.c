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

uint32_t sf_hover_controller(uint32_t d_val);
int32_t sf_spin_controller(uint32_t spin);

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
   // Initialize sensors and drives
   sf_sensor_sonic_init();
   sf_sensor_gyro_init();
   sf_drive_init();

   //--- Task flight controller
   task_flightcontroller.id_task = 0; /*!< Must be 0 for all tasks */
   /*tn_task_create(&task_flightcontroller,            //-- task TCB
                 task_flightcontroller_func,           //-- task function
                 TASK_FLIGHTCONTROLLER_PRIORITY,       //-- task priority
                 &(task_flightcontroller_stack         //-- task stack first addr in memory
                    [TASK_FLIGHTCONTROLLER_STK_SIZE-1]),
                 TASK_FLIGHTCONTROLLER_STK_SIZE,       //-- task stack size (in int,not bytes)
                 NULL,                           //-- task function parameter
                 TN_TASK_START_ON_CREATION     //-- Creation option
                 );*/

}

/*! \fn void task_flightcontroller_func(void * par)
    \brief The flight controller task logic.
    \param par Void pointer. For future use.
    \return none
*/
void task_flightcontroller_func(void * par)
{
   uint32_t s = 0, z = 0, ip = 0, diff = 0, c;
   
   /* Prevent compiler warning */
   par = par;
   
   while(1)
   {     

      // First order filter params
      const int a1 = 1;
      const int b1 = 3;

      static uint32_t sonicLast = 0;

      // Get sensor readings
      s = sf_sensor_sonic_get();
      z = sf_sensor_gyro_z_get();

      // Filter sonic readings
      s = ((a1 * s) + (b1 * sonicLast))/(a1 + b1);
      sonicLast = s;

      ip = sf_hover_controller(s);
      //diff = sf_spin_controller(z);

      sf_drive_duty_set(ip - diff, ip + diff);

      /* Sleep 10 ticks */
      tn_task_sleep(10);
   }
}

/*! \fn uint32_t sf_hover_controller(uint32_t d_val)
    \brief PID Controller for altitude control
    \param d_val The altitude - sonic sensor reading
    \return The drive duty cycle
*/
uint32_t sf_hover_controller(uint32_t d_val) {

	//PID control

	static uint32_t i = 0;

	static int32_t err = 0, errLast = 0, sigErr = 0;
	static int32_t input = 0;
	static int32_t outP = 0;

	const int32_t kscale = 10000;
	const int32_t setP = 80;
	// scaled k
	const int32_t kp = 2000;
	const int32_t kd = 2000;
	const int32_t ki = 2;


	// First order filter params
	const int a1 = 1;
	const int b1 = 10;

	input = d_val;
	err = setP - input;

	outP = (kp * err) + (kd * (err - errLast)) + (ki * sigErr);
	errLast = err;

	//Convert to %
	outP = outP / 10000;

	if (outP > 90)
		outP = 90;
	else if (outP < 10)
		outP = 10;
	else
		sigErr += err;
	return outP;
}

/*! \fn int32_t sf_spin_controller(uint32_t spin) 
    \brief PID controller for spin/attitude control
    \param spin The z axis velocity - Gyro reading
    \return 1/2 of the offset between CW and CCW drives
*/
int32_t sf_spin_controller(uint32_t spin) 
{
	//PID control
	static uint32_t i = 0;

	static int32_t err = 0, errLast = 0, sigErr = 0;
	static int32_t input = 0;
	static int32_t outP = 0;

	const int32_t kscale = 10000;
	const int32_t setP = 0;
	// scaled k
	const int32_t kp = 2000;
	const int32_t kd = 2000;
	const int32_t ki = 2;

	input = spin;
	err = setP - input;

	outP = (kd * (err - errLast));
	errLast = err;

	//Convert to %
	outP = outP / 10000;

	if (outP > 5)
		outP = 0;
	else if (outP < -5)
		outP = -5;
	else
		sigErr += err;

	return outP;
}