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
// temp packet: for ranging info mostly
Packet temp_pkt;
Packet * pktAVR2ARM = &temp_pkt;
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
				pktAVR2ARM->data_double[0] = upRangingMsg->distance;
				pktAVR2ARM->data_double[1] = upRangingMsg->error;
				pktAVR2ARM->type = PKT_TYPE_RESULT;
				char * streamAVR2ARM = (char *)pktAVR2ARM;
				switch(msg->status)
				{
					case PHY_SUCCESS	:
						/* hwack received, ranging start successfully */
						break;
					case PHY_NO_ACK		:
						/* no hwack received, ranging didnt start */
						cli();
						// PrintRangingLog(src_addr, apl->dest[5], upRangingMsg);
						// sprintf(serial_print_buffer,"%07.2f,%03i",upRangingMsg->distance, upRangingMsg->error);
						// printf("%s, dest: %d, size of Packet: %d\r\n",serial_print_buffer,apl->dest[5],sizeof(Packet));
												
							putchar(START_BYTE);
							for (i = 0; i < sizeof(Packet); i++)
							{
									if (streamAVR2ARM[i] == START_BYTE || streamAVR2ARM[i] == ESC_BYTE || streamAVR2ARM[i] == STOP_BYTE)
									{
											putchar(ESC_BYTE);				
									}
									putchar (streamAVR2ARM[i]);
							}
							putchar(STOP_BYTE);
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
							//sprintf(serial_print_buffer,"%07.2f,%03i",upRangingMsg->distance, upRangingMsg->error);
							//printf("%s \r\n",serial_print_buffer);
							putchar(START_BYTE);
							for (i = 0; i < sizeof(Packet); i++)
							{
									if (streamAVR2ARM[i] == START_BYTE || streamAVR2ARM[i] == ESC_BYTE || streamAVR2ARM[i] == STOP_BYTE)
									{
											putchar(ESC_BYTE);				
									}
									putchar (streamAVR2ARM[i]);
							}
							putchar(STOP_BYTE);
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
					printf("print packet: msg length: %d \r\n",msg->len);
					PrintPacketLog(pkt_rx);

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
					pktAVR2ARM->data_double[0] = upRangingMsg->distance;
					pktAVR2ARM->data_double[1] = upRangingMsg->error;
					pktAVR2ARM->type = PKT_TYPE_RESULT;
					char * streamAVR2ARM = (char *)pktAVR2ARM;
					cli();
						// sprintf(serial_print_buffer,"%07.2f,%03i",upRangingMsg->distance, upRangingMsg->error);
						// printf("%s, dest: %d, size of Packet: %d\r\n",serial_print_buffer,apl->dest[5],sizeof(Packet));						
						// PrintRangingLog(src_addr, apl->dest[5], upRangingMsg);
						putchar(START_BYTE);
						for (i = 0; i < sizeof(Packet); i++)
						{
								if (streamAVR2ARM[i] == START_BYTE || streamAVR2ARM[i] == ESC_BYTE || streamAVR2ARM[i] == STOP_BYTE)
								{
										putchar(ESC_BYTE);				
								}
								putchar (streamAVR2ARM[i]);
						}
						putchar(STOP_BYTE);
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

	temp_pkt.id = 0;
	temp_pkt.type = PKT_TYPE_RANGING;
	temp_pkt.checksum = 0;
	temp_pkt.dest = 0;
	temp_pkt.src = 0;
	temp_pkt.length = sizeof(Packet);
	
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
		Packet *pktARM2AVR = (Packet *)downMsg.data;
		// check if the packet's format is correct
		if (pktARM2AVR->checksum != 0)
		{
				sendFailAck();
		}
		else
		{// checksum is correct: ADD CHECKSUM !!!
				apl->dest[5] = pktARM2AVR->dest;
				PrintPacketLog(pktARM2AVR);
				if (pktARM2AVR->type == PKT_TYPE_DATA)
				{
						// send the data packet
						pktARM2AVR->src = apl->src[5];			
						SendBuffer();
						// inform ARM that the transform is correct	
						sendSuccAck();						
				}
				else if (pktARM2AVR->type == PKT_TYPE_RANGING)
				{
						// send ranging pkt
						SendRange();
						// inform ARM that the transform is correct	
						sendSuccAck();
				}
				else if (pktARM2AVR->type == PKT_TYPE_SETTING)
				{
						// setting src address
						SetAVR(pktARM2AVR);
						// inform ARM that the transform is correct	
						sendSuccAck();
				}
				else if (pktARM2AVR->type == PKT_TYPE_TERMINAL)
				{
						// terminal mode
						if (pktARM2AVR->data_int[0] = START_SIGNAL)
						{
								apl->dest[5] = pktARM2AVR->dest;
								src_addr = pktARM2AVR->src;
								SendRange();
						}
						apl->len = 0;			
				}
				else
				{
						apl->len = 0;
						//printf("packet type error: %c \n", pktARM2AVR->type);
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
		printf("success");	
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



