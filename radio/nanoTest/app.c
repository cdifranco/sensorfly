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
#include	"packet.h"
#include	"app.h"
#include	"phy.h"
#include 	"nnspi.h"
#include	"led.h"

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
uint8_t temp[16];
int ct = 0;
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
uint8_t src_addr;
/**
 * @brief Process incoming messages.
 * @param *msg this is the message pointer
 *
 * This function receives all incoming messages from the lower layer
 * and sends the payload to the serial interface.
 */
void APLCallback (MyMsgT *msg)
{
#ifdef RTS_CTS_ENABLE
	int ctsState;
	if(CTS_PORT & CTS_PIN)
		ctsState = 1;
	else
		ctsState = 0;
	if(ctsState)
	{
		RTSAndWaitCTS ();
		CTSSet(0);
	}
#endif

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
						break;
					case PHY_NO_ACK		:
						/* no hwack received, ranging didnt start */
						cli();
						sprintf(serial_print_buffer,"%07.2f,%03i",upRangingMsg->distance, upRangingMsg->error);
						printf("%s %d \r\n",serial_print_buffer,apl->dest[5]);
						sei();
						break;
					case PHY_BUSY 		:
					case PHY_BUSY_TX 	:
						/* measurement is allready running (BUSY), wait
							for PD_RANGING_INDICATION first! */
						break;
					case PHY_CONFIGURATION_ERROR :
						/* driver isnt correct initialized for ranging */						
						cli();
						sprintf(serial_print_buffer,"%07.2f,%03i",upRangingMsg->distance, upRangingMsg->error);
						printf("%s \r\n",serial_print_buffer);
						sei();
						break;
					default : break;
				}
				break;

		case PD_DATA_INDICATION:
					// Check if packet is for this node
					pkt_rx = (Packet *)msg->data;					
					if(memcmp(msg->rxAddr,apl->src,6) != 0)
					{
							cli();
							printf("msg for %d and addr is %d \r\n",msg->rxAddr[5],apl->src[5]);
							sei();
							break;
					}
					// Check length of packet
					if (memcmp(&(msg->len),&(pkt_rx->length),1) != 0)
					{
							cli();
							printf("length of the pkt is not consistent, should be %d but only get %d \n",pkt_rx->length,msg->len);
							sei();
							break;
					}
				  cli();
					// PrintPacket(pkt_rx);
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
					sei();
					
				break;
								
		case PD_RANGING_INDICATION:
		case PD_RANGING_FAST_INDICATION:
					// getting the ranging result data
					upRangingMsg = (RangingPIB*) msg->data;
					cli();
					PrintRangingLog(src_addr, apl->dest[5], upRangingMsg);
					sei();
					break;

		default:		break;
	}

#ifdef RTS_CTS_ENABLE
	if(ctsState)
			CTSSet(1);
#endif
}

/**
 * @brief Initialization of modul variables.
 *
 * Initialize the application variables. APPInit is called after
 * init/start of the lower layers.
 */
void APLInit(void)
{

	MyByte8T		s_address[] = {0,0,0,0,0,0};
	MyByte8T		d_address[] = {0,0,0,0,0,0};
	apl = &aplM;
	memcpy(apl->src, s_address, 6);
	memcpy(apl->dest, d_address, 6);

	// send reset signal to ARM
	putchar(START_BYTE);
  putchar(RESET_BYTE);
  putchar(STOP_BYTE);
   	
	/* These variables are used by the demo application.
	 * They are used by the user interface
	 */
	apl->hwclock = 0;
	apl->len = 0;

	/* switch on receiver */
	SetStartComm();

#ifdef RTS_CTS_ENABLE
	CTSSet(1);
#endif
}

void APLPoll (void)
{
	if (__pkt_rx_flag)
	{
#ifdef RTS_CTS_ENABLE
		RTSAndWaitCTS ();
		CTSSet(0);
#endif
		Packet *pktArm2Radio = (Packet *)downMsg.data;
		// check if the packet's format is correct
		if (pktArm2Radio->checksum != 0)
		{
				sendFailAck();
		}
		else
		{// checksum is correct: ADD CHECKSUM !!!
				apl->dest[5] = pktArm2Radio->dest;
				PrintPacketLog(pktArm2Radio);
				if (pktArm2Radio->type == PKT_TYPE_DATA)
				{
						// send the data packet
						pktArm2Radio->src = apl->src[5];			
						SendBuffer();
						// inform ARM that the transform is correct	
						sendSuccAck();						
				}
				else if (pktArm2Radio->type == PKT_TYPE_RANGING)
				{
						// send ranging pkt
						SendRange();
						// inform ARM that the transform is correct	
						sendSuccAck();
				}
				else if (pktArm2Radio->type == PKT_TYPE_SETTING)
				{
						// setting src address
						SetAVR(pktArm2Radio);
						// inform ARM that the transform is correct	
						sendSuccAck();
				}
				else if (pktArm2Radio->type == PKT_TYPE_TERMINAL)
				{
						// terminal mode
						if (pktArm2Radio->data[0] = START_SIGNAL)
						{
								apl->dest[5] = pktArm2Radio->dest;
								src_addr = pktArm2Radio->src;
								SendRange();
						}
						apl->len = 0;			
				}
				else
				{
						apl->len = 0;
						//printf("packet type error: %c \n", pktArm2Radio->type);
						sendFailAck();
				}
		}
		__pkt_rx_flag = 0;

#ifdef RTS_CTS_ENABLE
		CTSSet(1);
#endif
	}
}

void sendSuccAck(void)
{
		putchar(START_BYTE);
		putchar(SUCC_BYTE);
		putchar(STOP_BYTE);
		//printf("success");	
}

void sendFailAck(void)
{
		//printf("fail: ");		
		putchar(START_BYTE);
		putchar(FAIL_BYTE);
		putchar(STOP_BYTE);
		apl->len = 0;
}

void CTSSet (int new_state)
{
	switch (new_state) {
	   case 0:
	      CTS_PORT &= ~CTS_PIN;
	      break;
	   case 1:
          CTS_PORT |= CTS_PIN;
	      break;
	   default: //we should never reach here
	   break;
	}
}

void RTSAndWaitCTS (void)
{
	volatile uint32_t flag;
	do
	{
		flag = PINA;
	}while(!(flag & RTS_PIN));
}

void SendBuffer (void)
{
	memcpy (downMsg.addr, apl->dest, 6);
	downMsg.prim = PD_DATA_REQUEST;
	downMsg.len = apl->len;
	apl->len = 0;
	SendMsg (&downMsg);
}

void SendRange (void)
{
	memcpy (downMsg.addr, apl->dest, 6);
	downMsg.prim = PD_RANGING_REQUEST;
	apl->len = 0;
	SendMsg (&downMsg);
}

void SetAVR(Packet *setPkt)
{
	apl->len = 0;
	SetSrcAddr (setPkt->src);
}

void SetSrcAddr (uint8_t src)
{
	apl->src[5] = src;
	downMsg.prim = PLME_SET_REQUEST;
	downMsg.attribute = PHY_MAC_ADDRESS1;
	memcpy (downMsg.data, apl->src, 6);
	PLMESap (&downMsg);
}

void SetStartComm(void)
{
	/* switch on receiver */
	downMsg.prim = PLME_SET_REQUEST;
	downMsg.attribute = PHY_RX_CMD;
	downMsg.value = PHY_RX_ON;
	PLMESap (&downMsg);
}



