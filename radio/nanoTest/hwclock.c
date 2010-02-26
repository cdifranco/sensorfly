/* $Id$ */
/**
 * @file hwclock.c
 * @date 2007-Dez-11
 * @author S. Rohdemann, S. Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief Timer support for AVR.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the source code for the utility functions
 *    of the Hardware Timer Function.
 */

/*
 * $Log$
 */

#include	"config.h"
#include 	<avr/io.h>
#ifndef __OPTIMIZE__
#	define __OPTIMIZE__ 0
#endif
#include 	<util/delay.h>
#include 	<avr/interrupt.h>
#include    "ntrxtypes.h"
#include	"hwclock.h"
#include 	"keys.h"
#include    "avrport.h"

#define	MILLISECONDS_PER_TICK	10 /* at 7,3728 MHz prescaler 1024 -> TCNT0 = 256 - 72 for 10ms tick */
#define TICK_OFFSET ( 256 - ( F_CPU / 102400)  ) /* 8Mhz div 1024 10ms */
#define	STATE_HIGH	 5
#define	STATE_LOW	-5
#define MAX_DELAY_US ( 768 / ( F_CPU / 1000000UL ))
#define MAX_DELAY_MS ( 262 / ( F_CPU / 1000000UL ))

MyBoolT	key_flags[NKEYS] =
{
	FALSE
};

volatile MyDword32T	jiffies = 0;

/**
 * @brief Delay processing for n microseconds.
 * @param us this is the delay in microseconds
 *
 * This function is used for waiting before continue with
 * programm execution. Interrupts are still processed.
 * Because of the high inaccuracy of the delay function
 * this function tries to compensate the delay error
 * by adding an offset.
 */
void HWDelayus( MyWord16T us )
{
	MyWord16T ti;

	if( us == 0 ) return;

	/*
	 * this is for the loop processing time
	 * Delays in ms are about 10% longer.
	 * Delays in us between 100% down to 10%.
	 * _delay_us seams to be not very accurate.
	 */
	ti = us << 1;
	if( us > 90 )
	{
		ti += (us >> 2);
	}

	while( ti > MAX_DELAY_US )
	{
		_delay_loop_1( MAX_DELAY_US );
		ti -= MAX_DELAY_US;
	}
	if( ti > 0 )
	{
		_delay_loop_1( ti );
	}
}

/**
 * @brief Delay processing for n milliseconds.
 * @param us this is the delay in milliseconds
 *
 * This function is used for waiting before continue with
 * programm execution. Interrupts are still processed.
 */
void HWDelayms( MyWord16T ms )
{
	if( ms == 0 ) return;

	/*
	 * this is for the loop processing time
	 * Delays in ms are about 10% longer.
	 * Delays in us between 100% down to 10%.
	 * _delay_us seams to be not very accurate.
	 */
	while( ms > MAX_DELAY_MS )
	{
		_delay_ms( MAX_DELAY_MS );
		ms -= MAX_DELAY_MS;
	}

	if( ms > 0 )
	{
		_delay_ms( ms );
	}
}

/**
 * @brief clock tick initialization
 * @param start this is the start value for jiffies
 *
 * This function initializes the Timer 0 in the AVR to
 * generate an interrupt ever 10 ms.
 */
void hwclockRestart (MyDword32T start)
{
	AVR_TCCR0 = (1 << CS02) | (1 << CS00);  /* Timer0 clk/1024 normal mode int every 0.13888 ms */
	AVR_TCCR0 &= ~( 1 << CS01 );
	AVR_TIMSK = (1 << TOIE0);
	AVR_TCNT0 = TICK_OFFSET; 					/* 10 ms at 8MHz */

	jiffies = start;
}

/**
 * @brief clock tick initialization
 *
 * This function initializes the Timer 0 in the AVR to
 * generate an interrupt ever 10 ms.
 */
void hwclock_init(void)
{
#	ifdef CONFIG_USE_KEYS
	SW0_DIR &= ~SW0_PIN;
	SW0_PORT |= SW0_PIN;
#	endif /* CONFIG_USE_KEYS */
	hwclockRestart (0);
	sei ();
}


/**
 * @brief return system clock in milliseconds
 *
 * This function returns the elapsed time since
 * program start in milliseconds.
 */
MyDword32T	hwclock(void)
{
	MyDword32T r;

	cli();
	r = jiffies;
	sei();

	return (r);
}

/**
 * @brief Timer 0 overflow interrupt
 *
 * Interrupt service routine for the hardware timer
 */
SIGNAL( SIG_OVERFLOW0 )
{
#	ifdef CONFIG_USE_KEYS
	unsigned char i;
	static		int		state[NKEYS] = { 0 };
#	endif /* CONFIG_USE_KEYS */

	AVR_TCNT0 = TICK_OFFSET;

#	ifdef CONFIG_USE_KEYS
	for (i = 0; i < NKEYS; i++)
	{
		if (SW0_IN == 0)
		{
			if(state[i] > STATE_LOW)	{
				state[i]--;
			}
		}	else	{
			if(state[i] < STATE_HIGH)	{
				state[i]++;
			}
		}

		if(state[i] == STATE_HIGH)	{
			key_flags[i] = FALSE;
		}

		if(state[i] == STATE_LOW)	{
			key_flags[i] = TRUE;
		}
	}
#	endif
	jiffies += MILLISECONDS_PER_TICK;
}

