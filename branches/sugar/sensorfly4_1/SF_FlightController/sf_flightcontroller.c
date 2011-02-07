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
#include <math.h>
#include "../common/sensorfly.h"
#include "sf_flightcontroller.h"
//#include "fuzzy_control.h"

//-----------------------------------------------------------------------------
// Local declarations
//-----------------------------------------------------------------------------
unsigned int task_flightcontroller_stack[TASK_FLIGHTCONTROLLER_STK_SIZE];
TN_TCB  task_flightcontroller;
int16_t flightData[10];

//Events
extern TN_EVENT flightControllerEventFlag;
uint32_t flightControllerEvent;

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
	uint32_t s = 0, z = 0, c, timer = 0, flightTime, hopWaitTime = 0;
	int16_t ax, ay, az,topRot = 0, botRot = 0, diff = 0, heading = 0, pitch = 0,roll = 0;
	uint8_t cwLoad = 0, ccwLoad = 0;
	uint32_t sonicLast = 0, gyroLast = 0, setP = 0;
	
	uint16_t gyroFilt[10] = {0,0,0,0,0,0,0,0,0,0};//0,0,0,0,0,0,0,0,0,0}; 
	uint8_t rw = 0, numHops =0, ip = 0;
	
	uint32_t hopTime = 0, downTime =0;
	
	// First order filter params
	const int a1 = 1;
	const int b1 = 3;
	
	/* flight time interval */
	par = par;
	flightTime = FLIGHTTIME*100;
	
	/*Zeroing of motors, gyro reading to get setpoint*/
	sf_drive_duty_set(0, 0);
	tn_task_sleep(40000);
	setP =  sf_sensor_gyro_z_get();
	//if (setP <= 416) setP = 416;
	//set_spin_zero(sf_sensor_gyro_z_get());
	//setHeadingRefreshRateTo10Hz_itsTheMaximum();
	
	
	while(1)
	{  
		//wait on flightControllerEvent. occurs @ 100Hz. Governed by timer1 in sf_user.c
		rw = tn_event_wait(&flightControllerEventFlag,0x01,TN_EVENT_WCOND_OR,&flightControllerEvent,TN_WAIT_INFINITE);
		
		if (rw == TERR_NO_ERR)
		{
			if(timer > flightTime) //fly for FLIGHTTIME sec
			{
				sf_drive_duty_set(0,0);
				break;
			}
			
			//Get sensor readings
			s = sf_sensor_sonic_get();
			z = sf_sensor_gyro_z_get();
			getAcceleration(&ax, &ay, &az);
			
			
			/* Filter sonic readings*/
			//s = ((a1 * s) + (b1 * sonicLast))/(a1 + b1);
			//sonicLast = s;
			
			/*Get compass heading(at <= 5Hz:- HMC6343 manual pg 13)*/
			if(timer % 20 == 0) 
			{ 
				GetCompassHeading(&heading, &pitch, &roll);//get heading at 5Hz
				heading /=10;
				ip = 70 + sf_hover_controller(sf_sensor_sonic_to_inches(s),timer,40); // height = 40 inches
				//ip = sf_hop(flightTime,sf_sensor_sonic_to_inches(s),timer,&setP);
			}
			
			/* PID control for spin. Hop control*/                                
			if (timer % 10 == 0) //10Hz spin control.
			{ 
				diff = sf_spin_controller(z, setP, gyroFilt);
			}
			
			topRot = ip - diff; // - sf_spin_controller(z, setP, gyroFilt); //74- ccwLoad;
			botRot = ip + diff; // + sf_spin_controller(z, setP, gyroFilt); //74+  cwLoad;
			
			if(topRot <= 0||botRot <= 0)
			{
				sf_drive_duty_set(0,0);
				topRot = 0;
				botRot = 0;
			}
			else
			{
				sf_drive_duty_set(topRot, botRot);
			}
			
			
			/*Flight data Recorder*/
			flightData[0] = (uint16_t)sf_sensor_sonic_to_inches(s);
			flightData[1] = (uint16_t)z;
			flightData[2] = topRot;
			flightData[3] = botRot;
			flightData[4] = setP;
			flightData[5] = ip;
			flightData[6] = az;
			flightData[7] = timer;
			flightData[8] = heading;
			
			timer++;
			
			/*clear the event flag once flight controller has executed*/
			tn_event_clear(&flightControllerEventFlag,0x00);
		}//end if no ERR
	}//end while
}//task_flightcontroller_func

