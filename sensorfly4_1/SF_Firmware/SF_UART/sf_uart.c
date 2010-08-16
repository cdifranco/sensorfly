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
#include "../../SF_Network/sf_network.h"

// UART status register bits
#define RX_INT    (0x2<<1)
#define TO_INT    (0x6<<1)
#define TX_INT    (0x1<<1)

// Pin 0.20
#define CTS_MASK (1<<20)
// Pin 0.21
#define RTS_MASK (1<<21)

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
  trace(__FILE__,__LINE__,"enter uart0 int handler");

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
    trace(__FILE__,__LINE__,"uart int: rx event");
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
  
  if(status == TX_INT) {
    trace(__FILE__,__LINE__,"uart int: releasing semaphore for uart0 tx fifo empty");
    tn_sem_isignal(&semFifoEmptyTxUART0);
  }
  trace(__FILE__,__LINE__,"exiting uart int");
}


void sf_uart0_enqueue(unsigned char * buf, int size)
{
  trace(__FILE__,__LINE__,"enter sf_uart0_enqueue");
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
  trace(__FILE__,__LINE__,"sf_uart0_enqueue: acquired semaphore");
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
  trace(__FILE__,__LINE__,"sf_uart0_enqueue: signaled tx semaphore");
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


Read_Sf_packet_state read_into_packet ( Sf_packet* output_area, unsigned char c) {
   static char count = 0;
   Read_Sf_packet_state ret;
   count++;
   switch (count){
     case 1 :
       if (1 == c)
       {
          output_area->header_soh = c;
          ret = receiving_packet;
       }
       else {
          count = 0;
          ret = malformed_packet;
       }
       break;
     case 2 :
       if (Sf_packet_header_overhead + sizeof(crc) <= c) {
          output_area->size = c;
          ret = receiving_packet;
       } 
       else {
          count = 0;
          ret = malformed_packet;
       }
       break;
       case 3 :
          output_area->type = c;
          ret = receiving_packet;
       break;
       case 4 : //If we weren't expecting a chksum, this case would need to consider the possibility that the Sf_packet ends here
          output_area->tag = c;
          ret = receiving_packet;
       break;
       default : //In this case we need to handle the possibility that this is the last packet, and in that case we need to see whether the checksum checks.
          output_area->params [count - Sf_packet_header_overhead - 1] = c;
          if (count == output_area->size) { //This is true only at the last character of the packet
             crc transmitted_checksum = *(crc*) &output_area->params [ count - Sf_packet_header_overhead - sizeof(crc) ]; //potencial endianness issue here if checksum spans more than a byte.
             crc computed_checksum = crcFast ( (unsigned char *) output_area, count - sizeof(crc), 0);
             if (transmitted_checksum == computed_checksum)
                ret = packet_received;
             else
                ret = malformed_packet;
             count = 0;
          }
          else
             ret = receiving_packet;
       break;
   }
   return ret;
}

Read_Sf_packet_state sf_uart0_Sf_packet_rx ( Sf_packet *output_area, unsigned char *rcvd_data, int len, char tag_to_look_for ) {
   int pos = 0; //index into received data, might point to garbage before a packet
   int count; //index from actual start of a packet
   Read_Sf_packet_state status_after_char_insert = no_packet;
   int const smallest_possible_packet_size = Sf_packet_header_overhead + sizeof(crc); //For some reason the while loop doesn't like the calculation if this sum is defined inside the while condition so we provide a variable for it

   if (len < Sf_packet_header_overhead + sizeof(crc))
     return malformed_packet;
   //packets conform to protocol: <ascii soh><size byte><type byte><tag byte><data><cksum byte>
   while (  len - pos >= smallest_possible_packet_size ){ //we won't deal with partial packets in the buffer
      if (rcvd_data[pos] == '\x01') {
         if (rcvd_data[pos+3] == tag_to_look_for) { 
            for (status_after_char_insert = receiving_packet; receiving_packet == status_after_char_insert && pos < len; pos++) {
               status_after_char_insert = read_into_packet (output_area, rcvd_data[pos]);     
            }
            if (packet_received == status_after_char_insert)
              break;
         }
      }
      pos++; //the packet we are looking for hasn't started yet, so see if we find it one byte ahead
   }
   if (no_packet == status_after_char_insert)
      trace(__FILE__,__LINE__,"sf_uart0_sf_packet_rx: packet not found");
   return status_after_char_insert;
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
  //comment out next line to force send messages to radio in case radio doesn't signal CTS for some odd reason
  while(!(rIO0PIN & CTS_MASK));
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