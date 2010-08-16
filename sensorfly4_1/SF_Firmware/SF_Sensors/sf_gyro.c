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
#include "sf_gyro.h"

//-----------------------------------------------------------------------------
// Local declarations
//-----------------------------------------------------------------------------
#define PINSEL1_ADC01_MASK ((1 << 24) | (1 << 25))
#define PINSEL1_ADC02_MASK ((1 << 26) | (1 << 27))


#define PINSEL1_ADC01_EN (1 << 24)
#define PINSEL1_ADC02_EN (1 << 26)
#define AUTO_ZERO_SEL (1 << 17) //Gyro's AZ on P1.17
//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
/*! \fn void sf_sensor_gyro_init()
    \brief Initialize the 2-axis gyroscope sensor
    \param none
    \return none
*/
void sf_sensor_gyro_init() 
{

    // Enable ADC0.1
    rPINSEL1 = (rPINSEL1 & ~PINSEL1_ADC01_MASK) | PINSEL1_ADC01_EN;

    // Enable ADC0.2
    rPINSEL1 = (rPINSEL1 & ~PINSEL1_ADC02_MASK) | PINSEL1_ADC02_EN;
}

/*! \fn uint32_t sf_sensor_gyro_z_get() 
    \brief Get z-axis low resolution gyro reading
    \param none
    \return The value
*/
uint32_t sf_sensor_gyro_z_get() 
{

	uint32_t val = 0;
	rAD0CR = 0x00200400 | (1 << 1);
	rAD0CR |= 0x01000000;                              // START = 001 = Start Conversion Now

	do
	{
	  val = rAD0GDR;                                   // Read A/D Data Register
	}
	while ((val & 0x80000000) == 0);                  // Wait ADC Conversion Complete
	val = (val >> 6) & 0x03FF;                        // Shift ADC Result to Integer

	rAD0CR = 0x00;
	return val;
}\

/*! \fn uint32_t sf_sensor_gyro_z45_get() 
    \brief Get z-axis high resolution gyro reading (z x 4.5)
    \param none
    \return The value
*/
uint32_t sf_sensor_gyro_z45_get() 
{

    uint32_t val = 0;

    rAD0CR = 0x00200400 | (1 << 2);
    rAD0CR |= 0x01000000;                              // START = 001 = Start Conversion Now

    do
    {
      val = rAD0GDR;                                   // Read A/D Data Register
    }
    while ((val & 0x80000000) == 0);                  // Wait ADC Conversion Complete
    val = (val >> 6) & 0x03FF;                        // Shift ADC Result to Integer
    rAD0CR = 0x00;
    return val;
}

/*! \fn void  sf_sensor_gyro_auto_zero()
    \brief Reset the gyro's zero for better range when using high resolution readings.
    \param none
    \return none
*/
void sf_sensor_gyro_auto_zero(){
    rIO1DIR |= AUTO_ZERO_SEL;
    rIO1SET |= AUTO_ZERO_SEL;
    unsigned short wait = 0;
    while(wait < 7000){
      //TODO: Find a better way to wait 7ms!
      //Gyro auto zero time is 7ms. 
      //Loop here to wait before clearing register. Diego 06072010
      wait++;
    }
    rIO1CLR = AUTO_ZERO_SEL;
}