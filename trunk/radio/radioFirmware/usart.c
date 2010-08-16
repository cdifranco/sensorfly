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
#include    "hwclock.h"
#include	"config.h"
#include    "ntrxtypes.h"
#include	"usart.h"
#include	"avrport.h"

#include 	<avr/io.h>
#include 	<avr/interrupt.h>
#include    <stdio.h>
#define ENTER_TASK {unsigned char cSREG=SREG; cli();
#define LEAVE_TASK  SREG=cSREG; sei();}
#define PACKET_RECEIVE_TIMEOUT_MSEC 30000

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
	
	//PORTD |= (1 << PD0);
	//DDRD &= ~(1 << DDD0);
		
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



/**
 * read_pkt:
 * @output_area: buffer where a packet comming in from the UART will be placed.
 *
 * Read out a packet from the serial connection. read_pkt() polls the console
 * for input. When a new (complete) packet is available, it returns, filling the buffer with the packet.
 *
 * Returns: An error code: 0 for OK, 1 for failure.
 *
 */
Read_sf_status    readSfPacket(Sf_packet * output_area)
{
    static int count = 0;
    static unsigned long time_packet_started = 0;
    /*  The latest_checksums array would be needed if we want to go back to calculating the 
    *   checksum of the message progressively rather than at the end of the message; 
    *   We would also need to uncomment a few portions of code below.
    *static crc latest_checksums [sizeof(crc)+1]; 
    */
    char c;
    Read_sf_status ret;
    unsigned long curr_time = hwclock();

    if (!kbhit()) {
        ret.nbytes = count;
        if (count!=0) {
	    if ( curr_time < (unsigned long) (time_packet_started + PACKET_RECEIVE_TIMEOUT_MSEC) ) { 
               ret.state = waiting_for_rest_of_packet;
            }
	    else {
	     /*// this code is useful when debugging timeouts
	      *if (count >= 2) {
	      *   printf("timed out in kbhit check, expected %d chars but got %d\n", output_area->size, count);
	      *} else {
	      *   puts("timed out in kbhit check before receiving msg size");
              *}
	      */
	       count = 0;
	       ret.state = packet_timed_out;
	    }
	}
        else
            ret.state = no_packet;
        return ret;
    }
    c = getchar();
    ret.nbytes = count;
    if ( (count > 0)   &&    ( curr_time > (unsigned long) (time_packet_started + PACKET_RECEIVE_TIMEOUT_MSEC)) ) {
       count = 0;
       ret.state = packet_timed_out;
       return ret;
    }
    count++;

    /*
    * // We are not calculating the checksum progressively, and rather we are calculating it 
    * // in one expensive step that occurs when the last packet is received, because otherwise
    * // we start loosing packets. In case we decide to calculate checksums progressively again, the
    * // following code needs to be uncommented:
    *if (1 == count)
    *	latest_checksums [0] = 0;
    *int i;
    *for (i=0; i<sizeof(crc); i++) {
    *   latest_checksums[sizeof(crc)-i] = latest_checksums[sizeof(crc)-i-1];
    *}
    *latest_checksums[0] = crcFast ((unsigned char *)&c, 1, latest_checksums[0]);
    */
    
    //PROTOCOL:  [soh][<size>][<type>][<tag>][<data>][<chksum>]  

    switch (count) {
        case 1:  //first byte of packet has to be ascii's "Start of header"
	    if ( 1 == c ) {
	       time_packet_started = hwclock();
	       output_area->header_soh = c;
               ret.state = receiving_packet;
	    }
	    else {
	       count = 0;  //discard partial packet because it is malformed
	       //printf ("unxpctd %d",c);
	       ret.state = malformed_packet; 
	    }
            break;
        case 2:
	    if (c >= Sf_packet_header_overhead + sizeof(crc)) {
	    output_area->size = c;
            	ret.state = receiving_packet;
	    } else {
		count = 0;
		//puts ("malfrmd pkt: too shrt");
		ret.state = malformed_packet;
	    }
            break;
        case 3:
            output_area->type = c;
            ret.state = receiving_packet;
            break;
	case 4:
	    //If we didn't have a checksum footer, this case would need to handle the possibility that this was the last character of the packet.
            output_area->tag = c;
            ret.state = receiving_packet;
	    break;
        default:
	    //This case needs to detect when a character is the last one of the message, and whether the checksum checks.
	    if (count > Sf_packet_header_overhead ) {  //sanity check
                output_area->params[count - Sf_packet_header_overhead - 1] = c;
                if (count == output_area->size) { //this is true if the current char is the last one
		    crc transmitted_checksum = *(crc *) &output_area->params [count - Sf_packet_header_overhead - sizeof(crc)]; //potential endianness issue here
		    crc computed_checksum = crcFast ( (unsigned char *) output_area, count - sizeof(crc), 0);
		    if (transmitted_checksum == computed_checksum) // we calculate the cksum once at the end rather than progressively as we get chars because otherwise we start loosing packets. Now, if we were progressively calculating a cksum as chars came in, we would use the following condition in this conditional: (latest_checksums[sizeof(crc)] == transmitted_checksum). 
			ret.state = packet_received;
		    else {
			printf("e%x,a%x\n",computed_checksum,transmitted_checksum);
		        ret.state = malformed_packet;
		    }
                    count = 0;
                }
                else 
	           ret.state = receiving_packet;
	    }
	    else {
	        //printf ("wrong protocol version? count=%d, expected header size=%d\n", count, Sf_packet_header_overhead);
		count = 0;
		ret.state = malformed_packet;
	    }
	    break;
    }
    return ret;
}



/* crcFast (From netrino.com) calculate the CRC checksum of a message */
crc crcFast (unsigned char message [], int message_len, crc previous_partial_cksum) {
   crc remainder;
   static char first_time = 1;
   static crc crcTable [256];
   if (first_time == 1) {
      first_time = 0;
      int dividend;
      //precompute the table for fast crc calculation
      for (dividend=0; dividend < 256; dividend++) {
         remainder = dividend << (CRC_WIDTH - 8);
	 char n;
         for (n = 8; n>0; n--) {
	    if (remainder & CRC_TOPBIT)
	       remainder = (remainder << 1) ^ CRC_POLYNOMIAL;
	    else
	       remainder = (remainder << 1);
	 }
	 crcTable[dividend] = remainder;
      }
   }

   unsigned char data;
   remainder = previous_partial_cksum;
   int i;
   for (i=0; i<message_len; i++) {
      data = message[i] ^ (remainder >> (CRC_WIDTH - 8));
      remainder = crcTable[data] ^ (remainder << 8);
   }

   return (remainder);
}

#endif  /* CONFIG_CONSOLE */
