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

#define GUI 1
#ifdef GUI
#	define GUI_CHAR putchar ('#')
#else
#	define GUI_CHAR
#endif
/**
 * @def TIME_OUT
 * @brief Idle time before sending data.
 *
 * This value sets the idle time before the application stopps waiting for
 * new data and sends all collected data to the preset device.
 *
 */
#define TIME_OUT  20
/**
 * @def PROMPT
 * @brief Prompt for command line.
 *
 * This string is used to indicate that the application is ready for new
 * input through the command line.
 *
 */
#define	PROMPT	"nn> "
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
	MyByte8T   mode;		/**< Distinguish between configuration mode and communication mode */
	MyByte8T   help;		/**< Print the help text only once or when requested */
	MyByte8T 	len;		/**< Number of bytes in send buffer */

	MyByte8T   key[4];		/**< Current key state */
	MyByte8T 	tkey[4];	/**< Last key state to toggle the state */
	MyByte8T   skey_k1;
	MyByte8T   skey_k2;
	MyByte8T   skey_k3;
} AplMemT;

/** @brief Memory for all layer configuration settings.  */
AplMemT aplM;
/** @brief Pointer to memory for all layer configuration settings.  */
AplMemT *apl;
/** @brief Message structure for data requests.  */
MyMsgT downMsg;
/** @brief Message structure for payload of received ranging packets.  */
RangingPIB *upRangingMsg;


MyInt16T packets_received;
MyInt16T packets_to_send;

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
	
	static MyByte8T lastAddr[6];
	
	static MyByte8T lastPacketTag = 0;
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
										if (apl->mode == 0) break;
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
										if (apl->mode == 0) break;
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
								if (apl->mode == 1)
								{
									if (memcmp (msg->addr, lastAddr, 6) != 0)
									{
										memcpy (lastAddr, msg->addr, 6);
										GUI_CHAR;
										printf ("\n%d> ", msg->addr[5]);
									}
									GUI_CHAR;
									for (i = 0; i < msg->len; i++)
									{
										putchar (msg->data[i]);
										if (msg->data[i] == 0x0d)
										{
											putchar (0x0a);
#											ifdef GUI
											putchar (0x0d);
#											endif
										}
									}
								}
								if (apl->mode == 5)
								{
									
									if(msg->data[0] != lastPacketTag)
									{ 
										lastPacketTag = msg->data[0];
										packets_received++;
										printf("%d packets received ", packets_received);
										printf("%d",msg->data[0]);
										printf("\n");
									}
								}
								break;
								
		case PD_RANGING_INDICATION:
		case PD_RANGING_FAST_INDICATION:
								if (apl->mode == 0) break;
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
	MyByte8T		s_address[] = {0,0,0,0,0,0};
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
	apl->mode = 1;
	apl->help = 0;
	apl->len = 0;

#	ifdef CONFIG_USE_KEYS
	apl->key[1] = FALSE;
	apl->key[2] = FALSE;
	apl->key[3] = FALSE;
	apl->tkey[1] = FALSE;
	apl->tkey[2] = FALSE;
	apl->tkey[3] = FALSE;
#	endif /* CONFIG_USE_KEYS */

	/* switch on receiver */
	downMsg.prim = PLME_SET_REQUEST;
	downMsg.attribute = PHY_RX_CMD;
	downMsg.value = PHY_RX_ON;
	PLMESap (&downMsg);

	// /* switch off receiver */
	// downMsg.prim = PLME_SET_REQUEST;
	// downMsg.attribute = PHY_RX_CMD;
	// downMsg.value = PHY_TRX_OFF;
	// PLMESap (&downMsg);
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

void SendFastRange (void)
{
	// clear RTS
	memcpy (downMsg.addr, apl->dest, 6);
	downMsg.prim = PD_RANGING_FAST_REQUEST;
	apl->len = 0;
	SendMsg (&downMsg);
	// Set RTS
}

#define CONFIG_ALIVE_LED 1
#ifdef CONFIG_ALIVE_LED
#include "led.h"
/**
 * @brief Periodicaly flash one LED.
 *
 * Hartbeat to indicate a normal running application
 */
/****************************************************************************/
void IsAlive (void)
/****************************************************************************/
{
	static	int	flag = 0;
	static	unsigned long	last = 0;
	unsigned long	now;

	now = hwclock();
	if(now - last > 500)
	{
		last = now;
		if(flag == 0)
		{
			flag = 1;
			LED_ALIVE (LED_ON);
		}
		else
		{
			flag = 0;
			LED_ALIVE (LED_OFF);
		}
	}
}
#endif

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
	
	//int i, j;

#	ifdef CONFIG_ALIVE_LED
	IsAlive ();
#	endif

