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
#include "sf_sonic.h"

//-----------------------------------------------------------------------------
// Local declarations
//-----------------------------------------------------------------------------
#define PINSEL1_ADC17_MASK ((1 << 12) | (1 << 13))
#define PINSEL1_ADC17_EN (1 << 12)

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
/*! \fn void sf_sensor_sonic_init(void)
 \brief Initialize the ultrasound height sensor
 \param none
 \return none
 */
void sf_sensor_sonic_init() 
{
    // Enable ADC1.7
    rPINSEL1 = (rPINSEL1 & ~PINSEL1_ADC17_MASK) | PINSEL1_ADC17_EN;
}

/*! \fn uint32_t sf_sensor_sonic_get() 
 \brief Get height reading from ultrasound sensor
 \param none
 \return The value
 */
uint32_t sf_sensor_sonic_get() 
{
	
    volatile uint32_t val = 0;
	
    rAD1CR = 0x00200400 | (1 << 7);
	
    rAD1CR |= 0x01000000;                              // START = 001 = Start Conversion Now
	
    do
    {
		val = rAD1GDR;                                   // Read A/D Data Register
    }
    while ((val & 0x80000000) == 0);                  // Wait ADC Conversion Complete
    val = (val >> 6) & 0x03FF;                        // Shift ADC Result to Integer
	
    rAD1CR = 0x00;
	
    return val;
}
/* This function converts the read ADC value into inches
 * ADC1.7 is a 10 bit ADC pegged at 3.3V
 * a reading of '1' corresponds to 3.3/1023 V
 * Data sheet specifies that sonic sensor in analog mode yields:
 * 0.0064V per inch
 * dist in inches = (reading*(3.3/1023))/0.0064
 */
uint32_t sf_sensor_sonic_to_inches(uint32_t adcValue)
{
	volatile uint32_t val_to_inches = 0;
	val_to_inches = (uint32_t)((adcValue * (3.3/1023))/ 0.0064);
   return val_to_inches;
 }