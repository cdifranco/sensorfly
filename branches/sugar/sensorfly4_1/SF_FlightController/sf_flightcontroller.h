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

#ifndef SF_FLIGHTCONTROLLER_H_
#define SF_FLIGHTCONTROLLER_H_
#include "fuzzy_control.h"
#include "../SF_Firmware/SF_Drives/sf_drive.h"
#include "../SF_Firmware/SF_Sensors/sf_sonic.h"
#include "../SF_Firmware/SF_Sensors/sf_gyro.h"

//-----------------------------------------------------------------------------
// Task flight controller
//-----------------------------------------------------------------------------
#define  TASK_FLIGHTCONTROLLER_PRIORITY   1
#define  TASK_FLIGHTCONTROLLER_STK_SIZE  128
#define  FLIGHTTIME 30                   //time in seconds
#define  PI 3.14159265;

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------
void  sf_flightcontroller_task_init(void);
inline uint8_t sf_sinusoid_input(float x, uint8_t w); // yield sine input bounded btwn 70 and 100 Pc power
int32_t sf_hover_controller(uint32_t d_val,uint32_t timer, int32_t setP);
int32_t sf_spin_controller(uint32_t spin,uint32_t setPo, uint16_t *gyroFilt);
void task_flightcontroller_func(void * par);
//uint8_t sf_hop(uint8_t high,uint32_t hopTime,uint32_t downTime,uint32_t timer,uint32_t *setP);
uint8_t sf_hop(uint32_t flightTime,uint32_t d_val_inches, uint32_t timer, uint32_t *setP);
uint16_t push(uint16_t *gyroFilt, uint16_t gyro);
//-----------------------------------------------------------------------------
// Navigation API
//-----------------------------------------------------------------------------

#endif /* SF_FLIGHTCONTROLLER_H_ */