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

#include <stdio.h>
#include <string.h>
#include "../common/sensorfly.h"
#include "sf_network.h"

//-----------------------------------------------------------------------------
// Local declarations
//-----------------------------------------------------------------------------

UARTDRV drvUART0;
unsigned char gUART0PayloadBuf[UART0_RX_PAYLOAD_BUF_SIZE];

//-------- Semaphores -----------------------

TN_SEM  semTxUART0;
TN_SEM  semFifoEmptyTxUART0;

//------- Queues ----------------------------

#define  QUEUE_TX_UART0_SIZE      4
#define  QUEUE_RX_UART0_SIZE      4

   //--- UART1 RX queue
TN_DQUE  queueRxUART0;
void     * queueRxUART0Mem[QUEUE_RX_UART0_SIZE];

TN_DQUE  queueTxUART0;
void     * queueTxUART0Mem[QUEUE_TX_UART0_SIZE];


//------- Memory pools ----------------------------

//--- Memory pool element size must be aligned to 4
//-- In this example, the item size (16) is already align to 4
//-- but in common case, to force right item size align, use macro MAKE_ALIG(item_type)
//---   defined in the file tn_port.h

TN_FMP RxUART0MemPool;
#define RxUART0MemPoolItemSize   (UART_FIFO_SIZE)
unsigned int memRxUART0MemPool[QUEUE_RX_UART0_SIZE * (RxUART0MemPoolItemSize/ sizeof(int))];

TN_FMP TxUART0MemPool;
#define TxUART0MemPoolItemSize   (UART_FIFO_SIZE)
unsigned int memTxUART0MemPool[QUEUE_TX_UART0_SIZE * (TxUART0MemPoolItemSize/ sizeof(int))];

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

void  sf_network_init()
{
   //-- UART0 rx drv
   drvUART0.buf          = &gUART0PayloadBuf[0];
   drvUART0.max_buf_size = UART0_RX_PAYLOAD_BUF_SIZE;
   drvUART0.pos          = 0;

    //--- Semaphores

   semTxUART0.id_sem = 0;
   tn_sem_create(&semTxUART0,
                           1,   //-- Start value
                           1);  //-- Max value

   semFifoEmptyTxUART0.id_sem = 0;
   tn_sem_create(&semFifoEmptyTxUART0,
                           1,   //-- Start value
                           1);  //-- Max value

   //--- Queues

   queueRxUART0.id_dque = 0;
   tn_queue_create(&queueRxUART0,       //-- Ptr to already existing TN_DQUE
                  &queueRxUART0Mem[0],  //-- Ptr to already existing array of void * to store data queue entries.Can be NULL
                  QUEUE_RX_UART0_SIZE   //-- Capacity of data queue(num entries).Can be 0
                  );

   queueTxUART0.id_dque = 0;
   tn_queue_create(&queueTxUART0,       //-- Ptr to already existing TN_DQUE
                  &queueTxUART0Mem[0],  //-- Ptr to already existing array of void * to store data queue entries.Can be NULL
                  QUEUE_TX_UART0_SIZE   //-- Capacity of data queue(num entries).Can be 0
                  );

   //--- Memory pools

   RxUART0MemPool.id_fmp = 0;
   tn_fmem_create(&RxUART0MemPool,
                 (void *)&memRxUART0MemPool[0], // start_addr
                 RxUART0MemPoolItemSize,
                 QUEUE_RX_UART0_SIZE
                 );
   
   TxUART0MemPool.id_fmp = 0;
   tn_fmem_create(&TxUART0MemPool,
                 (void *)&memTxUART0MemPool[0], // start_addr
                 TxUART0MemPoolItemSize,
                 QUEUE_TX_UART0_SIZE
                 );

}