int32_t sf_spin_controller(uint32_t spinRate, uint32_t setPo, uint16_t *gyroFilt) 
{
	//PID control
	static int32_t err = 0, errLast = 0, sigErr = 0;
	int32_t outP = 0;
	const int8_t LIM = 10;
	
	
	int32_t kscale = 10000;
	
	// scaled k
	int32_t kp = 20300; //1.08 kp =1.2 ki = 0.005 outP limits +-5
	int32_t ki = 10;//45; //int 32_t 0.0045
	
	spinRate = push(gyroFilt, spinRate); //performs a len(gyroFilt) value average filter
	err = spinRate - setPo;
	
	//dead band to avoid overcompensation
	
	if(abs(err) <= 2)err = 0; 
	
	outP = (kp * err) + (ki*sigErr);
	outP = outP/kscale;
	
	//bound on the result 
	
	if (outP > LIM) //CW spin
		outP = LIM;
	else if (outP < -LIM)//CCW spin -3
		outP = -LIM;
	else
        sigErr += err;
	
	return outP;
}

uint16_t push(uint16_t *gyroFilt, uint16_t gyro)
{
	static uint16_t pos = 0;
	uint8_t numVals = 10;
	static uint8_t gyroSet = 0;
	static uint32_t sum = 0;
	
	if( (pos < numVals-1) && (gyroSet == 0) )
    {
		sum += gyro;
		gyro = sum/(sum + 1);
		*(gyroFilt + pos) = gyro;
		pos = (pos + 1) % numVals;
    }
	else
    {
		gyroSet = 1; //after initial round of collections                              
		sum += gyro;
		gyro = sum/numVals; //average numVals previous values
		*(gyroFilt + pos) = gyro;
		pos = (pos + 1) % numVals;
		sum -= *(gyroFilt + pos);
    }
    return gyro;
}

int32_t sf_hover_controller(uint32_t d_val, uint32_t timer, int32_t setP)
{
	static uint32_t i = 0;
	static int32_t err = 0, errLast = 0, sigErr = 0;
	int32_t outP = 0;
	
	
	//const int32_t setP = 40; // inches
	
	// scaled k
	int32_t scale = 10000;
	int32_t kp = 5400; //2
	int32_t kd = 0;
	int32_t ki = 0; //0.8
	
	//input = d_val;
	err = setP - d_val;
	
	
	//Dead Band
	if(abs(err)<= 3)err = 0; //accept accuracy of upto 2 inches
	
	outP = (kp * err) + (ki * sigErr) + (kd*(err-errLast));
	outP = outP/scale;
	
	errLast = err;
	
	// Bound the output upper end
	if (outP >= 20) //25 
	{
		outP = 20;
	}
	else if (outP < -15) //-5
	{
		outP = -15;
	}
	else 
	{
		sigErr += err;
	}
	
	return outP;
}

uint8_t sf_hop(uint32_t flighttime,uint32_t d_val_inches, uint32_t timer, uint32_t *setP)// power lower bound = 70%
{
	static int state = 0;
	static uint32_t timerStart = 0;
	static uint8_t power = 0;
	uint8_t low = 70;
	uint32_t zeroSpin = 418, flightTime = 0; 
	uint32_t numHops = 3;
	
	float climbDescRatio = 1/3.0;
	static uint32_t climbTimeSec = 0;
	static uint32_t descendTimeSec = 0;
	static uint32_t deadTime = 0; 
	
	if (state == 0) //set up hop params
	{
		flightTime = flighttime/numHops;
		climbTimeSec = (climbDescRatio)*flightTime;
		descendTimeSec = (climbDescRatio)*flightTime;
		deadTime = (climbDescRatio)*flightTime;
		timerStart = timer; //reference clock
	}
	//set up climber
	if ( timer <= (timerStart + climbTimeSec) )//We want to climb
	{
		state = 1;  
		return  75 + sf_hover_controller(d_val_inches,timer, 20);
	}
	else if( (timer > (timerStart + climbTimeSec))&&(timer <= (timerStart + climbTimeSec + descendTimeSec)) )//we want to descend
	{
		state = 2;
		if( (d_val_inches < 10)||(d_val_inches > 100) ) return 0; 
		return 50; 
	}
	else if( (timer > (timerStart + climbTimeSec + descendTimeSec)) && (timer <= (timerStart + climbTimeSec + descendTimeSec+ deadTime)) )
	{
		state = 3;
		return 0;
	}
	else
	{
		state = 0; //reset for next hop
		
		//if ((*setP)<= zeroSpin)*setP = 450; //cause clockwise spin
		//else if ((*setP) >= 450) *setP = zeroSpin; //no spin
		return 0;
	}
}




