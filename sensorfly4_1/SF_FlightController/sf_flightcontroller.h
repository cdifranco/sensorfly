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

#include "../SF_Firmware/SF_Drives/sf_drive.h"
#include "../SF_Firmware/SF_Sensors/sf_sonic.h"
#include "../SF_Firmware/SF_Sensors/sf_gyro.h"

//-----------------------------------------------------------------------------
// Task flight controller
//-----------------------------------------------------------------------------
#define  TASK_FLIGHTCONTROLLER_PRIORITY   1
#define  TASK_FLIGHTCONTROLLER_STK_SIZE  128
#define  TASK_FLIGHTCONTROLLER_TASKID 11

//-----------------------------------------------------------------------------
// Initialization
//-----------------------------------------------------------------------------
void  sf_flightcontroller_task_init(void);

//-----------------------------------------------------------------------------
// Navigation API
//-----------------------------------------------------------------------------

#endif /* SF_FLIGHTCONTROLLER_H_ */