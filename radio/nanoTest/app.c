/* $Id$ */
/**
 * @file app.c
 * @date 2007-Dez-10
 * @author S.Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief Application layer of this demo..
 *
 * @todo Making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the source code for the demo application.
 */

/*
 * $Log$
 */
#include 	<stdio.h>
#include 	<string.h>
#include	<stdlib.h>
#include	"config.h"
#include	"ntrxtypes.h"
#include	"ntrxranging.h"
#include	"usart.h"
#include 	"keys.h"
#include 	"ntrxutil.h"
#include	"hwclock.h"
#include	"app.h"
#include	"phy.h"
#include 	"nnspi.h"
#include	"led.h"
#include	"packet.h"  

/**
 * @def TIME_OUT
 * @brief Idle time before sending data.
 *
 * This value sets the idle time before the application stopps waiting for
 * new data and sends all collected data to the preset device.
 *
 */
#define TIME_OUT  100
/**
 * @def SendMsg
 * @brief Data request to the next lower layer.
 *
 * This function calls the next lower layer to request a data transmission.
 *
 */
#define SendMsg PDSap

/** @brief Memory for all layer configuration settings.  */
AplMemT aplM;
/** @brief Pointer to memory for all layer configuration settings.  */
AplMemT *apl;
/** @brief Message structure for data requests.  */
MyMsgT downMsg;
/** @brief Message structure for payload of received ranging packets.  */
RangingPIB *upRangingMsg;
Packet * pkt_rx;

uint8_t state = 0;
volatile uint8_t __pkt_rx_flag = 0;

/**
 * @brief Process incoming messages.
 * @param *msg this is the message pointer
 *
 * This function receives all incoming messages from the lower layer
 * and sends the payload to the serial interface.
 */
/**************************************************************************/
void APLCallback (MyMsgT *msg)
/**************************************************************************/
{
	MyChar8T serial_print_buffer[32];
	
	MyInt16T i;
	
	switch (msg->prim)
	{
		case PD_DATA_CONFIRM: 	
								downMsg.len = 0;
							  	break;
		case PD_RANGING_CONFIRM:
		case PD_RANGING_FAST_CONFIRM:	
				upRangingMsg = (RangingPIB*) msg->data;
				switch(msg->status)
				{
					case PHY_SUCCESS	:
						/* hwack received, ranging start successfully */
						//printf("ranging start successfully \n");
						break;
					case PHY_NO_ACK		:
						/* no hwack received, ranging didnt start */
						sprintf(serial_print_buffer,"%07.2f,%03i\n",upRangingMsg->distance, upRangingMsg->error);
						printf("ranging did not start : %s \n",serial_print_buffer);
						break;
					case PHY_BUSY 		:
					case PHY_BUSY_TX 	:
						/* measurement is allready running (BUSY), wait
							for PD_RANGING_INDICATION first! */
						//printf("ranging~~\n");
						break;
					case PHY_CONFIGURATION_ERROR :
						/* driver isnt correct initialized for ranging */
						sprintf(serial_print_buffer,"%07.2f,%03i\n",upRangingMsg->distance, upRangingMsg->error);
						printf("initialization error : %s \n",serial_print_buffer);
						break;
					default : break;
				}
				break;

		case PD_DATA_INDICATION:
          // Check if packet is for this node
					if(memcmp(msg->addr,apl->src,6) != 0)
          {
							printf("not for this packet, msg for %d and addr is %d \n",msg->addr[5],apl->src[5]);
							break;
          }                  
					// Check length of packet
					pkt_rx = (Packet *)msg->data;
					if (memcmp(&(msg->len),&(pkt_rx->length),1) != 0)
					{
							printf("length of the pkt is not consistent, should be %d but only get %d \n",pkt_rx->length,msg->len);
							break;
					}
					// Send packet to ARM
					putchar(START_BYTE);
					for (i = 0; i < msg->len; i++)
					{
							if (msg->data[i] == START_BYTE || msg->data[i] == ESC_BYTE || msg->data[i] == STOP_BYTE)
							{
									putchar(ESC_BYTE);				
							}
							putchar (msg->data[i]);
					}
					putchar(STOP_BYTE);
					
				break;
								
		case PD_RANGING_INDICATION:
		case PD_RANGING_FAST_INDICATION:
						// getting the ranging result data
						upRangingMsg = (RangingPIB*) msg->data;
						
						// create pkt that will send to ARM
						Packet pktRadio2Arm;
						memcpy(pktRadio2Arm.data, &(upRangingMsg->distance), 4);
						memcpy(&(pktRadio2Arm.data[4]), &(upRangingMsg->error), 1);
						pktRadio2Arm.id = 0; // set id 0 pkt as ranging pkt
						pktRadio2Arm.type = PKT_TYPE_RANGING;
						pktRadio2Arm.checksum = 0;
						pktRadio2Arm.dest = 0; // set dest = 0 only when it is ranging
						pktRadio2Arm.src = 0; // set src = 0 only when it is ranging
						pktRadio2Arm.length = 11; // only contains distance and error info = 5 bytes
						Packet * pkt_temp = &(pktRadio2Arm);
						
						//  packet testing
						//PrintRangingPacket(pkt_temp);

						// send the encoded pkt up to ARM	byte by byte
						char * bufRadio2Arm = (char *)pkt_temp;
						int i;
						putchar(START_BYTE);
						for (i = 0; i< 9; i++)
						{
								if (bufRadio2Arm[i] == START_BYTE || bufRadio2Arm[i] == ESC_BYTE || bufRadio2Arm[i] == STOP_BYTE)
								{
										putchar(ESC_BYTE);
								}
								putchar(bufRadio2Arm[i]);
						}
						putchar(STOP_BYTE);		
	
				break;

default:				break;
	}
}

