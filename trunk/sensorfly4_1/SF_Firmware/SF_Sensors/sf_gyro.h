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

#ifndef SF_GYRO_H_
#define SF_GYRO_H_

//#define AZ_ZERO 256 //middle of scale in "auto-zeroed" gyro. Diego 06072010

void sf_sensor_gyro_init();
uint32_t sf_sensor_gyro_z_get();
uint32_t sf_sensor_gyro_z45_get(); //auto zero for better range. Diego 06072010
void sf_sensor_gyro_auto_zero();
#endif /* SF_GYRO_H_ */
