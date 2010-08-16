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


unsigned int task_get_heading_stack[TASK_GET_HEADING_STK_SIZE];
uint16_t current_heading=0;
uint16_t desired_heading=0;
uint32_t az_zero = 0;
uint8_t frzInt = 0;
TN_TCB task_get_heading;

void task_flightcontroller_func(void * par);
void task_get_heading_func(void * par);

uint32_t sf_hover_controller(uint32_t d_val);
uint32_t sf_learning_hover_controller(uint32_t d_val);
int32_t sf_spin_controller(int32_t spin);

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
   uint32_t lastS=0, s = 0, z = 0, ip = 0, c, count = 0,start = 0, x=0;
   uint8_t diff1 = 0, diff2=0;
   int16_t dTime=10;
   int32_t diff = 0;
   unsigned short Blink = 1;
   char * buf = "Hello World\n";
   char msg[16]; 
   /* Prevent compiler warning */
   par = par;

   /*Calibrate gyro for high precision readings. Diego 06072010*/
//     sf_sensor_gyro_auto_zero();
   //az_zero = get_gyro_zero();
   
   while(start<3000){
    start++;
    tn_task_sleep(10);
   }
   az_zero = sf_sensor_gyro_z_get();
   while(1)
   {  
      count++;
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
      lastS = s;
      ip = sf_hover_controller(s);
      if(count==2000){
        diff = sf_heading_controller();
        if(diff<0){
          dTime=-diff;
          diff=-1;
          
        }
        if(diff>0){
          dTime=diff;
          diff=1;
         }
         count=0;
         if(Blink==0){
           sf_led_on();
           Blink=1;
         }

      }//else if(count>dTime){
        diff = sf_spin_controller(z);
      //}
      start++;
      if(start<35000){
         sf_drive_duty_set(((float)ip*1), ((float)ip*1.1)); //(upper blade ccw, lower blade cw)
         //sf_drive_duty_set((ip - diff), (ip + diff)); //(upper blade, lower blade)
         //sf_drive_duty_set(0, 0);
      }else{
        sf_drive_duty_set(0, 0);
        break;
      }
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

	static int32_t err = 0, errLast = 0, sigErr = 0, temp=0;
	static int32_t input = 0;
	static int32_t outP = 0;
        static int32_t count = 0;

	const int32_t kscale = 10000;
	const int32_t setP = 80;
	// scaled k
	const int32_t kp = 6000;
	const int32_t kd = 5000;//2000;
	const int32_t ki = 2;//2;


	// First order filter params
	const int a1 = 1;
	const int b1 = 10;

	input = d_val;
	err = setP - input;

	outP = (kp * err) + (kd * (err - errLast)) + (ki * sigErr);

        if(count==125){
              count=0;
              errLast = err;
         }
        count++;

	//Convert to %
	outP = outP / kscale;

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
int32_t sf_spin_controller(int32_t spin) 
{
	//PID control
	//static uint32_t i = 0;

	static int32_t err = 0, errLast = 0, sigErr = 0;
	static int32_t input = 0, temp=0;
	static int32_t outP = 0;
        const int32_t kscale = 20000;
        int32_t setP = 0;
       	
	// scaled k
	const int32_t kp = 18000;
	const int32_t kd = 10000;
	const int32_t ki = 1;

	input = spin;
        setP=az_zero;
	err = input - setP;
        outP = (kp * err) + (ki * sigErr) + (kd * (err - errLast));
        //errLast = err;

        //Convert to %
        
        outP = outP / kscale;      
        if (outP > 3)
                outP = 3;
        else if (outP < -3)
                outP = -3;
        else if (outP <1 && outP>-1)
              outP=0;
        else
                sigErr+=err;
        return outP;
}

int16_t sf_heading_controller(){
  //PID control
	static uint32_t i = 0;
	static int32_t err = 0, errLast = 0, sigErr = 0;
	static int32_t input = 0;
	static int32_t outP = 0;
        char * buf = "Hello World\n";
	const int32_t kscale = 12000;
        const int32_t setP = desired_heading;
	//const int32_t setP = 0;
	// scaled k
	const int32_t kp = 600;
	const int32_t kd = 0;
	const int32_t ki = 0;
	input = current_heading;
	//err = subtract_angles(setP, input);

	outP = (kp * err) + (ki * sigErr) + (kd * (err - errLast));
	errLast = err;
	//Convert to %
      outP = outP / kscale;
      
	if (outP > 1000)
		outP = 1000;
	else if (outP < -1000)
		outP = -1000;
        else
                sigErr += err;

      return outP;
}


/*! \fn void  sf_get_heading_task_init()
    \brief initializes the get heading task
    \param  none
    \return none
*/
void  sf_get_heading_task_init()
{
   //uint16_t cP=0, cR=0;
   //GetCompassHeading(&current_heading, &cP, &cR);
   desired_heading=1800;//for now we try to fly in a fixed direction.
 
   //--- Task Get Heading 
   task_get_heading.id_task = 0; /*!< Must be 0 for all tasks */
   tn_task_create(&task_get_heading,            //-- task TCB
                 task_get_heading_func,           //-- task function
                 TASK_GET_HEADING_PRIORITY,       //-- task priority
                 &(task_get_heading_stack         //-- task stack first addr in memory
                    [TASK_GET_HEADING_STK_SIZE-1]),
                 TASK_GET_HEADING_STK_SIZE,       //-- task stack size (in int,not bytes)
                 NULL,                           //-- task function parameter
                 TN_TASK_START_ON_CREATION     //-- Creation option
                 );

}

/*! \fn void task_get_heading_func(void * par)
    \brief task that keeps track of current heading
    \param  Void pointer. For future use.
    \return none
*/

void task_get_heading_func(void * par)
{
  uint8_t cE=0, p=0, cS=0, headings_to_average=10;
  uint16_t cH=0, cP=0, cR=0, new_heading=0, margin_of_error=200;
  //char * buf = "Hello World\n";
  // First order filter params
  const int a1 = 1;
  const int b1 = 3;
  while(1)
  {
    while(p<headings_to_average)
    {
      //cE=GetCompassHeading(&cH, &cP, &cR);
      if(cE==0){
        new_heading+=cH;
        p++;
        tn_task_sleep(100);
      }
    }
    new_heading=new_heading/headings_to_average;
    if(current_heading-new_heading<margin_of_error && current_heading-new_heading>-margin_of_error){
      current_heading = new_heading;
       cS=0;
 //       buf=sfitoa(cH, 10);
 //       buf=strncat(buf, "\n", 10);
 //     sf_network_tx_enqueue(buf, strlen(buf));
 //     sf_network_tx_send();
    }else{
       cS++;
       if(cS>2){
          current_heading=new_heading;
          cS=0;
       }
    }
    
    p=0;
    new_heading=0;
  }
}

int32_t get_gyro_zero(){
  uint32_t average=0;
  uint8_t count=0;
  const uint8_t moe=10;
  uint32_t lastReading=0;
  uint32_t currentReading=0;
  while(count<8){
     currentReading=sf_sensor_gyro_z_get();
     if(lastReading-10 < currentReading < lastReading + 10){
        average+=currentReading;
     }else{
        average=currentReading;
        count=0;
     }
     count++;
     lastReading=currentReading;
  }
  average=average/count;
  return average;
}