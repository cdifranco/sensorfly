/*
 * uart.h
 *
 *  Created on: Oct 2, 2009
 *      Author: Aveek
 */

#ifndef UART_H_
#define UART_H_

#include "sensorfly.h"

#define UART0IN_FILENO   0       // stdin
#define UART0OUT_FILENO  1       // stdout
#define STDERR_FILENO	 2       // stderr

#define UART1IN_FILENO   3       // radio_in
#define UART1OUT_FILENO  4       // radio_out

#define PINSEL_TXD0  0
#define PINSEL_RXD0  2
#define PINSEL_TXD1  16
#define PINSEL_RXD1  18

#define UART0_PINSEL    ((1<<PINSEL_TXD0)|(1<<PINSEL_RXD0))
#define UART0_PINMASK   (0x0000000F)
#define UART1_PINSEL    ((1<<PINSEL_TXD1)|(1<<PINSEL_RXD1))
#define UART1_PINMASK   (0x000F0000)

#define LCR_DISABLE_LATCH_ACCESS 	(uint32_t)0x00000000
#define LCR_ENABLE_LATCH_ACCESS  	(uint32_t)0x00000080
#define LCR_CHAR_LENGTH_8        	(uint32_t)0x00000003
#define BAUD_9600_MSB				0x00
#define BAUD_9600_LSB	    	  	0x61
#define LSR_THR_EMPTY           	(uint32_t)0x00000020
#define LSR_RBR_EMPTY           	(uint32_t)0x00000001

#define BAUD_115200_MSB				0x00
#define BAUD_115200_LSB	    	  	0x08


#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

uint32_t UARTInit();
void UART0Handler( void ) __attribute__ ((interrupt ("IRQ")));
void UARTSend( uint8_t *BufferPtr, uint32_t Length );

void GetSerialPacket(uint8_t c);
void uart0_putc(char c);
uint8_t uart0_getc();
int uart0_getc_nb();     // return -1 if no char avail


extern volatile uint32_t UART0Status;
extern volatile uint8_t UART0TxEmpty;
extern volatile uint32_t UART0Count;

#endif /* UART_H_ */