#	ifdef CONFIG_USE_KEYS
	if ((key_k3() == TRUE) || (apl->skey_k1 == TRUE))
	{
		apl->skey_k1 = FALSE;
		if (apl->tkey[3] == FALSE)
		{
			apl->tkey[3] = TRUE;
			if (apl->key[3] == FALSE)
			{
				apl->key[3] = TRUE;
				LED5 (LED_ON);

				DDRF |= 0x02;
				PORTF |= 0x02;
				NTRXSetupTRxMode (NA_80MHz, NA_1us, NA_1M_S);
				PORTF &= ~0x02;
			}
			else
			{
				apl->key[3] = FALSE;
				LED5 (LED_OFF);
			}
		}
	}
	else
	{
		apl->tkey[3] = FALSE;
	}


	if ((key_k2() == TRUE) || (apl->skey_k2 == TRUE))
	{
		apl->skey_k2 = FALSE;
		if (apl->tkey[2] == FALSE)
		{
			apl->tkey[2] = TRUE;
			if (apl->key[2] == FALSE)
			{
				apl->key[2] = TRUE;
			}
			else
			{
				apl->key[2] = FALSE;
			}
		}
	}
	else
	{
		apl->key[2] = FALSE;
		apl->tkey[2] = FALSE;
	}

	if ((key_k1() == TRUE) || (apl->skey_k3 == TRUE))
	{
		apl->skey_k3 = FALSE;
		if (apl->tkey[1] == FALSE)
		{
			apl->tkey[1] = TRUE;
			if (apl->key[1] == FALSE)
			{
				apl->key[1] = TRUE;
				LED6 (LED_ON);
			}
			else
			{
				apl->key[1] = FALSE;
				LED6 (LED_OFF);
			}
		}
	}
	else
	{
		apl->tkey[1] = FALSE;
	}
