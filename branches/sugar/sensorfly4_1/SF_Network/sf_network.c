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
#include "../common/tn_irq.h"
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
#define RxUART0MemPoolItemSize   (16)
unsigned int memRxUART0MemPool[QUEUE_RX_UART0_SIZE * (RxUART0MemPoolItemSize/ sizeof(int))];

TN_FMP TxUART0MemPool;
#define TxUART0MemPoolItemSize   (16)
unsigned int memTxUART0MemPool[QUEUE_TX_UART0_SIZE * (TxUART0MemPoolItemSize/ sizeof(int))];

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
void  sf_network_init()
{

   /* Initialize the UART */
   sf_uart0_init();

   /* Install UART0 with priority 1 */
   tn_irq_install(UART0_ID, 1, sf_uart0_int_handler);


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

void sf_network_wait_until_rx()
{
  int rc;
  int i;
  int len;
  int nbytes;
  unsigned int rx_block;
  unsigned int mem_addr;
  unsigned char * rx_mem;//the data shall be put here and can be get from here
      
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
           // payload processing

        }
     }

     tn_fmem_release(&RxUART0MemPool,(void*)rx_mem);
  }
}


void sf_network_pkt_wait_until_rx()
{
//  int rc;
//  int i;
//  int len;
//  int nbytes;
//  unsigned int rx_block;
//  unsigned int mem_addr;
//  unsigned char * rx_mem;//the data shall be put here and can be get from here
//      
//  rc = tn_queue_receive(&queueRxUART0, (void **)&rx_block, TN_WAIT_INFINITE);
//  if(rc == TERR_NO_ERR)
//  {
//       
//      //--- Unpack len & addr
//  
//     len = ((unsigned int)rx_block) >> 24;
//     mem_addr = (((unsigned int)&memRxUART0MemPool[0]) & 0xFF000000) |
//                               (((unsigned int)rx_block) & 0x00FFFFFF);
//     rx_mem = (unsigned char *) mem_addr;
//  
//     for(i = 0; i < len; i++)
//     {
//        nbytes = sf_uart0_pkt_rx(&drvUART0, rx_mem[i]);
//        if(nbytes > 0)
//        {
//           // payload processing
//
//        }
//     }
//     tn_fmem_release(&RxUART0MemPool,(void*)rx_mem);
//  }
}


void sf_network_tx_send()
{
  int rc;
  int i;
  int len;
  unsigned int tx_block;
  unsigned int mem_addr;
  unsigned char * tx_mem;
  
  rc = tn_queue_receive(&queueTxUART0, (void **)&tx_block, TN_WAIT_INFINITE);
  if(rc == TERR_NO_ERR)
  {
     //-- Wait for radio CTS
     // sf_uart0_cts_wait();
     
     //-- Wait UART Tx FIFO interrupt (here - UART Tx FIFO is empty)
  
     tn_sem_acquire(&semFifoEmptyTxUART0, TN_WAIT_INFINITE);
  
     len = ((unsigned int)tx_block) >> 24;
     mem_addr = (((unsigned int)&memTxUART0MemPool[0]) & 0xFF000000) |
             (((unsigned int)tx_block) & 0x00FFFFFF);
     tx_mem = (unsigned char *) mem_addr;
     for(i = 0; i < len; i++)
        rU0THR = tx_mem[i];
  
     tn_fmem_release(&TxUART0MemPool,(void*)tx_mem);
  }
}

/*! \fn 
    \brief
    \param
    \return
*/
void sf_network_tx_enqueue(unsigned char * buf, int size)
{
    sf_uart0_enqueue(buf,size);
}