/*
 * gyro.c
 *
 *  Created on: Jun 18, 2009
 *      Author: Aveek
 */

#include "sensorfly.h"
#include "gyro.h"

#define PINSEL1_ADC01_MASK ((1 << 24) | (1 << 25))
#define PINSEL1_ADC02_MASK ((1 << 26) | (1 << 27))


#define PINSEL1_ADC01_EN (1 << 24)
#define PINSEL1_ADC02_EN (1 << 26)

void GyroInit(void) {

	// Enable ADC0.1
	PINSEL1 = (PINSEL1 & ~PINSEL1_ADC01_MASK) | PINSEL1_ADC01_EN;

	// Enable ADC0.2
	PINSEL1 = (PINSEL1 & ~PINSEL1_ADC02_MASK) | PINSEL1_ADC02_EN;

}

uint32_t GyroGetZ(void) {

	uint32_t val = 0;

	AD0CR = 0x00200400 | (1 << 1);

	AD0CR |= 0x01000000;                              // START = 001 = Start Conversion Now

	do
	{
	  val = AD0GDR;                                   // Read A/D Data Register
	}
	while ((val & 0x80000000) == 0);                  // Wait ADC Conversion Complete
	val = (val >> 6) & 0x03FF;                        // Shift ADC Result to Integer

	AD0CR = 0x00;
	return val;
}

uint32_t GyroGetZ45(void) {

	uint32_t val = 0;

	AD0CR = 0x00200400 | (1 << 2);

	AD0CR |= 0x01000000;                              // START = 001 = Start Conversion Now

	do
	{
	  val = AD0GDR;                                   // Read A/D Data Register
	}
	while ((val & 0x80000000) == 0);                  // Wait ADC Conversion Complete
	val = (val >> 6) & 0x03FF;                        // Shift ADC Result to Integer
	AD0CR = 0x00;
	return val;
}
