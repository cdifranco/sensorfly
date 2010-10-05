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

#include "../../common/sensorfly.h"
#include "sf_uart.h"
#include "../../SF_Network/sf_network.h"

// UART status register bits
#define RX_INT    (0x2<<1)
#define TO_INT    (0x6<<1)
#define TX_INT    (0x1<<1)

// Pin 0.21
#define CTS_MASK (1<<22)
// Pin 0.20
#define RTS_MASK (1<<21)

// encode bytes
#define START_BYTE  0xFF
#define ESC_BYTE    0x1B
#define STOP_BYTE   0xEF   


void sf_uart0_init()
{
  //-- Pins P0.0-TXD0 & P0.1-RXD0
  rPINSEL0 |=   1 |  (1<<2);

  // Pins 0.20 and 0.21 RTS(output) & CTS(input)
  rIO0DIR |= (1 << 21);

  // enable access to divisor latch regs
  rU0LCR = LCR_ENABLE_LATCH_ACCESS;

  // set divisor for 38400 baud rate
  rU0DLM = 0;
  rU0DLL = 49;

  // disable access to divisor latch regs
  rU0LCR = LCR_DISABLE_LATCH_ACCESS;
  
  //-- Enable Rx & Tx interrupts
  //-- bit 0 =1 - Enable RDA(Receive Data Available) int
                
  //-- bit 1 =1 - Enable THRE(Transmit Holding Register Empty) Interrupt
  rU0IER = 3;

  //-- Int Trigger - 14 bytes, Enable FIFO,Reset Tx FIFO & Rx FIFO
  rU0FCR = (0x3<<6) | 1;

  //-- setup line control reg - disable break transmission, even parity,
  //-- 1 stop bit, 8 bit chars
  rU0LCR = 0x13;

  // init RTS to high
  sf_uart0_rts_set();
}

void sf_uart0_int_handler()
{
  register int rc;
  register int data;
  register int i;
  unsigned int status;
  unsigned char * rx_buf;
  
  status = rU0IIR;  // Reset Int Request Source
  
  status &= (0x7 <<1);
  
  /* RX */
  
  if(status == RX_INT || status == TO_INT)
  {
    rc = tn_fmem_get_ipolling(&RxUART0MemPool, (void **) &rx_buf);
    if(rc != TERR_NO_ERR)
       rx_buf = NULL;
  
    rc = 0;
    i = UART_FIFO_SIZE;
    while((rU0LSR & 0x01) && (i--)) //-- Rx FIFO not empty
    {
       data = rU0RBR;

//-----------------------------------------------------------------------------
       if(rx_buf)
          rx_buf[rc++] = (unsigned char)data;
    }
  
    if(rc > 0)
    {
       //--- Pack length & address
       data = (rc << 24) | (((unsigned int)rx_buf) & 0x00FFFFFF);
       data = tn_queue_isend_polling(&queueRxUART0, (void *) data);
       if(data != TERR_NO_ERR)
          rc = 0;
    }
  
    if(rc == 0 && rx_buf)
       tn_fmem_irelease(&RxUART0MemPool, (void *) rx_buf);
  }
  
  /* TX */
  
  if(status == TX_INT)
    tn_sem_isignal(&semFifoEmptyTxUART0);
}

char * encode(Packet * pkt)
{
    char buf[16];
    uint32_t i,j=0;
    uint8_t __length = 6;
    buf[0] = START_BYTE;
    for (i = 0; i< __length; i++ )
    { 
       if (*(((char *)pkt)+j) == START_BYTE || *(((char *)pkt)+j) == STOP_BYTE || *(((char *)pkt)+j) == ESC_BYTE)
       {
           buf[i]=ESC_BYTE;
           __length++;
           i++;
       }
       j++;
       buf[i]=*(((char *)pkt)+i);
    }
    return buf;
}


void sf_uart0_pkt_send(Packet *pkt)
{
    char * pkt_bytes = encode(pkt);
    uint8_t __length = 6;
    uint32_t i;
    tn_sem_acquire(&semTxUART0,TN_WAIT_INFINITE);
    
    // cut the data length into FIFO_SIZE
    if(__length > UART_FIFO_SIZE)
      __length = UART_FIFO_SIZE;
    
    for(i = 0; i < __length; i++)
      rU0THR = pkt_bytes[i];

    tn_sem_signal(&semTxUART0);
}

// receive from uart
int sf_uart0_rx(unsigned char * buf, unsigned char in_byte, int max_buf_size)
{
  static int pos = 0;
  int tmp;
  
  if(in_byte == '\r' || in_byte == '\n') //-- We ignore empty str here
  {
    tmp = pos;
    pos = 0;
    return tmp;
  }
  else
  {
    buf[pos] = in_byte;
    pos++;
    if(pos >= max_buf_size) //-- Too many symbols
    {
       tmp = pos;
       pos = 0;
       return tmp;
    }
  }
  return 0;
}


int sf_uart0_str_rx(UARTDRV * ud, unsigned char in_byte)
{
  int tmp;
  
  if(in_byte == '\r' || in_byte == '\n') //-- We ignore empty str here
  {
    tmp = ud->pos;
    ud->pos = 0;
    return tmp;
  }
  else
  {
    ud->buf[ud->pos] = in_byte;
    ud->pos++;
    if(ud->pos >= ud->max_buf_size) //-- Too many symbols
    {
       tmp = ud->pos;
       ud->pos = 0;
       return tmp;
    }
  }
  return 0;
}

/**
 * TODO: add packet receive code 
 */
int sf_uart0_pkt_rx(Packet *pkt, unsigned char in_byte[4])
{
 
  return 0;
}


/*! \fn 
    \brief
    \param
    \return
*/
void sf_uart0_cts_wait()
{
  volatile uint32_t flag;
  flag = rIO0PIN;
  while(~(flag & CTS_MASK));
}

/*! \fn 
    \brief
    \param
    \return
*/
void sf_uart0_rts_set()
{
  rIO0CLR |= RTS_MASK;
}

/*! \fn 
    \brief
    \param
    \return
*/
void sf_uart0_rts_clr()
{
  rIO0SET |= RTS_MASK;
}