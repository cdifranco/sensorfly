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

#include "../../common/sensorfly.h"
#include "sf_drive.h"

//-----------------------------------------------------------------------------
// Local declarations
//-----------------------------------------------------------------------------
#define PWM0_125_HZ 120000
#define PWM_FREQ PWM0_125_HZ

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
/*! \fn void sf_drive_init(void)
    \brief Initialize SensorFly drives
    \param none
    \return none
*/
void sf_drive_init(void) 
{
	uint32_t duty_cycle;

	rPINSEL0 |= 0x000A0000;

	rPWMPR = 0;
	
	// Enable PWM4 & PWM6 for output
	rPWMPCR |= 0x00005000;
	rPWMMCR |= 0x00000002;

	duty_cycle = ((0 * PWM_FREQ) / 100);

	rPWMMR0  = PWM_FREQ;
	rPWMMR4	= duty_cycle;
	rPWMMR6	= duty_cycle;

	rPWMLER = 0x00000051;
	rPWMTCR = 0x00000009;
}


/*! \fn void sf_drive_duty_set(uint32_t drive1DC, uint32_t drive2DC)
    \brief Set drive duty cycles
    \param drive1DC Clockwise drive duty cycle 
    \param drive2DC Counter-clockwise drive duty cycle
    \return none
*/
void sf_drive_duty_set(uint32_t drive1DC, uint32_t drive2DC) 
{

	uint32_t duty_cycle;

	duty_cycle = ((drive1DC * PWM_FREQ) / 100);
	rPWMMR4 = duty_cycle;
	duty_cycle = ((drive2DC * PWM_FREQ) / 100);
	rPWMMR6 = duty_cycle;
	rPWMLER = 0x00000050;
}