/**
 * @brief Initialization of modul variables.
 *
 * Initialize the application variables. APPInit is called after
 * init/start of the lower layers.
 */
/***************************************************************************/
void APLInit(void)
/***************************************************************************/
{
	MyByte8T		s_address[] = {0,0,0,0,0,2};
	MyByte8T		d_address[] = {0,0,0,0,0,1};

    apl = &aplM;

	/*
     * for this simple demo transmitter and receiver
     * use the same MAC address. This way we need only
     * one executable for both stations.
     */
	memcpy (apl->src, s_address, 6);
	memcpy (apl->dest, d_address, 6);
	downMsg.prim = PLME_SET_REQUEST;
	downMsg.attribute = PHY_MAC_ADDRESS1;
	memcpy (downMsg.data, s_address, 6);
	PLMESap (&downMsg);

	/* These variables are used by the demo application.
     * They are used by the user interface
     */
    apl->hwclock = 0;
	apl->len = 0;

	/* switch on receiver */
	downMsg.prim = PLME_SET_REQUEST;
	downMsg.attribute = PHY_RX_CMD;
	downMsg.value = PHY_RX_ON;
	PLMESap (&downMsg);
}

/**
 * @brief Prepare message to send.
 *
 * This function initializes the message struct and copies the payload
 * to the payload buffer and requests a data transmission from the lower layer.
 */
/***************************************************************************/
void SendBuffer (void)
/***************************************************************************/
{
	// clear RTS
	memcpy (downMsg.addr, apl->dest, 6);
	downMsg.prim = PD_DATA_REQUEST;
	downMsg.len = apl->len;
	apl->len = 0;
	SendMsg (&downMsg);
	// Set RTS
}

void SendRange (void)
{
	// clear RTS
	memcpy (downMsg.addr, apl->dest, 6);
	downMsg.prim = PD_RANGING_REQUEST;
	apl->len = 0;
	SendMsg (&downMsg);
	// Set RTS
}

/**
 * @brief Monitors user input.
 *
 * This function monitors the keys on the board and collects user input from
 * the serial interface
 */
/***************************************************************************/
void APLPoll (void)
/***************************************************************************/
{
		//static	char buf[CONFIG_CONSOLE_LINE_SIZE];
		//static	int write_prompt = 1;
	
		//MyInt16T c;
	
		//static MyByte8T packets_sent = 0xFF;
		//MyByte8T power = 0;
		Packet * pktArm2Radio;


		// Get pkt from ARM --> only apply in sender
		if (__pkt_rx_flag)
		{
				// set src and dest based on pkt info
				pktArm2Radio = (Packet *)downMsg.data;
				memcpy(&(apl->dest[5]), &(pktArm2Radio->dest), 1);
				memcpy(&(apl->src[5]), &(pktArm2Radio->src), 1);
				//PrintPacket(pktArm2Radio);
				// decide with type of packet is sending
				if (pktArm2Radio->type == PKT_TYPE_DATA)
				{
						//printf("packet type: data \n");
						// send the data
						SendBuffer();
				} 
				else if (pktArm2Radio->type == PKT_TYPE_RANGING)
				{
						//printf("packet type: ranging \n");
						// send ranging pkt
						SendRange();
				}
				else if (pktArm2Radio->type == PKT_TYPE_SETTING)
				{
						printf("packet type: setting \n");			
				}
				else 
				{
						printf("packet type error: %c \n", pktArm2Radio->type);
				} 

				__pkt_rx_flag = 0;
		}


/* AVR 2 ARM Test
		// Send pkt from AVR to ARM
		Packet * pkt;
		pkt->id = 'a';
		pkt->dest = 'b';
		pkt->src = 'c';
		pkt->type = PKT_TYPE_DATA;
		pkt->checksum = 'e';
		pkt->length = 'f';
		pkt->data[0]='h';
		pkt->data[1]='i';
		char * buf2 = (char *)pkt;
	
		int i;
		putchar(START_BYTE);
		for (i = 0; i< 10; i++)
		{
				if (buf2[i] == START_BYTE || buf2[i] == ESC_BYTE || buf2[i] == STOP_BYTE)
				{
						putchar(ESC_BYTE);
				}
				putchar(buf2[i]);
		}
		putchar(STOP_BYTE);	
*/

}
