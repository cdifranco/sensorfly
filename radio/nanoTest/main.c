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


#include    <stdio.h>
#include 	<avr/sleep.h>
#include	<avr/wdt.h>
#include	<avr/power.h>
#include    "config.h"
#include	"led.h"
#include	"ntrxutil.h"
#include    "ntrxtypes.h"
#include    "hwclock.h"
#include	"phy.h"
#include 	"packet.h"

#define CONFIG_CONSOLE 1
#define CONFIG_PRINTF 1


#ifdef	CONFIG_CONSOLE
#include    "usart.h"
#endif	/* CONFIG_CONSOLE */

extern  void    APLInit(void);
extern	void	APLPoll(void);

#ifdef CONFIG_WATCHDOG
#include 	<avr/wdt.h>
#endif


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

	/** sub task 1: define a specific packet and send it back to computer*/
	Packet * pkt;
	pkt->id = 1;
	pkt->dest = 1;
	pkt->src = 2;
	pkt->type = PKT_TYPE_DATA;
	pkt->checksum = 0;
	pkt->length = 8;
	pkt->data[0]='a';
	pkt->data[1]='b';
	char buf[10];
	
	
	while(1)
	{
/*
		int i;
		
		buf[0] = START_BYTE;
		buf[9] = STOP_BYTE;
		
		for (i = 1; i<9; i++)
		{
			buf[i] = 'a';
		}
	
		for (i = 0; i< 10; i++)
		{
				if (buf[i] == START_BYTE || buf[i] == ESC_BYTE || buf[i] == STOP_BYTE)
				{
						putchar(ESC_BYTE);
				}
				putchar(buf[i]);

		}
*/
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