#	endif /* CONFIG_USE_KEYS */

	//printf("Hi this is your radio\n");
	
	if (apl->mode == 0)
	{
		/* Help text for all available commands */
		if (apl->help == 0)
		{
			apl->help = 1;
			GUI_CHAR;
			printf ("Usage:\n");
			printf ("src  <mac addr>        set local MAC address\n");
			printf ("dest <mac addr>        set destination MAC address\n");
			printf ("sc <n>                 set channel [0 80MHz, 1us, 2.441GHz cf, no FEC]\n");
			printf ("                                   [1 22MHz, 4us, 2.412GHz cf, no FEC]\n");
			printf ("                                   [2 22MHz, 4us, 2.442GHz cf, no FEC]\n");
			printf ("                                   [3 22MHz, 4us, 2.472GHz cf, no FEC]\n");
			printf ("pow <value>			set power	[0 - 8 ( * 8), -33dBm to 0dBm]\n");
			printf ("ack <value>			set auto ack [0 False, 1 True]\n");

			printf ("chat                   start communication\n");
			printf ("rang                   send ranging request\n");
			printf ("ranf                   send fast ranging request\n");
			printf ("ptes 			        packet loss test | send packets\n");
			printf ("plis                   packet loss/ range test | listen packets\n");
			printf ("help                   this menu\n");
			printf ("[ESC]                  leave chat mode\n");
		}

		/* write a promt to mark configuration mode */
		if(write_prompt)
		{
			printf(PROMPT);
			write_prompt = 0;
		}

		if(!read_line(buf))
		{
			return;
		}

		write_prompt = 1;

		/* select the requested command by the user and execute it */
		if(buf[0] != 0)
		{
			if (memcmp (buf, "src ", 4) == 0)
			{
				apl->src[5] = atoi (&(buf[4]));
				// NTRXSetStaAddress (app->src);
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_MAC_ADDRESS1;
				memcpy (downMsg.data, apl->src, 6);
				PLMESap (&downMsg);
				puts ("Ok");
			}
			else if (memcmp (buf, "dest ", 5) == 0)
			{
				apl->dest[5] = atoi (&(buf[5]));
				puts ("Ok");

			}


			if (strncmp (buf, "sc ", 3) == 0)
			{
				downMsg.value = (MyByte8T)(atoi (&(buf[3])));
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_LOG_CHANNEL;
				PLMESap (&downMsg);
				if (downMsg.status == PHY_SUCCESS)
				{
					puts ("Ok");
				}
				else
				{
					puts ("illegal channel");
				}

			}
			else if (strncmp (buf, "pow ", 4) == 0)
			{
				power = (MyByte8T)(atoi (&(buf[3])));
				if (power != 0)
				{
					power = (power * 8) - 1;
				}
				downMsg.value = power;
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_TX_POWER;
				PLMESap (&downMsg);
				if (downMsg.status == PHY_SUCCESS)
				{
					puts ("Ok");
				}
				else
				{
					puts ("invalid value");
				}
			}
			else if (strncmp (buf, "ack ", 4) == 0)
			{
				downMsg.value = (MyByte8T)(atoi (&(buf[3])));
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_ARQ;
				PLMESap (&downMsg);
				if (downMsg.status == PHY_SUCCESS)
				{
					puts ("Ok");
				}
				else
				{
					puts ("invalid value");
				}
			}
			
			else if (strncmp (buf, "help", 4) == 0)
			{
				apl->help = 0;
			}
			else if (strncmp (buf, "chat", 4) == 0)
			{
				apl->mode = 1;
				/* switch on receiver */
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_RX_CMD;
				downMsg.value = PHY_RX_ON;
				PLMESap (&downMsg);
			}
			else if (strncmp (buf, "rang", 4) == 0)
			{
				apl->mode = 2;
				/* switch FEC on */
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_FEC;
				downMsg.value = FALSE;
				PLMESap (&downMsg);

				/* switch on receiver */
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_RX_CMD;
				downMsg.value = PHY_RX_ON;
				PLMESap (&downMsg);
			}
			else if (strncmp (buf, "ranf", 4) == 0)
			{
				apl->mode = 3;
				
				/* switch FEC on */
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_FEC;
				downMsg.value = FALSE;
				PLMESap (&downMsg);
				
				/* switch on receiver */
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_RX_CMD;
				downMsg.value = PHY_RX_ON;
				PLMESap (&downMsg);
			}
			else if (strncmp (buf, "ptes", 4) == 0)
			{
				apl->mode = 4;
				packets_received = 0;
				
				/* switch on receiver */
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_RX_CMD;
				downMsg.value = PHY_RX_ON;
				PLMESap (&downMsg);
			}
			else if (strncmp (buf, "plis", 4) == 0)
			{
				apl->mode = 5;
				packets_received = 0;
				
				/* switch on receiver */
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_RX_CMD;
				downMsg.value = PHY_RX_ON;
				PLMESap (&downMsg);
			}
			downMsg.len = 0;
		}
	}
	else if (apl->mode == 1)
	{
		/* In chat mode collect all user input and transmit it to the peer device */
		if(!kbhit())
		{
			if ((apl->hwclock + TIME_OUT < hwclock ()) && (apl->len > 0))
			{
				SendBuffer ();
			}
			return;
		}
		/* reset timer */
		apl->hwclock = hwclock ();
		while (kbhit())
		{
			c = getchar ();
			if (c == 0x1b)
			{
				apl->mode = 0;
				apl->len = 0;
				printf ("\nEnd of chat mode.\n");
				/* switch off receiver */
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_RX_CMD;
				downMsg.value = PHY_TRX_OFF;
				PLMESap (&downMsg);
				return;
			}

#			ifdef GUI
			if( c != 0x0d )
#			endif
				putchar (c);

			downMsg.data[apl->len++] = (MyByte8T)c;
		}

		if (apl->len >= CONFIG_PAYLOAD_LEN - 1)
		{
			SendBuffer ();
		}
	}
	else if (apl->mode == 2) 
	{
		/* In ptes mode sends 100 range to peer */
		
		if ((apl->hwclock + TIME_OUT < hwclock ()) && (packets_sent < 101))
		{
			packets_sent++;
			SendRange ();
			/* reset timer */
			apl->hwclock = hwclock ();
		}

		if(packets_sent >= 101) 
		{
			printf("Sent %d range reqs.\n", packets_sent-1);
			packets_sent = 0;
			apl->mode = 0;
			apl->len = 0;
			printf ("\nEnd of range test.\n");
			/* switch off receiver */
			downMsg.prim = PLME_SET_REQUEST;
			downMsg.attribute = PHY_RX_CMD;
			downMsg.value = PHY_TRX_OFF;
			PLMESap (&downMsg);
		}
		return;
		
	}
	else if (apl->mode == 3)
	{
			/* In ptes mode sends 100 range to peer */

			if ((apl->hwclock + TIME_OUT < hwclock ()) && (packets_sent < 101))
			{
				packets_sent++;
				SendFastRange ();
				/* reset timer */
				apl->hwclock = hwclock ();
			}

			if(packets_sent >= 101) 
			{
				printf("Sent %d range reqs.\n", packets_sent);
				packets_sent = 0;
				apl->mode = 0;
				apl->len = 0;
				printf ("\nEnd of range test.\n");
				/* switch off receiver */
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_RX_CMD;
				downMsg.value = PHY_TRX_OFF;
				PLMESap (&downMsg);
			}
			return;
	}
	else if (apl->mode == 4)
	{
		/* In ptes mode sends 100 packets to peer */
		
		if( (apl->len == 0) && (packets_sent < 100) )
		{
			downMsg.data[apl->len++] = packets_sent;
		}
		
		if ((apl->hwclock + TIME_OUT < hwclock ()) && (apl->len > 0))
		{
			packets_sent++;
			SendBuffer ();
			/* reset timer */
			apl->hwclock = hwclock ();
		}

		if(packets_sent >= 100) 
		{
			printf("Sent %d packets.\n", packets_sent);
			packets_sent = 0;
			apl->mode = 0;
			apl->len = 0;
			printf ("\nEnd of packet test.\n");
			/* switch off receiver */
			downMsg.prim = PLME_SET_REQUEST;
			downMsg.attribute = PHY_RX_CMD;
			downMsg.value = PHY_TRX_OFF;
			PLMESap (&downMsg);
		}
		return;

	}
	else if (apl->mode == 5)
	{
		while (kbhit())
		{
			c = getchar ();
			if (c == 0x1b)
			{
				apl->mode = 0;
				apl->len = 0;
				printf ("\nEnd of chat mode.\n");
				/* switch off receiver */
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_RX_CMD;
				downMsg.value = PHY_TRX_OFF;
				PLMESap (&downMsg);
				return;
			}
		}
	}
}
