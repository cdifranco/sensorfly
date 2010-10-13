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

extern TN_EVENT ctsSet;

// Pin 0.21
#define CTS_MASK (1<<21)
// Pin 0.20
#define RTS_MASK (1<<20)
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

}

void sf_network_pkt_send(Packet * pkt)
{
#ifdef  RTS_CTS_ENABLE
    int flag;
    if(rIO0PIN & CTS_MASK)
      flag = 1;
    else
      flag = 0;
    if(flag)
      sf_uart0_cts_set(0);
#endif
    
    unsigned int p_flags_pattern;
    
    //-- Send RTS and wait for radio CTS
#ifdef  RTS_CTS_ENABLE
    if(!(rIO0PIN & RTS_MASK))
       tn_event_wait(&ctsSet, 0x00000001, TN_EVENT_WCOND_OR, &p_flags_pattern, TN_WAIT_INFINITE);
#endif
    sf_uart0_pkt_send(pkt);
#ifdef  RTS_CTS_ENABLE
    tn_event_clear(&ctsSet, 0x00000000);
#endif
#ifdef  RTS_CTS_ENABLE
    if(flag)
      sf_uart0_cts_set(1);
#endif
}

Packet * sf_network_pkt_receive()
{
    sf_uart0_pkt_receive();

}

void sf_network_pkt_release()
{
    // release the drvUART buffer
#ifdef  RTS_CTS_ENABLE
   sf_uart0_cts_set(1);
#endif
}