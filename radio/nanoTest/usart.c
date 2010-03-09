/* $Id$ */
/**
 * @file usart.c
 * @date 2007-Dez-11
 * @author S. Rohdemann, S. Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief Console support functions.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the source code for the console
 *       and serial port support functions.
 */

/*
 * $Log$
 */

#include	"config.h"
#include    "ntrxtypes.h"
#include	"usart.h"
#include	"avrport.h"

#include 	<avr/io.h>
#include 	<avr/interrupt.h>
#include    <stdio.h>
#define ENTER_TASK {unsigned char cSREG=SREG; cli();
#define LEAVE_TASK  SREG=cSREG; sei();}

volatile MyByte8T serBuffer;
volatile MyByte8T full;

int	_getchar(FILE *stream);
int	_putchar(char c, FILE *stream);
static FILE stdinouterr = FDEV_SETUP_STREAM (_putchar, _getchar, _FDEV_SETUP_RW );

/**
 * interrupt USART1_REC:
 *
 * The interrupt service routine for receiving characters on the console i/f
 *
 * Returns: none
 */
SIGNAL(AVR_USART_RECV)
{
	serBuffer = AVR_UDR1;
	full = 1;
}

/**
 * console_init:
 *
 * Initialize console i/o support
 *
 * Returns: none.
 */
void	console_init(void)
{
	cli();
    /*
     * enable UART 1
     */
	
	PORTD |= (1 << PD0);
	DDRD &= ~(1 << DDD0);
		
	AVR_UBRR1H = 0;                                    		/* USART1: 38400 bps */
	AVR_UBRR1L = 25;
	AVR_UCSR1C |= (1 << AVR_UCSZ11) | (1 << AVR_UCSZ10);     /* USART1: asynchronous 8N1 */
	AVR_UCSR1B |= (1 << AVR_RXCIE1) | (1 << AVR_RXEN1)
				| (1 << AVR_TXEN1);							/* USART1: TX and RX
													 			and RXCIE enabled */
	AVR_UCSR1A |= (1 << AVR_U2X1);

	full = 0;
    stdout = &stdinouterr;
    stdin  = &stdinouterr;
    stderr = &stdinouterr;

	sei ();

}

/*
 * the following function need iogetc/ioputc/iostat from iolow.c
 */

/**
 * getchar:
 *
 * getchar() reads the next character from the console interface
 *
 * Returns: the character read in
 */
int	_getchar(FILE *stream)
{
	while (full == 0);
	full = 0;
	return serBuffer;
}

/**
 * putchar:
 * @c: -input- the byte to be sent
 *
 * putchar() transmits the @c byte via the console interface
 *
 * Returns: none
 */
int	_putchar(char c, FILE *stream)
{
	if(c == '\n')
	{
		while((AVR_UCSR1A & (1 << AVR_UDRE1)) == 0);
		AVR_UDR1 = '\r';
	}

	while((AVR_UCSR1A & (1 << AVR_UDRE1)) == 0);
	AVR_UDR1 = c;

	return	0;
}

/**
 * kbhit:
 *
 * kbhit() returns the status of the console interface
 *
 * Returns: 1 if one or more characters are ready for input
 *          0 otherwise
 */
int		kbhit(void)
{
	return	full;
}

#ifdef  CONFIG_CONSOLE
/**
 * read_line:
 * @buf: buffer to fill out.
 *
 * Read one line from the console interface. read_line() polls the console
 * for input. If a character is available it will be stored in @buf.
 * If ENTER is read the buffer will be closed with '\0'.
 * The '\n' or '\r' is NOT stored in @buf.
 *
 * Returns: buf if a complete line is read in.
 *          0 otherwise
 */
char    *read_line(char buf[])
{
    static  int     cnt = 0;
    char            c;

    if(!kbhit())
	{
        return  (char *) 0;
    }

    c = getchar();

    if(c == '\n' || c == '\r')
	{
        putchar('\r');
        putchar('\n');
        buf[cnt] = 0;
        cnt = 0;
        return  buf;
    }
	else
	{
        if(c == '\b' || c == 0x7f)
		{
            if(cnt > 0)
			{
                cnt--;
                putchar('\b');
                putchar(' ');
                putchar('\b');
            }
            return  (char *) 0;
        }

        if(cnt < CONFIG_CONSOLE_LINE_SIZE-1)
		{
            if(c >= 0x20 && c < 0x7f)
			{
                buf[cnt++] = c;
                putchar(c);
            }
        }
        return  (char *) 0;
    }
}
#endif  /* CONFIG_CONSOLE */
