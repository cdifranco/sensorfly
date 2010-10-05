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


#define START_BYTE  0xFF
#define ESC_BYTE    0x1B
#define STOP_BYTE   0xEF   

/**
 * @def TIME_OUT
 * @brief Idle time before sending data.
 *
 * This value sets the idle time before the application stopps waiting for
 * new data and sends all collected data to the preset device.
 *
 */
#define TIME_OUT  40
/**
 * @def SendMsg
 * @brief Data request to the next lower layer.
 *
 * This function calls the next lower layer to request a data transmission.
 *
 */
#define SendMsg PDSap

/** @brief Structure type for all layer configuration settings
 *
 * To be IEEE layer complient, all configuration data is stored
 * in one data structure.
 */
typedef struct
{
	MyDword32T hwclock; 	/**< Timestamp to determine a timeout condition for a data request */
	MyAddrT    dest;		/**< MAC address of the peer device */
	MyAddrT    src;		/**< MAC address of this device */
	MyByte8T 	len;		/**< Number of bytes in send buffer */
} AplMemT;

/** @brief Memory for all layer configuration settings.  */
AplMemT aplM;
/** @brief Pointer to memory for all layer configuration settings.  */
AplMemT *apl;
/** @brief Message structure for data requests.  */
MyMsgT downMsg;
/** @brief Message structure for payload of received ranging packets.  */
RangingPIB *upRangingMsg;


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
						break;
					case PHY_NO_ACK		:
						/* no hwack received, ranging didnt start */
						//sprintf(serial_print_buffer,"#%07.2f:%03i:%03i\n",upRangingMsg->distance,msg->addr[0],upRangingMsg->error);
						sprintf(serial_print_buffer,"%07.2f,%03i\n",upRangingMsg->distance, upRangingMsg->error);
						printf("%s",serial_print_buffer);
						//ranging_start_flag = TRUE;
						break;
					case PHY_BUSY 		:
					case PHY_BUSY_TX 	:
						/* measurement is allready running (BUSY), wait
							for PD_RANGING_INDICATION first! */
						break;
					case PHY_CONFIGURATION_ERROR :
						/* driver isnt correct initialized for ranging */
						//sprintf(serial_print_buffer,"#%07.2f:%03i:%03i\n",upRangingMsg->distance,msg->addr[0],upRangingMsg->error);
						sprintf(serial_print_buffer,"%07.2f,%03i\n",upRangingMsg->distance, upRangingMsg->error);
						printf("%s",serial_print_buffer);
						//ranging_start_flag = TRUE;
						break;
					default : break;
				}
				break;

		case PD_DATA_INDICATION:
                    // Check if packet is for this node
                    if(memcmp(msg->addr,apl->src,6) != 0)
                    {
                       break;
                    }
                    
					// Check length of packet
					
					// Send packet to arm
					for (i = 0; i < msg->len; i++)
					{
						putchar (msg->data[i]);
					}
					
				break;
								
		case PD_RANGING_INDICATION:
		case PD_RANGING_FAST_INDICATION:
				LED0 (LED_OFF);
				upRangingMsg = (RangingPIB*) msg->data;

				//sprintf(serial_print_buffer,"#%07.2f:%03i:%03i\n",upRangingMsg->distance,msg->addr[0],upRangingMsg->error);
				sprintf(serial_print_buffer,"%07.2f,%03i\n",upRangingMsg->distance, upRangingMsg->error);
				printf("%s",serial_print_buffer);
				//ranging_start_flag = TRUE;
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
	static	char buf[CONFIG_CONSOLE_LINE_SIZE];
	static	int write_prompt = 1;
	
	MyInt16T c;
	
	static MyByte8T packets_sent = 0xFF;
	MyByte8T power = 0;
	
	/* In chat mode collect all user input and transmit it to the peer device */
	/** now only chat mode in this layer is needed*/
	if(!kbhit())
	{
		if ((apl->hwclock + TIME_OUT < hwclock ()) && (apl->len > 0))
		{
			// Timeout reset packet
		    apl->len = 0;
		}
		return;
	}
	/* reset timer */
	apl->hwclock = hwclock ();

	while (kbhit())
	{
		c = getchar ();
		
		// check if start byte
		// check if stop byte
		// remove escape
		
		
		/** print out what is sending out byte by byte (for sender is a must)*/
		//putchar (c);

		downMsg.data[apl->len++] = (MyByte8T)c;
	}

}
