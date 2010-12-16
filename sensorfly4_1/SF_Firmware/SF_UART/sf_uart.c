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
#define CTS_MASK (1<<21)
// Pin 0.20
#define RTS_MASK (1<<20)

// encode bytes
#define START_BYTE  0xFF
#define ESC_BYTE    0x1B
#define STOP_BYTE   0xEF
#define RESET_BYTE  0xEE  
#define FAIL_BYTE   0xFE
#define SUCC_BYTE   0xFD

extern TN_EVENT ctsSet;


extern UARTDRV drvUART0;
extern int __pkt_rx_flag;
extern int state;

//-------- Events --------------------------
TN_EVENT eventRxUART0;
uint32_t eventPattern;
TN_EVENT eventReset;
TN_EVENT eventFail;
TN_EVENT eventSucc;



void sf_uart0_init()
{
  //-- Pins P0.0-TXD0 & P0.1-RXD0
  rPINSEL0 |=   1 |  (1<<2);

  // Pins 0.20 and 0.21 RTS(input) & CTS(output)
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

  // init CTS to high
#ifdef  RTS_CTS_ENABLE
  sf_uart0_cts_set(1);
#endif

  // set up event flag
  eventRxUART0.id_event = 17;
  tn_event_create(&eventRxUART0,TN_EVENT_ATTR_SINGLE,0x00000000);
  //tn_event_clear(&eventRxUART0,0x00000000);
  state = 0;
  eventReset.id_event = 19;
  tn_event_create(&eventReset,TN_EVENT_ATTR_SINGLE,0x00000000);
  tn_event_clear(&eventReset,0x00000000);
  eventFail.id_event = 21;
  tn_event_create(&eventFail,TN_EVENT_ATTR_SINGLE,0x00000000);
  tn_event_clear(&eventFail,0x00000000);
  eventSucc.id_event = 23;
  tn_event_create(&eventSucc,TN_EVENT_ATTR_SINGLE,0x00000000);
  tn_event_clear(&eventSucc,0x00000000);
  
}

void sf_uart0_int_handler(void)
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
                  case 0 :// wait
                      if (data == START_BYTE)
                      {
                            state = 1;
                      }
                      break;
                  case 1 :// start
                      if (data == ESC_BYTE)
                      {
                            state = 2;
                      }
                      else if (data == STOP_BYTE)	//stop(received)
                      {
#ifdef  RTS_CTS_ENABLE
    sf_uart0_cts_set(0);
#endif
                           state = 0;
                           if (drvUART0.pos == 1 && drvUART0.buf[0] == RESET_BYTE)
                           {
                                drvUART0.pos = 0;
                                tn_event_iset(&eventReset,0x00000001);
                           }
                           else if (drvUART0.pos == 1 && drvUART0.buf[0] == FAIL_BYTE)
                           {
                                drvUART0.pos = 0;
                                tn_event_iset(&eventFail,0x00000001);
                           }
                           else if (drvUART0.pos == 1 && drvUART0.buf[0] == SUCC_BYTE)
                           {
                                drvUART0.pos = 0;
                                tn_event_iset(&eventSucc,0x00000001);
                           }
                           else
                           {
                                drvUART0.pos = 0;
                                tn_event_iset(&eventRxUART0,0x00000001);
                           }                      
                      }
                      else if (data == START_BYTE)
                      {
                            drvUART0.pos = 0;
                      }
                      else
                      {
                             drvUART0.buf[drvUART0.pos] =(unsigned char)data;
                             drvUART0.pos++;                              
                      }
                      break;
                  case 2 :
                      state = 1;
                      drvUART0.buf[drvUART0.pos] =(unsigned char)data;
                      drvUART0.pos++;                   
                      break;
                  default :
                      break;
              }
          }
          
          if (drvUART0.pos > drvUART0.max_buf_size)
          {
              drvUART0.pos = 0;
              state = 0;
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
    for (i = 0; i< sizeof(Packet); i++ )
    { 
       if (pkt_bytes[i] == START_BYTE || pkt_bytes[i] == STOP_BYTE || pkt_bytes[i] == ESC_BYTE || pkt_bytes[i] == RESET_BYTE || pkt_bytes[i] == FAIL_BYTE || pkt_bytes[i] == SUCC_BYTE)
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

Packet * sf_uart0_pkt_receive()
{
    // get the packet and check the length of the packet
    int rw;   // result of system waiting
    Packet * pktAVR2ARM;
    rw = tn_event_wait(&eventRxUART0,0x00000001,TN_EVENT_WCOND_OR,&eventPattern,5000);
    if (rw == TERR_NO_ERR)
    {
        // get the packet from the buffer
        pktAVR2ARM = (Packet *)drvUART0.buf;
        tn_event_clear(&eventRxUART0,0x00000000);
    }
    else if (rw == TERR_TIMEOUT)
    {
        return NULL;
    }
    return pktAVR2ARM;
}

/*! \fn 
    \brief set cts on arm side
    \param new_state
    \return void
*/
void sf_uart0_cts_set (uint32_t new_state)
{
  switch (new_state)
  {
    case 0:
            rIO0CLR |= CTS_MASK;
            break;
    case 1:
            rIO0SET |= CTS_MASK;
            break;
    default:
            break;
  }
}
