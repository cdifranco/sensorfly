/* $Id$ */
/**
 * @file nnspi.c
 * @date 2007-Dez-11
 * @author B. Jozefini, S. Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief SPI bus access functions.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the source code for the SPI bus
 *       control functions of the AVR controller.
 */

/*
 * $Log$
 */

#include "config.h"
#include "ntrxtypes.h"
#include "nnspi.h"
#include "hwclock.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "avrport.h"

// #define CONFIG_SPI_TRACE 1
#ifdef CONFIG_SPI_TRACE
#include <stdio.h>

static int traceId = 0;
static int traceOn = FALSE;

void SpiTraceOn (MyBoolT flag)
{
	traceOn = flag;
}

void PrintSpi(CMDT command, MyByte8T address,
			  MyByte8T *buffer, MyByte8T len)
{
	char c;
	int n;

	printf ("%4d:", traceId++);
	c = (command == WRITE_CMD) ? 'W' : 'R';
	printf (" %c", c);
	if (address < 16)
	{
		printf (" 0x0");
	}
	else
	{
		printf (" 0x");
	}
	printf ("%x", address);
	printf (" %3d: ", len);
	if (len > 6) len = 6;
   	for (n = 0; n < len; n++)
   	{
		if (buffer[n] < 16)
		{
			printf ("0");
		}
   		printf ("%x ", buffer[n]);
   	}
    printf ("\n");
}

void traceSpiReset (void)
{
	traceId = 0;
}
#endif /* CONFIG_SPI_TRACE */
/**
 * NanoReset:
 *
 * NanoReset() resets the nanoNET chip and adjusts the pin level.
 *
 * Returns: none
 */

void NanoReset(void)
{
	PONRESET_PORT &= ~PONRESET_PIN;               	/* set TRX reset high    */
	HWDelayms (1);
	PONRESET_PORT |= PONRESET_PIN;                	/* set TRX reset high    */
}

/**
 * InitSPI:
 *
 * InitSPI() initializes the SCI2 module and adjusts the pin level
 *
 * Returns: none
 */

void InitSPI(void)
{
	cli();
    /*
     * SPI bus on Port B
     */
	SPI_PORT |=	 SPI_SS
			 |  SPI_CLK
			 |  SPI_MOSI;

	SPI_DIR |=  SPI_SS                           	/* SPI Slave select      */
           	| 	SPI_CLK                           	/* SPI clock             */
           	| 	SPI_MOSI;                          	/* SPI Mosi              */

    /*
     * Interrupt and SPI chipselect lines
     */
	PONRESET_PORT |= PONRESET_PIN;                	/* set TRX reset high    */
	PONRESET_DIR  |= PONRESET_PIN;                 /* PD5 for pwrupreset    */


	AVR_SPCR  = ( 1 << AVR_SPE ) 							/* enable SPI master	 */
		  | ( 1 << AVR_MSTR ) 							/* clock rate fosc/2	 */
		  | ( 1 << AVR_DORD );							/* LSB first 			 */

	AVR_SPSR |= ( 1 << AVR_SPI2X );                       	/* fosc/2 (Double speeed) */

	sei ();
}

void SetupSPI (void)
{
	NTRXSPIWriteByte (0x00, 0x42);
}

void NTRXSPIRead(MyByte8T address, MyByte8T *buffer, MyByte8T len)
{
#	ifdef CONFIG_SPI_TRACE
	MyByte8T *tb;
	MyByte8T tl;
	tb = buffer;
	tl = len;
#	endif


    if (len > 0x80 || len == 0) return;
	ENTER_TASK;

	SPI_PORT &=	 ~SPI_SS;
	AVR_SPDR = (len & 0x7F);
	while(!(AVR_SPSR &(1 << AVR_SPIF)));

	AVR_SPDR = address;
	while(!(AVR_SPSR &(1 << AVR_SPIF)));

    while(len-- > 0)
    {
		AVR_SPDR = 0xff;
		while(!(AVR_SPSR &(1 << AVR_SPIF)));
		*buffer++ = AVR_SPDR;
    }
	SPI_PORT |=	 SPI_SS;
	LEAVE_TASK;
#	ifdef CONFIG_SPI_TRACE
	if (traceOn == TRUE)
	{
		// traceSpi(READ_CMD, address, tb, tl);
		PrintSpi (READ_CMD, address, tb, tl);
	}
#	endif /* CONFIG_SPI_TRACE */
}

void NTRXSPIWrite(MyByte8T address, MyByte8T *buffer, MyByte8T len)
{

#	ifdef CONFIG_SPI_TRACE
	if (traceOn == TRUE)
	{
		PrintSpi(WRITE_CMD, address, buffer, len);
	}
#	endif /* CONFIG_SPI_TRACE */
	ENTER_TASK;
	SPI_PORT &=	 ~SPI_SS;

	AVR_SPDR = 0x80 | (len & 0x7F);
	while(!(AVR_SPSR &(1 << AVR_SPIF)));

	AVR_SPDR = address;
	while(!(AVR_SPSR &(1 << AVR_SPIF)));

    while(len-- > 0)
    {
		AVR_SPDR = *buffer++;
		while(!(AVR_SPSR &(1 << AVR_SPIF)));
    }
	SPI_PORT |=	 SPI_SS;
	LEAVE_TASK;
}

void NTRXSPIReadByte(MyByte8T address, MyByte8T *buffer)
{
	ENTER_TASK;
	/* chip select */
	SPI_PORT &=	 ~SPI_SS;

	AVR_SPDR = 1;
	while(!(AVR_SPSR &(1 << AVR_SPIF)));

	AVR_SPDR = address;
	while(!(AVR_SPSR &(1 << AVR_SPIF)));

	AVR_SPDR = 0xff;
	while(!(AVR_SPSR &(1 << AVR_SPIF)));
	*buffer = AVR_SPDR;

	/* chip select */
	SPI_PORT |=	 SPI_SS;

	LEAVE_TASK;
#	ifdef CONFIG_SPI_TRACE
	if (traceOn == TRUE)
	{
		PrintSpi (READ_CMD, address, buffer, 1);
	}
#	endif /* CONFIG_SPI_TRACE */
}

void NTRXSPIWriteByte(MyByte8T address, MyByte8T buffer)
{
#	ifdef CONFIG_SPI_TRACE
	if (traceOn == TRUE)
	{
		PrintSpi(WRITE_CMD, address, &buffer, 1);
	}
#	endif /* CONFIG_SPI_TRACE */
	ENTER_TASK;
	/* chip select */
	SPI_PORT &=	 ~SPI_SS;

	AVR_SPDR = 0x81;
	while(!(AVR_SPSR &(1 << AVR_SPIF)));

	AVR_SPDR = address;
	while(!(AVR_SPSR &(1 << AVR_SPIF)));

	AVR_SPDR = buffer;
	while(!(AVR_SPSR &(1 << AVR_SPIF)));

	/* chip select */
	SPI_PORT |=	 SPI_SS;
	LEAVE_TASK;
}

/**
 * nanosetIRQ:
 * @value: -input- boolean enables (TRUE) or disables (FALSE) the external
 *                 interrupt request (interrupts from the nanochip)
 *
 * Returns: none
 */

void nanosetIRQ(MyBoolT value)
{
    if (value == TRUE)
	{
		EIMSK |= (1 << SPI_INT);
	}
	else
	{
		EIMSK &= ~(1 << SPI_INT);
	}
}

