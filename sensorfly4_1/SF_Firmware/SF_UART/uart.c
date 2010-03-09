/*
 * uart.c
 *
 *  Created on: Oct 2, 2009
 *      Author: Aveek
 */

#include "LPC214x.h"
#include "uart.h"
#include "iap.h"
#include "vic.h"
#include "radio.h"


#define VIC_IntEnable_UART0     (0x00000040)
#define VIC_IntEnClr_UART0      (0x00000040)
#define VIC_IntSelect_UART0     (0x00000040)
#define VIC_Channel_UART0       (6)
#define VIC_VectCntl_ENABLE     (0x00000020)


volatile uint32_t UART0Status;
volatile uint8_t UART0TxEmpty = 1;
volatile uint8_t c;

/*****************************************************************************
** Function name:		UART0Handler
**
** Descriptions:		UART0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void UART0Handler (void)
{
    uint8_t IIRValue, LSRValue;
    uint8_t Dummy;

    uint32_t cpsr;

    cpsr = DisableIRQ();				/* handles nested interrupt */
    IIRValue = UART0_IIR;

    IIRValue >>= 1;			/* skip pending bit in IIR */
    IIRValue &= 0x07;			/* check bit 1~3, interrupt identification */
    if ( IIRValue == IIR_RLS )		/* Receive Line Status */
    {
	LSRValue = UART0_LSR;
	/* Receive Line Status */
	if ( LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI) )
	{
	    /* There are errors or break interrupt */
	    /* Read LSR will clear the interrupt */
	    UART0Status = LSRValue;
	    Dummy = UART0_RBR;		/* Dummy read on RX to clear
					interrupt, then bail out */
	    RestoreIRQ(cpsr);
	    VICVectAddr = 0;		/* Acknowledge Interrupt */
	    return;
	}

	if ( LSRValue & LSR_RDR )	/* Receive Data Ready */
	{
	    /* If no error on RLS, normal ready, save into the data buffer. */
	    /* Note: read RBR will clear the interrupt */

		c = UART0_RBR;
		GetSerialPacket(c);
	}
    }
    else if ( IIRValue == IIR_RDA )	/* Receive Data Available */
    {
	/* Receive Data Available */
    	c = UART0_RBR;
    	GetSerialPacket(c);
    }
    else if ( IIRValue == IIR_CTI )	/* Character timeout indicator */
    {
		/* Character Time-out indicator */
		UART0Status |= 0x100;		/* Bit 9 as the CTI error */
		}
		else if ( IIRValue == IIR_THRE )	/* THRE, transmit holding register empty */
		{
		/* THRE interrupt */
		LSRValue = UART0_LSR;		/* Check status in the LSR to see if
						valid data in U0THR or not */
		if ( LSRValue & LSR_THRE )
		{
			UART0TxEmpty = 1;
		}
		else
		{
			UART0TxEmpty = 0;
		}
    }

    RestoreIRQ(cpsr);
    VICVectAddr = 0;		/* Acknowledge Interrupt */
}

/*****************************************************************************
** Function name:		UARTInit
**
** Descriptions:		Initialize UART0 port, setup pin select,
**				clock, parity, stop bits, FIFO, etc.
**
** parameters:			UART baudrate
** Returned value:		true or false, return false only if the
**				interrupt handler can't be installed to the
**				VIC table
**
*****************************************************************************/
uint32_t UART0Init()
{
	PINSEL0 = (PINSEL0 & ~UART0_PINMASK) | UART0_PINSEL;
	// enable access to divisor latch regs
	UART0_LCR = LCR_ENABLE_LATCH_ACCESS | LCR_CHAR_LENGTH_8;
	// set divisor for desired baud
	UART0_FDR = 0x00000010;
	UART0_DLM = BAUD_9600_MSB; //BAUD_115200_MSB;
	UART0_DLL = BAUD_9600_LSB; //BAUD_115200_LSB;
	// disable access to divisor latch regs
	UART0_LCR = LCR_DISABLE_LATCH_ACCESS | LCR_CHAR_LENGTH_8;
	// setup fifo control reg - trigger level 0 (1 byte fifos), no dma
	// enable and clear fifo
	UART0_FCR = 0x07;
	// setup line control reg - disable break transmittion, even parity,
	// 1 stop bit, 8 bit chars
	UART0_LCR = 0x03;	// Turn off even parity

	VICIntSelect &= ~VIC_IntSelect_UART0;
    VICVectAddr2 = (uint32_t) UART0Handler;
    VICVectCntl2 = VIC_VectCntl_ENABLE | VIC_Channel_UART0;
    VICIntEnable |= VIC_IntEnable_UART0;

    UART0_IER = IER_RBR | IER_THRE | IER_RLS;	/* Enable UART0 interrupt */

    return (TRUE);
}


/**
 * Get Serial Packet
 */
void GetSerialPacket(uint8_t val) {

	static int prev_val, len=0, inPkt = 0;

	if(RadioPacketReceived == 1)
		return;

	if(len >= RADIO_MAX_PKT_LEN) {
		inPkt = 0;
		len = 0;
		return;
	}

	if(inPkt == 0) {
		if(val != START) {
			return;
		} else {
			inPkt = 1;
			prev_val = START;
		}
	}else if(inPkt == 1) {
		if(prev_val == ESC) {
			RadioRxBuffer[len] = val;
			len++;
			prev_val = val;
		}
		if(val == ESC) {
			prev_val = ESC;
			return;
		}
		if(val == END) {
			inPkt = 0;
			RadioPacketReceived = 1;
			RadioPacketLength = len;
			len = 0;
			return;
		}

		RadioRxBuffer[len] = val;
		len++;
		prev_val = val;
	}
}

int uart0_getc_nb()
{
	if ((UART0_LSR & LSR_RBR_EMPTY) == 0) {
	    return -1;
	  }
	  else {
	    return UART0_RBR;
	  }
}

uint8_t uart0_getc()
{
	while ((UART0_LSR & LSR_RBR_EMPTY) == 0);
	  return UART0_RBR;
}

uint8_t uart1_getc()
{
}
/*****************************************************************************
** Function name:		UARTSend
**
** Descriptions:		Send a block of data to the UART 0 port based
**				on the data length
**
** parameters:			buffer pointer, and data length
** Returned value:		None
**
*****************************************************************************/
void UARTSend(uint8_t *BufferPtr, uint32_t Length )
{
	UART0_IER = IER_THRE | IER_RLS; // Disable RBR

    while ( Length != 0 )
    {
	while ( !(UART0TxEmpty & 0x01) );	/* THRE status, contain valid
						data */
	UART0_THR = *BufferPtr;
	UART0TxEmpty = 0;	/* not empty in the THR until it shifts out */
	BufferPtr++;
	Length--;
    }

    UART0_IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
    return;
}


void uart0_putc(char c) {
	UART0_IER = IER_THRE | IER_RLS; // Disable RBR
	while ( !(UART0TxEmpty & 0x01) );	/* THRE status, contain valid data */
	UART0_THR = c;
	UART0TxEmpty = 0;	/* not empty in the THR until it shifts out */
	UART0_IER = IER_THRE | IER_RLS | IER_RBR;	/* Re-enable RBR */
}

void uart1_putc(char c) {
}
/******************************************************************************
**                            End Of File
******************************************************************************/
