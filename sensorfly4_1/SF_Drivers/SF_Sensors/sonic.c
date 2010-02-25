/*
 * sonic.c
 *
 *  Created on: Jun 18, 2009
 *      Author: Aveek
 */
#include "sensorfly.h"
#include "sonic.h"


//#define PINSEL1_ADC04_MASK ((1 << 18) | (1 << 19))
//#define PINSEL1_ADC04_EN (1 << 18)

#define PINSEL1_ADC17_MASK ((1 << 12) | (1 << 13))
#define PINSEL1_ADC17_EN (1 << 12)

void SonicInit(void) {

	// Enable ADC0.4 for SFv2
	//PINSEL1 = (PINSEL1 & ~PINSEL1_ADC04_MASK) | PINSEL1_ADC04_EN;

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
