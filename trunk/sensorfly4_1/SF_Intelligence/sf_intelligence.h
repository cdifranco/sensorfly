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

#ifndef SF_INTELLIGENCE_H_
#define SF_INTELLIGENCE_H_
#include "sf_mapping.h"

//-----------------------------------------------------------------------------
// Task Get Position
//-----------------------------------------------------------------------------
#define  TASK_GET_POSITION_PRIORITY   1
#define  TASK_GET_POSITION_STK_SIZE  256

void  sf_get_position_task_init();
void task_get_position_func(void * par);
void  sf_get_position_task_init();
void task_act_as_base_node_func(void * par);
void task_act_as_origin_func(void * par);
#endif /* SF_INTELLIGENCE_H_ */