Read_Sf_packet_state sf_network_wait_until_packet_rx ( Sf_packet *output_area, char tag, unsigned int timeout )
{
  trace(__FILE__,__LINE__,"enter wait until packet rx");
  int rc;
  int i;
  int len;
  int nbytes;
  unsigned int rx_block;
  unsigned int mem_addr;
  unsigned char * rx_mem;
  Read_Sf_packet_state rs;
      
  trace(__FILE__,__LINE__,"about to enter tn queue receive");
  rc = tn_queue_receive(&queueRxUART0, (void **)&rx_block, timeout);
  trace(__FILE__,__LINE__,"wait until packet rx: exited tn queue receive");
  if(rc == TERR_NO_ERR)
  {
      //--- Unpack len & addr of received bytes (might be a partial packet)
     len = ((unsigned int)rx_block) >> 24;
     mem_addr = (((unsigned int)&memRxUART0MemPool[0]) & 0xFF000000) |
                               (((unsigned int)rx_block) & 0x00FFFFFF);
     rx_mem = (unsigned char *) mem_addr;
     //rx_mem holds start of received data, len holds length of received data
     rs = sf_uart0_Sf_packet_rx(output_area, rx_mem, len, tag);
     tn_fmem_release (&RxUART0MemPool,(void*)rx_mem);
  } 
  else {
     //packet is no good, so reset header bytes to 0
     memset (output_area, 0, Sf_packet_header_overhead);
     if (TERR_TIMEOUT) {
        rs = packet_timed_out; 
        tn_fmem_release (&RxUART0MemPool,(void*)rx_mem);
        //trace(__FILE__,__LINE__,"timed out waiting for packet");
     }
     else {
        rs = malformed_packet; // supposedly we wouldn't ever get here! we aren't releasing the mem pool!, just in case, we say we got a malformed packet
        //trace(__FILE__,__LINE__,"shouldn't ever see me");
     }
  }
  //trace(__FILE__,__LINE__,"exit wait until packet rx");
  return rs;
}

void sf_network_wait_until_rx ( )
{
  trace(__FILE__,__LINE__,"enter wait until rx");
  int rc;
  int i;
  int len;
  int nbytes;
  unsigned int rx_block;
  unsigned int mem_addr;
  unsigned char * rx_mem;
      
  rc = tn_queue_receive(&queueRxUART0, (void **)&rx_block, TN_WAIT_INFINITE);
  if(rc == TERR_NO_ERR)
  {
      //--- Unpack len & addr
     len = ((unsigned int)rx_block) >> 24;
     mem_addr = (((unsigned int)&memRxUART0MemPool[0]) & 0xFF000000) |
                               (((unsigned int)rx_block) & 0x00FFFFFF);
     rx_mem = (unsigned char *) mem_addr;
  
     for(i = 0; i < len; i++)
     {
        nbytes = sf_uart0_str_rx(&drvUART0, rx_mem[i]);
        if(nbytes > 0)
        {
           if (memcmp (&rx_mem[0], "nn>", 3) == 0)
           {
              int a;
              //got prompt!
           }
        }
     }
     tn_fmem_release(&RxUART0MemPool,(void*)rx_mem);
  }
}

void sf_network_tx_send()
{
  trace(__FILE__,__LINE__,"enter sf net tx send");
  
  int rc;
  int i;
  int len;
  unsigned int tx_block;
  unsigned int mem_addr;
  unsigned char * tx_mem;
  static int times_this_function_has_been_called = 0;

  times_this_function_has_been_called++;
  
  rc = tn_queue_receive(&queueTxUART0, (void **)&tx_block, TN_WAIT_INFINITE);
  if(rc == TERR_NO_ERR)
  {
     //-- Wait for radio CTS
     sf_uart0_cts_wait();
     
     //-- Wait UART Tx FIFO interrupt (here - UART Tx FIFO is empty)
     //trace(__FILE__,__LINE__,"acquiring semaphore");
     tn_sem_acquire(&semFifoEmptyTxUART0, TN_WAIT_INFINITE);
     //trace(__FILE__,__LINE__,"semaphore acquired");

     len = ((unsigned int)tx_block) >> 24;
     mem_addr = (((unsigned int)&memTxUART0MemPool[0]) & 0xFF000000) |
             (((unsigned int)tx_block) & 0x00FFFFFF);
     tx_mem = (unsigned char *) mem_addr;
     for(i = 0; i < len; i++)
        rU0THR = tx_mem[i];
  
     tn_fmem_release(&TxUART0MemPool,(void*)tx_mem);
  } 
  else
      trace(__FILE__,__LINE__,"couldn't receive from queue");
  //trace(__FILE__,__LINE__,"exit tx send");
}

