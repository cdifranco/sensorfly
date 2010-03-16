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

// UART status register bits
#define RX_INT    (0x2<<1)
#define TO_INT    (0x6<<1)
#define TX_INT    (0x1<<1)

void sf_uart0_init()
{
  //-- Pins P0.0-TXD0 & P0.1-RXD0
  rPINSEL0 |=   1 |  (1<<2);

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


void sf_uart0_enqueue(unsigned char * buf, int size)
{
  int tx_bytes;
  int rc;
  unsigned char * tx_buf;
  unsigned int data;
  unsigned char * ptr = buf;
  int nbytes = size;
  if(ptr == NULL)
    return;
  
  //-- for Tx UART FIFO treshold = 0 -> load = UART_FIFO_SIZE
  
  tn_sem_acquire(&semTxUART0,TN_WAIT_INFINITE);
  
  while(nbytes)
  {
    if(nbytes > UART_FIFO_SIZE)
       tx_bytes = UART_FIFO_SIZE;
    else
       tx_bytes = nbytes;
    rc = tn_fmem_get(&TxUART0MemPool, (void **) &tx_buf, TN_WAIT_INFINITE);
    if(rc == TERR_NO_ERR)
    {
       memcpy(tx_buf, ptr, tx_bytes); //-- for non-blocking tx we need copy
       //-- Pack len & addr
       data = (tx_bytes << 24) | (((unsigned int)tx_buf) & 0x00FFFFFF);
       tn_queue_send(&queueTxUART0, (void *)data, TN_WAIT_INFINITE);
    }
    ptr    += tx_bytes;
    nbytes -= tx_bytes;
  }
  
  tn_sem_signal(&semTxUART0);
}


void sf_uart0_str_enqueue(unsigned char * buf)
{
   if(buf == NULL || buf[0] == 0)
      return;
   sf_uart0_enqueue(buf, strlen((char*)buf));
}


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
