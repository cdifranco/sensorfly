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
#include "sonic.h"

#define PINSEL1_ADC17_MASK ((1 << 12) | (1 << 13))
#define PINSEL1_ADC17_EN (1 << 12)

void SonicInit(void) {

	// Enable ADC1.7 for SFv3
	PINSEL1 = (PINSEL1 & ~PINSEL1_ADC17_MASK) | PINSEL1_ADC17_EN;

}

uint32_t SonicGetVal(void) {

	volatile uint32_t val = 0;

	AD1CR = 0x00200400 | (1 << 7);

	AD1CR |= 0x01000000;                              // START = 001 = Start Conversion Now

	do
	{
	  val = AD1GDR;                                   // Read A/D Data Register
	}
	while ((val & 0x80000000) == 0);                  // Wait ADC Conversion Complete
	val = (val >> 6) & 0x03FF;                        // Shift ADC Result to Integer

	AD1CR = 0x00;

	return val;

}