/*! \fn void sf_network_tx_enqueue
    \brief puts the UART_FIFO_SIZE sized buffer at the end of the uart queue (the queue would then need to be sent by sf_network_tx_send)
    \param par char *buf, start address of the buffer with a size at most equal to UART_FIFO_SIZE
    \param par int size, length of the buffer, must not be greater than UART_FIFO_SIZE.
    \return
*/
void sf_network_tx_enqueue(unsigned char * buf, int size)
{
  //trace(__FILE__,__LINE__,"enter tx enqueue");
  sf_uart0_enqueue(buf,size);
  //trace(__FILE__,__LINE__,"exit tx enqueue");
}

/*! \fn void sf_network_send_through_radio
    \brief  user-level call for sending data through the radio (users should have already set radio parameters such as source and destination address before using this function)
    \param  par unsigned char buf, the start address of the message
    \param  par int size, the size of the message
    \return an int from the Write_Sf_packet_state enumeration
*/
Write_Sf_packet_state sf_network_send_through_radio (unsigned char * buf, int size)
{
   trace(__FILE__,__LINE__,"enter sf send through radio");
   unsigned char _buf [UART_FIFO_SIZE];
   Read_Sf_packet_state st;
   int input_pos = 0;

   if (  size < 0 )
     return illegal_input_size;
   
   while ( input_pos < size )
   {
      int n_to_cpy;
      if (size - input_pos > UART_FIFO_SIZE)
         n_to_cpy = UART_FIFO_SIZE;
      else
         n_to_cpy = size - input_pos;
      memcpy( _buf, &buf[input_pos], n_to_cpy);
      sf_network_tx_enqueue (_buf, n_to_cpy);
      sf_network_tx_send();
      input_pos += n_to_cpy;
   }
   return acknowledged;
}


/*! \fn void sf_network_send_Sf_packet_to_local_radio
    \brief  system-level call for sending Sf_packets to the radio (users should use sf_network_send_short_msg and sf_network_put_radio_in_send_mode)
    \param  par unsigned char buf, the start address of the payload
    \param  par int size, the size of the payload
    \return an int from the Write_Sf_packet_state enumeration
*/
Write_Sf_packet_state _sf_network_send_Sf_packet_to_local_radio(radio_msg_type cmd, unsigned char * buf, int size)
{
   trace(__FILE__,__LINE__,"enter _sf send to local radio");
   static char tag = 0;
   unsigned char _buf [UART_FIFO_SIZE];
   Sf_packet pkt;
   Read_Sf_packet_state st;
   crc checksum_so_far;
   unsigned char first_loop = 1;
   int input_pos = 0;

   //Protocol the radio expects:  [ascii_soh] size_byte type_byte tag_byte <data> chksum_byte
   if ( size > UART_FIFO_SIZE - Sf_packet_header_overhead - sizeof(crc) )
     return illegal_input_size;
   _buf[0] = 1; // soh (start of header)
   _buf[1] = Sf_packet_header_overhead + size + sizeof(crc); //this field represents the length of the packet sent to the radio including protocol overhead
   _buf[2] = cmd;  //type (from the radio_msg_type enum)
   _buf[3] = tag;  //tag
   

   //ok at this point the header is done, now append the payload that will go to the radio, along with a checksum for the message, as well as checksum, and send the whole packet
   memcpy( &_buf[Sf_packet_header_overhead], buf, size);
   checksum_so_far = crcFast ((unsigned char*)&_buf, Sf_packet_header_overhead + size, 0);
   memcpy( &_buf[Sf_packet_header_overhead + size], &checksum_so_far, sizeof(crc) );
   sf_network_tx_enqueue (_buf, Sf_packet_header_overhead + size + sizeof(crc)); // at this point we are using only 1 of the UART buffers. There's 4, but we've commented out the code that does 4 for tracing down a race condition in sf_network_wait_until_packet_rx()
   sf_network_tx_send();
   //message to radio has been sent at this point


   /*  //The following code can handle multiple UART buffers, and can be used for appending a payload and the checksum instead of the lines above, but has been commented out while hunting a race condition but inside sf_network_wait_until_packet_rx()
   while ( first_loop || (input_pos < size)  )
   {
      int n_to_cpy;
      if (first_loop) {
         if (size > UART_FIFO_SIZE - Sf_packet_header_overhead) {
            n_to_cpy = UART_FIFO_SIZE - Sf_packet_header_overhead;
         }
         else {
            n_to_cpy = size;
         }
         memcpy( &_buf[Sf_packet_header_overhead], buf, n_to_cpy);
         checksum_so_far = crcFast ((unsigned char*)&_buf, n_to_cpy + Sf_packet_header_overhead, 0);
         sf_network_tx_enqueue (_buf, n_to_cpy + Sf_packet_header_overhead );
      }
      else {
         if (size - input_pos > UART_FIFO_SIZE)
            n_to_cpy = UART_FIFO_SIZE;
         else
            n_to_cpy = size - input_pos;
         memcpy( _buf, &buf[input_pos], n_to_cpy);
         checksum_so_far = crcFast ((unsigned char*)&_buf, n_to_cpy, checksum_so_far);
         sf_network_tx_enqueue (_buf, n_to_cpy);
      }
      sf_network_tx_send();
      if (first_loop)
         input_pos += UART_FIFO_SIZE - Sf_packet_header_overhead;
      else
         input_pos += UART_FIFO_SIZE;
      first_loop = 0;
   }
   memcpy ( _buf, &checksum_so_far, sizeof(crc) );
   sf_network_tx_enqueue (_buf, sizeof(crc) );
   sf_network_tx_send();
   */
   st = sf_network_wait_until_packet_rx (&pkt, tag, 150);
   st = acknowledged;
   tag++;
   if (Sf_packet_unsolicited_message_tag == tag)
     tag++; //we don't want to send our next message marked with the "unsolicited message" tag.
   if (packet_received == st && ack == pkt.type) {
      trace(__FILE__,__LINE__,"exiting _sf send to local radio (ack)");
      return acknowledged;
   }
   else if (packet_timed_out == st) {
      trace(__FILE__,__LINE__,"exiting _sf send to local radio (timedout)");
      return packet_timed_out;
   } 
   else {
      switch(st) {
        case no_packet:
          trace(__FILE__,__LINE__,"exiting _sf send to local radio, nack: no pkt");
          break;
        case waiting_for_rest_of_packet:
          trace(__FILE__,__LINE__,"exiting _sf send to local radio, nack: waiting 4 rest of pkt");
          break;
        case receiving_packet:
          trace(__FILE__,__LINE__,"exiting _sf send to local radio, nack: recieving pkt");
          break;
        case malformed_packet:
          trace(__FILE__,__LINE__,"exiting _sf send to local radio, not acknowledged: malformed pkt");
          break;
        default:
          trace(__FILE__,__LINE__,"exiting _sf send to local radio, not acknowledged: unexpected reason");
          break;
      }
      return not_acknowledged;
   }
}