/*
 int32_t sf_hover_controller(uint32_t d_val, uint32_t timer) {
 
 //PID control
 
 static uint32_t i = 0;
 
 static int32_t err = 0, errLast = 0, sigErr = 0;
 static int32_t input = 0;
 static int32_t outP = 0;
 
 const int32_t kscale = 10000;
 const int32_t setP = 50; // setpoint in inches
 // scaled k
 const int32_t kp = 6000;
 const int32_t kd = 5000;
 const int32_t ki = 2;
 
 
 // First order filter params
 const int a1 = 1;
 const int b1 = 10;
 
 input = d_val;
 err = setP - d_val;
 
 outP = (kp * err) + (kd * (err - errLast)) + (ki * sigErr);
 errLast = err;
 
 //Convert to %
 outP = outP / 10000;
 
 if (outP > 90)
 outP = 90;
 else if (outP < 60)
 outP = 60;
 else
 sigErr += err;
 
 if (timer < 100) // initial take off stage. need power
 {
 outP = 90; // blow outP out of proportion.
 }
 
 return outP;
 }*/

/*int32_t sf_spin_controller(uint32_t spinRate, uint32_t setPo) 
 {
 //PID control
 static uint32_t i = 0;
 
 static int32_t err = 0, errLast = 0, sigErr = 0;
 static int32_t input = 0;
 static int32_t outP = 0;
 
 const int32_t kscale = 10000;
 const int32_t setP = setPo;//418; //0;
 
 // scaled k
 const int32_t kp = 2000;
 const int32_t kd = 2000;
 const int32_t ki = 2;
 
 input = spinRate;
 err = input - setP;
 
 outP = (kp * err) + kd * (err - errLast) + (ki * sigErr);
 errLast = err;
 
 //Convert to %
 outP = outP / 10000;
 
 if (outP > 5)
 outP = 5;//0
 else if (outP < -5)
 outP = -5;
 else
 sigErr += err;
 
 return outP;
 }
 
 
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
 //ip = sf_hop(83,hopTime,downTime,timer,&setP); 
 {
 static int counter = 0;
 static uint32_t timerStart = 0;
 static uint8_t power = 0;
 uint8_t low = 70;
 uint32_t zeroSpin = 418;
 
 float climbDescRatio = 3/5.0;
 float climbTimeSec = (climbDescRatio)*flightTime;
 float descendTimeSec = (1-climbDescRatio)*flightTime;
 
 if (counter == 0) 
 {
 timerStart = timer; //reference clock
 }
 //set up climber
 if ( timer <= (timerStart + climbTimeSec) )//We want to climb
 {
 counter ++;  
 return ;
 }
 else if( (timer > (timerStart + climbTimeSec))&& (timer <= (timerStart + climbTimeSec + descendTimeSec)) )
 {
 counter ++;
 return 60;
 }
 else if( (timer >= (timerStart + climbTimeSec + descendTimeSec))&&(timer <= (timerStart + climbTimeSec + descendTimeSec + downTime)) )
 {
 power = 0;
 return 0;
 }
 else
 {
 counter = 0; //reset for next hop
 power = 0;
 if ((*setP)<= zeroSpin)*setP = 450; //cause clockwise spin
 else if ((*setP) >= 450) *setP = zeroSpin; //no spin
 return 0;
 }
 }
 
 */