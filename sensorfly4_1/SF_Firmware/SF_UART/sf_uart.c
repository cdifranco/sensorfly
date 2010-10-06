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



extern UARTDRV drvUART0;
extern int __pkt_rx_flag;
extern int state;
extern int debug;

//-------- Events --------------------------
//TN_EVENT eventRxUART0;
//uint32_t eventPattern;



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

  __pkt_rx_flag = 0;
  state = 0;
  debug = 0;

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
          while((rU0LSR & 0x01)) //-- Rx FIFO not empty
          {
              data = rU0RBR;
              switch (state)
              {
                  case 0 :
                      if (data == START_BYTE)
                      {
                            state = 1;
                      }
                      break;
                  case 1 :
                      if (data == ESC_BYTE)
                      {
                            state = 2;
                      }
                      else if (data == STOP_BYTE)	//stop(received)
                      {
                             state = 0;
                             // initiate events
                             debug++;
                              __pkt_rx_flag = 1;
                      }
                      else
                      {
                             drvUART0.buf[drvUART0.pos] =(unsigned char)data;
                             drvUART0.pos++; 
                             debug++;
                      }
                      break;
                  case 2 :
                      state = 1;
                      drvUART0.buf[drvUART0.pos] =(unsigned char)data;
                      drvUART0.pos++;
                      debug++;
                      break;
                  default :
                      break;
              }
          }
          
          if (drvUART0.pos > drvUART0.max_buf_size)
          {
              drvUART0.pos = 0;
          }
        }
     
  
  /* TX */
  
  if(status == TX_INT)
    tn_sem_isignal(&semFifoEmptyTxUART0);
}

void sf_uart0_pkt_send(Packet *pkt)
{
    char * pkt_bytes =(char *)pkt;
    
    uint8_t i;
    tn_sem_acquire(&semTxUART0,TN_WAIT_INFINITE);
    
    // cut the data length into FIFO_SIZE
    if(pkt->length > UART_FIFO_SIZE)
      pkt->length = UART_FIFO_SIZE;

    
    tn_sem_acquire(&semFifoEmptyTxUART0, TN_WAIT_INFINITE);
    rU0THR = START_BYTE;

    for (i = 0; i< pkt->length; i++ )
    { 
       if (pkt_bytes[i] == START_BYTE || pkt_bytes[i] == STOP_BYTE || pkt_bytes[i] == ESC_BYTE)
       {
          
           tn_sem_acquire(&semFifoEmptyTxUART0, TN_WAIT_INFINITE);
           rU0THR = ESC_BYTE;
       }
      
       tn_sem_acquire(&semFifoEmptyTxUART0, TN_WAIT_INFINITE);
       rU0THR = pkt_bytes[i];
    }

    rU0THR = STOP_BYTE;
    
    tn_sem_signal(&semTxUART0);
}

void sf_uart0_pkt_receive()
{
    // get the packet and check the length of the packet
    if (__pkt_rx_flag)
    {
        //Packet * pktRadio2Arm = (Packet *)drvUART0.buf;
        char * bufRadio2Arm = (char *)drvUART0.buf;
        __pkt_rx_flag = 0;
    }
    
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
