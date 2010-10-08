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
#include "sensorfly.h"

//-----------------------------------------------------------------------------
// Local declarations
//-----------------------------------------------------------------------------

#define LED_SEL 0x800000

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
/*! \fn void sf_led_init()
    \brief Set port directions for led
    \param none
    \return none
*/
void sf_led_init()
{
   // P0.23 is the LED
   rIO0DIR |= LED_SEL;	
   rIO0SET |= LED_SEL;
}


/*! \fn void sf_led_on()
    \brief Switch led on
    \param none
    \return none
*/
void sf_led_on()
{
  rIO0CLR = LED_SEL;
}

/*! \fn void sf_led_off()
    \brief Switch led off
    \param none
    \return none
*/
void sf_led_off()
{
  rIO0SET = LED_SEL;
}