/*! \fn void sf_network_send_Sf_packet_to_local_radio
    \brief  system-level call for sending messages to the radio (users should use sf_network_send_msg)
    \param  par unsigned char buf, the start address of the message
    \param  par int size, the size of the message
    \return an int from the sf_network_read_status enumeration
*/
Write_Sf_packet_state sf_network_send_Sf_packet_to_local_radio(radio_msg_type cmd, unsigned char * buf, int size)
{
   trace(__FILE__,__LINE__,"enter sf_ send to local radio");
   static int num_successes = 0;
   static int num_fails = 0;
   //1 retry on failure
   Write_Sf_packet_state st = _sf_network_send_Sf_packet_to_local_radio (cmd, buf, size);
   if (acknowledged == st) {
     num_successes++;
   } 
   else {
     num_fails ++;
     trace(__FILE__,__LINE__,"first fail, trying again");
     st = _sf_network_send_Sf_packet_to_local_radio (cmd, buf, size);
     trace(__FILE__,__LINE__,"let's see");
     if (acknowledged == st) {
       num_successes ++;
     } else {
          //second fail
          trace(__FILE__,__LINE__,"failed two times, bailing out");
          num_fails++;
     }
   }
   trace(__FILE__,__LINE__,"exiting sf_ send to local radio");
   return st;
}


/*! \fn void sf_network_set_mac_address
    \brief  user-level call for setting this radio's mac address
    \param  par unsigned char address, the new mac address
    \return an int from the sf_network_read_status enumeration
*/
Write_Sf_packet_state sf_network_set_mac_address (unsigned char address) {
   trace(__FILE__,__LINE__,"enter set mac address");
   unsigned char buf [UART_FIFO_SIZE];
   sprintf( buf, "%d", address);
   //trace(__FILE__,__LINE__,"exiting set mac address");
   return sf_network_send_Sf_packet_to_local_radio (set_src_mac_address, buf, strlen(buf) + 1);
}

