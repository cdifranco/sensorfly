/******************************************************************************
*  SensorFly, a controlled-mobile sensor networking platform.
*  Copyright (C) 2010, Carnegie Mellon University
*  All rights reserved.
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, version 2.0 of the License.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*******************************************************************************/
#ifndef SF_UART_H_
#define SF_UART_H_

#include "../SF_Network/sf_network.h" 


//-----------------------------------------------------------------------------
//  UART PARAMS
//-----------------------------------------------------------------------------
#define UART_FIFO_SIZE    24


#define UART0_RX_PAYLOAD_BUF_SIZE  64

#define UART1_RX_PAYLOAD_BUF_SIZE  64

//-----------------------------------------------------------------------------
// PINS & REGISTERS
//-----------------------------------------------------------------------------
#define LCR_DISABLE_LATCH_ACCESS 	(uint32_t)0x00000000
#define LCR_ENABLE_LATCH_ACCESS  	(uint32_t)0x00000080
#define LCR_CHAR_LENGTH_8        	(uint32_t)0x00000003

#define LSR_THR_EMPTY           	(uint32_t)0x00000020
#define LSR_RBR_EMPTY           	(uint32_t)0x00000001

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

//-----------------------------------------------------------------------------
// UART & Network Globals
//-----------------------------------------------------------------------------
extern TN_DQUE  queueRxUART0;
extern TN_DQUE  queueTxUART0;

extern  TN_FMP  RxUART0MemPool;
extern  TN_FMP  TxUART0MemPool;

extern  TN_SEM  semTxUART0;
extern  TN_SEM  semFifoEmptyTxUART0;


//-----------------------------------------------------------------------------
// PROTOTYPES
//-----------------------------------------------------------------------------
typedef struct _UARTDRV
{
  unsigned char * buf;
  int max_buf_size;
  int pos;
} UARTDRV;

void sf_uart0_pkt_send(Packet *pkt);
int  sf_uart0_rx(unsigned char * buf, unsigned char in_byte, int max_buf_size);
int  sf_uart0_str_rx(UARTDRV * ud, unsigned char in_byte);
int  sf_uart0_pkt_rx(Packet * pkt, unsigned char in_byte[4]);


void sf_uart0_init();
void sf_uart0_int_handler();

//-----------------------------------------------------------------------------
// RTS & CTS with the Radio
//-----------------------------------------------------------------------------
void sf_uart0_cts_wait();
void sf_uart0_rts_set();
void sf_uart0_rts_clr();
Packet * sf_uart0_pkt_receive();
void sf_uart0_cts_set (uint32_t new_state);
#endif /* SF_UART_H_ */
