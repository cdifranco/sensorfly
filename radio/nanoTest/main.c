/* $Id$ */
/**
 * @file main.c
 * @date 2007-Dez-11
 * @author S. Rohdemann, S. Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief System main loop for AVR portation.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the source code for the system main loop
 *       that controls the nanoLOC devkit board.
 *
 */

/*
 * $Log$
 */


#include  <stdio.h>
#include 	<string.h>
#include 	<avr/sleep.h>
#include	<avr/wdt.h>
#include	<avr/power.h>
#include  "config.h"
#include	"led.h"
#include	"ntrxutil.h"
#include  "ntrxtypes.h"
#include  "hwclock.h"
#include	"phy.h"
#include 	"packet.h"
#include	"app.h"
#include  "usart.h"

#define CONFIG_CONSOLE 1
#define CONFIG_PRINTF 1

/**
 * error_handler:
 *
 * error_handler() print out the error number and stop the system.
 *
 * Returns: None
 */
void	error_handler(MyInt16T err)
{
#ifdef	CONFIG_CONSOLE
#ifdef	CONFIG_PRINTF
	printf("ErrorHandler: error %d\n", err);
	printf("System halted!\n");
#endif	/* CONFIG_PRINTF */
#endif	/* CONFIG_CONSOLE */
	while( 1 )
	{
		LED3 (LED_OFF);
		LED0 (LED_ON);
		HWDelayms ( 200 );
		LED0 (LED_OFF);
		LED1 (LED_ON);
		HWDelayms ( 200 );
		LED1 (LED_OFF);
		LED2 (LED_ON);
		HWDelayms ( 200 );
		LED2 (LED_OFF);
		LED3 (LED_ON);
		HWDelayms ( 200 );
		LED3 (LED_OFF);
		LED2 (LED_ON);
		HWDelayms ( 200 );
		LED2 (LED_OFF);
		LED1 (LED_ON);
		HWDelayms ( 200 );
		LED1 (LED_OFF);
	}
}

/**
 * main:
 *
 * The main() function of the PPS
 *
 * Returns: None.
 */
int	main(int ac, char *av[])
{
	/* Run with 16 MHz */
	clock_prescale_set(1);

	/*
   * LEDs are on Port C off avr board
   */
	LED0_DIR |= LED0_PIN;
	LED0 (LED_OFF);

	LED1_DIR |= LED1_PIN;
	LED1 (LED_OFF);

	LED2_DIR |= LED2_PIN;
	LED2 (LED_OFF);

	LED3_DIR |= LED3_PIN;
	LED3 (LED_OFF);

	/* Initialize direction of pins for flow control for comm with the ARM */
	CTS_DIR  |= CTS_PIN;	//output
	RTS_DIR  &= ~RTS_PIN;	//input
	RTS_PORT |= RTS_PIN; //pull up

	/*
   * Initialize the timer for periodic tasks
   * or time supervized actions
   */
	hwclock_init();

#	ifdef	CONFIG_CONSOLE
	console_init();
#	endif	/* CONFIG_CONSOLE */

	/*
	 * Initialize the ntrx driver
	 */
 	NTRXInit();
	PHYInit ();

	/*
	 * Start the ntrx driver
	 */
#	if CONFIG_REG_MAP_NR != 501
	NTRXStart();
#	endif

	/*
   * initialize user application
   */
	APLInit();
	/*
	 * SYSTEM MAIN LOOP
	 */

	while(1)
	{
		/*
		 * call application periodically
		 */
		APLPoll ();	
		/*
		 * Check for incomming packets
		 */
		PHYPoll ();
	}
	return	0;
}