/*! \fn Write_Sf_packet_state sf_network_put_radio_in_chat_mode
    \brief  user-level call for setting the radio to chat mode
    \return an int from the sf_network_read_status enumeration
*/
Write_Sf_packet_state sf_network_put_radio_in_chat_mode () {
   //to manually put a radio in chat mode via the terminal, send the following octal chars:  01 05 012 0250
   return sf_network_send_Sf_packet_to_local_radio (goto_chat_mode, 0, 0);
}

/*! \fn void sf_network_put_radio_in_send_mode
    \brief  user-level call for setting the radio to send mode. Radio will expect the specified number of bytes and send them over the air, then it will return to normal mode
    \param  par nbytes_to_be_sent the number of bytes that will be sent to the radio
    \return an int from the sf_network_read_status enumeration
*/
Write_Sf_packet_state sf_network_put_radio_in_send_mode (int nbytes_to_be_sent) {
   trace(__FILE__,__LINE__,"enter put radio in send mode");
   unsigned char buf [UART_FIFO_SIZE];
   sprintf( buf, "%d", nbytes_to_be_sent );
   //trace(__FILE__,__LINE__,"exiting put radio in send mode");
   return sf_network_send_Sf_packet_to_local_radio (goto_send_mode, buf, strlen(buf) + 1);
}

/*! \fn void sf_network_set_destination_mac_address
    \brief  user-level call for setting the destination radio's mac address
    \param  par unsigned char address, the destination mac address
    \return an int from the sf_network_read_status enumeration
*/
Write_Sf_packet_state sf_network_set_destination_mac_address (unsigned char address) {
   trace(__FILE__,__LINE__,"enter set dest mac addr");
   char buf [UART_FIFO_SIZE];
   sprintf(buf, "%d", address);
   //trace(__FILE__,__LINE__,"exiting set dest mac addr");
   return sf_network_send_Sf_packet_to_local_radio (set_dest_mac_address, buf, strlen(buf) + 1);
}

/*! \fn void sf_network_send_short_msg
    \brief  user-level, low-overhead call for sending a short message to another radio, it is assumed we have previously set the source and destination addresses
    \param  par char * buf, contains the start of the data to send to the other device
    \param  par int len, indicates the length of the data to send
    \return an int from the sf_network_read_status enumeration
*/
Write_Sf_packet_state sf_network_send_short_msg (char * buf, int size) {
   trace(__FILE__,__LINE__,"enter sf network send short msg");
   if (size > UART_FIFO_SIZE - Sf_packet_header_overhead - sizeof(crc) )
       return illegal_input_size;
   return sf_network_send_Sf_packet_to_local_radio (send_msg, buf, size);
}

/*! \fn void sf_network_print_stats
    \brief  user-level call for getting network-related statistics
    \return an int from the Write_Sf_packet_state enumeration
*/
Write_Sf_packet_state sf_network_print_stats () {
   return sf_network_send_Sf_packet_to_local_radio (get_net_stats, 0, 0);
}

/*! \fn void sf_network_send_range_request
    \brief  user-level call for sending a range request to a radio (destination radio must have been setup previous to calling this command)
    \return an int from the Write_Sf_packet_state enumeration
*/
Write_Sf_packet_state sf_network_send_range_request () {
   return sf_network_send_Sf_packet_to_local_radio (request_1way_range, 0, 0);
}

/*! \fn void sf_network_put_radio_in_listen_for_range_mode
    \brief  user-level call for having the radio reply to range requests from other radios
    \return an int from the Write_Sf_packet_state enumeration
*/
Write_Sf_packet_state sf_network_put_radio_in_listen_for_range_mode () {
   return sf_network_send_Sf_packet_to_local_radio (range_listen, 0, 0);
}


/*! \fn crc sf_network_crcFast
    \brief  network-level call for computing the crc checksum of a message
    \param  par message_len  the length of the message (not including space for the checksum)
    \param  par previous_partial_cksum  the previous value of the crc if it is being calculated in stages
    \return a crc with the checksum for the message (not including the checksum itself).
*/
crc crcFast (unsigned char message [], int message_len, crc previous_partial_cksum) {
   //trace(__FILE__,__LINE__,"enter crc fast");
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
   //trace(__FILE__,__LINE__,"exiting crc fast");
   return (remainder);
}

