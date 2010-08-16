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

/**
 * @def Sf_packet_unsolicited_msg_tag
 * @brief messages going from radio to avr that aren't a response to a message are tagged with this value
 */
#define Sf_packet_unsolicited_msg_tag 255
//#define GUI 1
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

/** @brief enumeration for supporting debugging and other modes of operation */
typedef enum {
    terminal,
    chat,
    request_two_way_range,
    request_fast_range,
    listen_for_range_requests,
    spray,
    talk_to_ARM,
    send_incoming_chars
} AplMode;

/** @brief enumeration for distinguishing message types comming from the arm serial line */
typedef enum {
    no_op,
    ack,
    nack,
    send_msg,
    set_src_mac_address,
    set_dest_mac_address,
    get_net_stats,
    set_antenna_power,
    set_channel,
    set_give_acknowledgements,
    goto_chat_mode,
    request_2way_range,
    request_1way_range,
    range_listen,
    goto_send_mode,
    incomming_data,
    incomming_range_indication,
    incomming_data_but_source_changed
} radio_msg_type;

/** @brief enumeration for supporting debugging and other modes of operation */
typedef struct {
    unsigned int packets_received;
    unsigned int malformed_packets;
    unsigned int packets_timed_out;
} Network_Stats;


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
	AplMode    mode;		/**< Distinguish between configuration mode and communication mode */
	MyByte8T   help;		/**< Print the help text only once or when requested */
	MyByte8T   len;		/**< Number of bytes in send buffer */
	MyByte8T   key[4];		/**< Current key state */
	MyByte8T   tkey[4];	/**< Last key state to toggle the state */
	MyByte8T   skey_k1;
	MyByte8T   skey_k2;
	MyByte8T   skey_k3;
	Sf_packet  outpkt;
	int	   remaining_bytes_to_be_sent;  //when in send mode, we'll send this number of packets and then return to talk_to_ARM mode
	int	   inbound_packet_available_flag;
	int	   ranging_info_available_flag;
	int	   are_we_listening_for_range;
} AplMemT;

/** @brief Memory for all layer configuration settings.  */
AplMemT aplM;
/** @brief Pointer to memory for all layer configuration settings.  */
AplMemT *apl;
/** @brief Message structure for data requests.  */
MyMsgT downMsg;
/** @brief Message structure for payload of received ranging packets.  */
RangingPIB *upRangingMsg;

/** @brief structure for maintaining network statistics */
static Network_Stats network_stats;
MyInt16T packets_received;

/* Global buffers */
Sf_packet  ranging_info_buffer;   /* buffer to receive incomming ranging requests, need this because applcallback can be executed within an interrupt */
Sf_packet  inpkt;       /* buffer to receive incomming data messages from the air */


/* @brief Send string to ARM
 * Helper function that makes writing to the ARM less cumbersome by wrapping our message contents around an envelope
 * Returns 0 on success, nonzero on failure. 
 */
int sendToARM (char *message, int msg_len, radio_msg_type msg_type, char tag) {
   if (msg_len > 256 - Sf_packet_header_overhead - sizeof(crc) ) {
      puts ("Err: avr to arm msg too long");
      return 1;
   }
   apl->outpkt.header_soh = '\x1';
   apl->outpkt.size = msg_len + Sf_packet_header_overhead + sizeof(crc);
   apl->outpkt.type = msg_type;
   apl->outpkt.tag = tag;
   if (message != apl->outpkt.params)
   	memcpy (apl->outpkt.params, message, msg_len);
   //debug start - check the input of the crc
   //int i;
   //printf ("->");
   //for (i=0; i< msg_len + envelope_overhead - sizeof(crc); i++) {
   //   printf ("%x ", (  (unsigned char *) &apl->outpkt )  [i]);
   //}
   //puts ("<-");
   //debug end
   apl->outpkt.params[msg_len] = crcFast ((unsigned char *) &apl->outpkt, msg_len + Sf_packet_header_overhead, 0 ); //TODO: return value of crcFast depends on width of crc check, which might change, so left side of this line needs to be rewritten to support that 
   if (apl->outpkt.size < Sf_packet_header_overhead + sizeof(crc) ) {
      puts ("Error: avr to arm msg impossibly shrt\n");
      return 1;
   }
   fwrite (& (apl->outpkt), 1, apl->outpkt.size, stdout);
   return 0;
}


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
										/* no hwack received, ranging didnt start */
										if ( talk_to_ARM == apl->mode ) {
											/* We would be here if our dest radio wasn't listening for our range request, but if that's the case we don't need to tell that to the ARM we'll just let it timeout */
											//char* replymsg = "dest didn't reply for rng"; //commented to save space in the string memory section.
											//sendToARM( replymsg, strlen(replymsg) + 1, nack, Sf_packet_unsolicited_msg_tag ); 
										}
										else {
											//sprintf(serial_print_buffer,"#%07.2f:%03i:%03i\n",upRangingMsg->distance,msg->addr[0],upRangingMsg->error);
											sprintf(serial_print_buffer,"confirmation %07.2f,%03i\n",upRangingMsg->distance, upRangingMsg->error);
											printf("%s",serial_print_buffer);
											//ranging_start_flag = TRUE;
										}
										break;
									case PHY_BUSY 		:
									case PHY_BUSY_TX 	:
										/* measurement is allready running (BUSY), wait
											for PD_RANGING_INDICATION first! */
										break;
									case PHY_CONFIGURATION_ERROR :
										if (terminal == apl->mode ) break;
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
								if (talk_to_ARM == apl->mode)
								{	
									if ( ! apl->inbound_packet_available_flag ) {
										apl->inbound_packet_available_flag = 1;
										if (  0 !=  memcmp (msg->addr, lastAddr, 6) ) {
											memcpy (lastAddr, msg->addr, 6);
											int address_size = sizeof(msg->addr[5]);
											memcpy( inpkt.params, &msg->addr[5], address_size );
											inpkt.params[ address_size ] = ':';
											memcpy( &inpkt.params[ address_size + 1 ], msg->data, msg->len );
	    										//sprintf( inpkt.params, "from %d: got %d bytes", msg->addr[5], msg->len );
											inpkt.size = msg->len + address_size + 1; // we add one because of the ":" character we use to separate the address from the data when getting information from a new source. This is an unorthodox use of Sf_packet structure, the member "size" of this struct is really supposed to mean the size of the whole message with header and checksum included but in this case we are just using it as a buffer to hold incomming info.
											inpkt.type = incomming_data_but_source_changed;
										} else { //don't send data source info if it's same as last time
											memcpy( inpkt.params, msg->data, msg->len );
											inpkt.size = msg->len; // See comment above
											inpkt.type = incomming_data;
										}
									}
									/*
									if (memcmp (msg->addr, lastAddr, 6) != 0)
									{
										memcpy (lastAddr, msg->addr, 6);
										printf ("\n%d> ", msg->addr[5]);
									}
									for (i = 0; i < msg->len; i++)
									{
										putchar (msg->data[i]);
									}
									*/
								}
								if (chat == apl->mode)
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
								if (listen_for_range_requests == apl->mode)
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

		case PD_RANGING_INDICATION:  // These cases trigger when we sent a ranging request and get a reflected response back.
		case PD_RANGING_FAST_INDICATION:
								if (terminal == apl->mode) break;
								if ( talk_to_ARM == apl->mode ) {
									LED0 (LED_OFF);
									upRangingMsg = (RangingPIB*) msg->data;
									if (  0  ==  upRangingMsg->error ) {
										if ( 0 == apl->ranging_info_available_flag ) {
											//sprintf(serial_print_buffer,"#%07.2f:%03i:%03i\n",upRangingMsg->distance,msg->addr[0],upRangingMsg->error);
											memcpy( &ranging_info_buffer.params, &upRangingMsg->distance, sizeof(upRangingMsg->distance)  );
											//sprintf( ((char*)&ranging_info_buffer.params) + sizeof(upRangingMsg->distance) ,"%07.2f",upRangingMsg->distance); // This line was supposed to append the ascii representation of the distance to the binary one, so we could confirm we were decoding distance properly in the ARM side
											apl->ranging_info_available_flag = 1;
											//printf("%s",serial_print_buffer);
											//ranging_start_flag = TRUE;
										}
									}
								}
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
	MyByte8T		s_address[] = {0,0,0,0,0,1};
	MyByte8T		d_address[] = {0,0,0,0,0,2};

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
	apl->mode = talk_to_ARM;
	apl->help = 0;
	apl->len = 0;
	apl->inbound_packet_available_flag = 0;

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
 * @brief Communicate the Clear To Send signal to the ARM, signaling when we are ready to accept packets for transmission, and when we need a small break.
 *
 * This function puts the clear to send pin to high or low, depending on the argument
 * .
 */
/***************************************************************************/
void setCTS (int new_state)
/***************************************************************************/
{
	switch (new_state) {
	   case 0:
	      CTS_PORT &= ~CTS_PIN;
	      break;
	   case 1:
              CTS_PORT |= CTS_PIN;
	      break;
	   default: //we should never reach here
	   //puts("Error: incorrect cts signal"); //commented out to save string space
	   break;
	}
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
	setCTS(0);
	memcpy (downMsg.addr, apl->dest, 6);
	downMsg.prim = PD_DATA_REQUEST;
	downMsg.len = apl->len;
	apl->len = 0;
	SendMsg (&downMsg);
        setCTS(1);
}


void SendRange (void)
{
	setCTS(0);
	memcpy (downMsg.addr, apl->dest, 6);
	downMsg.prim = PD_RANGING_REQUEST;
	apl->len = 0;
	SendMsg (&downMsg);
	setCTS(1);
}

void SendFastRange (void)
{
	setCTS(0);
	memcpy (downMsg.addr, apl->dest, 6);
	downMsg.prim = PD_RANGING_FAST_REQUEST;
	apl->len = 0;
	SendMsg (&downMsg);
	setCTS(1);
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

        static Sf_packet pkt;

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

	// "I'm alive" type messages
	// puts("alive"); //enable just to test if radio is comming through this part of execution (should do it all the time).
	//if (apl->hwclock + 15000  <  hwclock()) {
	//	apl->hwclock = hwclock();
	//	puts("alive");
	//}
	

        if (apl->mode == talk_to_ARM)
	{
            if (apl->help == 0) {
	    	sprintf(apl->outpkt.params, "rdy");
		sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, Sf_packet_unsolicited_msg_tag);
		CTS_PORT |= CTS_PIN;
		apl->help = 1;
            }
            Read_sf_status read_oper = readSfPacket(&pkt);
            switch(read_oper.state) {
                case no_packet: // here's where we send unsolicited messages
                    //printf("no_packet, nbytes = %d\n", read_oper.nbytes);  //enable only on verbose debug
		    if ( apl->ranging_info_available_flag ) {
			sendToARM (ranging_info_buffer.params, 4, incomming_range_indication, Sf_packet_unsolicited_msg_tag); // 4 is the sizeof a RangingPIB.distance element
			 // sendToARM (ranging_info_buffer.params, 4 + strlen(ranging_info_buffer.params+4) + 1, incomming_range_indication, Sf_packet_unsolicited_msg_tag); //  This line was used when we were appending the ascii representation of the distance to the binary representation of the distance in the packet sent to the ARM, for debugging purposes. We add 4 to some pointers here because 4 bytes is the length of a RangingPIB.distance element.
			apl->ranging_info_available_flag = 0;
		    } 
		    if ( apl->inbound_packet_available_flag ) {
		    	if ( incomming_data_but_source_changed == inpkt.type ) 
		    		sendToARM (inpkt.params, inpkt.size, incomming_data_but_source_changed, Sf_packet_unsolicited_msg_tag);
			else
				sendToARM (inpkt.params, inpkt.size, incomming_data, Sf_packet_unsolicited_msg_tag);
			apl->inbound_packet_available_flag = 0;
		    }
                    return;
		    break;
                case waiting_for_rest_of_packet:
                    //printf("waiting for rest of packet, nbytes = %d\n", read_oper.nbytes);  //enable only on verbose debug
                    return;
		    break;
                case receiving_packet:
                    //printf("receiving packet, nbytes = %d\n", read_oper.nbytes); //enable only on verbose debug
                    return;
		    break;
                case packet_received:
                    network_stats.packets_received++;
                    //printf("packet received, size=%d, type=%d, tag=%d, nbytes=%d payload:%s.\n", pkt.size, pkt.type, pkt.tag, read_oper.nbytes, &pkt.params[0]); //enable on debug
                    break;
                case malformed_packet:
                    network_stats.malformed_packets++;
                    //printf("malformed packet, nbytes = %d\n", read_oper.nbytes); //enable on debug
                    return;
		    break;
                case packet_timed_out:
                    network_stats.packets_timed_out++;
		    //printf("Packet timed out, nbytes = %d\n", read_oper.nbytes); //enable on debug
		    return;
		    break;
                default:
                    printf("invalid status %d,%d\n", read_oper.state, read_oper.nbytes);
                    return;
		    break;
            }

	    
            /* select the command requested by the ARM and execute it */
            if (no_op != pkt.type)  
            {
		    if (goto_send_mode == pkt.type) { //parameters: integer (ascii)   (number of bytes to be transmitted)
			apl->remaining_bytes_to_be_sent = atoi(pkt.params);
			apl->len = 0;
			sprintf (apl->outpkt.params, "ok send");
		        sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, pkt.tag);
			if (0 < apl->remaining_bytes_to_be_sent)
			   apl->mode = send_incoming_chars;
		    }
                    else if (set_src_mac_address == pkt.type)  //parameters:  integer (ascii)   (our new mac address)
                    {
                            apl->src[5] = atoi (pkt.params);
                            // NTRXSetStaAddress (app->src);
                            downMsg.prim = PLME_SET_REQUEST;
                            downMsg.attribute = PHY_MAC_ADDRESS1;
                            memcpy (downMsg.data, apl->src, 6);
                            PLMESap (&downMsg);
			    sprintf (apl->outpkt.params, "src %s", pkt.params);
		            sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, pkt.tag);
                    }
                    else if (set_dest_mac_address == pkt.type)  //parameters:  integer (ascii)  (sets  destination mac address for outbound packets)
                    {
                            apl->dest[5] = atoi (pkt.params);
			    sprintf (apl->outpkt.params, "dest %s", pkt.params);
		            sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, pkt.tag);

                    }
                    else if (set_channel == pkt.type) //parameters:  integer (ascii)   (sets channel)
                    {
                            downMsg.value = (MyByte8T)(atoi (pkt.params));
                            downMsg.prim = PLME_SET_REQUEST;
                            downMsg.attribute = PHY_LOG_CHANNEL;
                            PLMESap (&downMsg);
                            if (downMsg.status == PHY_SUCCESS)
                            {
                                   sprintf (apl->outpkt.params, "channel %s", pkt.params);
		            	   sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, pkt.tag);
                            }
                            else
                            {
				   sprintf (apl->outpkt.params, "Error: illegal channel: %s", pkt.params);
		            	   sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, nack, pkt.tag);
                            }
                    }
                    else if (set_antenna_power == pkt.type)  //parameters: <1 to 8> (ascii)  (the new antenna power)
                    {
                            power = (MyByte8T)(atoi (pkt.params));
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
                                    sprintf (apl->outpkt.params, "antenna power %s", pkt.params);
		            	    sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, pkt.tag);
                            }
                            else
                            {
                                    sprintf (apl->outpkt.params, "Error: illegal antenna power:%s", pkt.params);
		            	    sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, nack, pkt.tag);
                            }
                    }
                    else if (set_give_acknowledgements == pkt.type)  //parameters:  <1|0> (ascii)  (enables or disables responding with acknowledgements to devices that write to us)
                    {
                            downMsg.value = (MyByte8T)(atoi (pkt.params));
                            downMsg.prim = PLME_SET_REQUEST;
                            downMsg.attribute = PHY_ARQ;
                            PLMESap (&downMsg);
                            if (downMsg.status == PHY_SUCCESS)
                            {
				    sprintf (apl->outpkt.params, "acks %s", pkt.params);
		            	    sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, pkt.tag);
                            }
                            else
                            {
                                    sprintf (apl->outpkt.params, "Error: invalid ack setting");
		            	    sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, nack, pkt.tag);
                            }
                    }
                    else if (get_net_stats == pkt.type) //parameters: none
                    {        
			    sprintf (apl->outpkt.params, "stats: src %d dest %d", 
			       apl->src[5],
			       apl->dest[5]
			    );
		            sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, pkt.tag);
                    }
                    else if (goto_chat_mode == pkt.type) //parameters: none
                    {
			    sprintf (apl->outpkt.params, "chat");
		            sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, pkt.tag);
                            apl->mode = chat;
                            /* switch on receiver */
                            downMsg.prim = PLME_SET_REQUEST;
                            downMsg.attribute = PHY_RX_CMD;
                            downMsg.value = PHY_RX_ON;
                            PLMESap (&downMsg);
		    }
                    else if (request_2way_range == pkt.type) //parameters: none  (command for sending range packets, range is calculated on the way to the bouncer node, and also on the way back)
                    {
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

		            sprintf (apl->outpkt.params, "2way rng");
		            sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, pkt.tag); //we acknowledge the command first to prevent the possibility that the ARM gets a range response before the acknowledgement for the command.
			    SendRange();
                    }
                    else if (request_1way_range == pkt.type)  //parameters: none (command sends range request packets, range is calculated only on the way to the bouncer node (or on the way back, not sure, but only in one of the two)
                    {
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
		            
			    sprintf (apl->outpkt.params, "1way rng");
		            sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, pkt.tag); //we acknowledge the command first to prevent the possibiltity that the ARM gets a range response before the acknowledgement for the command.
			    SendFastRange();	
                    }
                    else if (spray == pkt.type) //parameters: none  (sprays messages to the destination mac address, used for measure of pkt loss)
                    {
                            apl->mode = spray;
                            packets_received = 0;

                            /* switch on receiver */
                            downMsg.prim = PLME_SET_REQUEST;
                            downMsg.attribute = PHY_RX_CMD;
                            downMsg.value = PHY_RX_ON;
                            PLMESap (&downMsg);
		            sprintf (apl->outpkt.params, "spray");
		            sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, pkt.tag);
                    }
                    else if (range_listen == pkt.type)  //parameters: none  (command for awaiting range packets)
                    {
                            /* switch on receiver */
                            downMsg.prim = PLME_SET_REQUEST;
                            downMsg.attribute = PHY_RX_CMD;
                            downMsg.value = PHY_RX_ON;
                            PLMESap (&downMsg);
			    sprintf (apl->outpkt.params, "rng listen");
		            sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, pkt.tag);
                    }
                    else if (send_msg == pkt.type)  { //parameters:  the data to send over-the-air, may be binary.
		        //TODO: react if message from uart doesn't fit into a single radio message
			apl->len = pkt.size - Sf_packet_header_overhead - sizeof(crc);
			memcpy (downMsg.data, pkt.params, apl->len ); //need a checksum here for the data that will be transmitted over-the-air
			SendBuffer();
			sprintf (apl->outpkt.params, "msg sent");
			sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, pkt.tag);
		    }
                    else {
		        sprintf(apl->outpkt.params, "unknown command:%d", pkt.type);
			sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, nack, pkt.tag);
                    }
		    downMsg.len = 0;
            }
	}
        else if (terminal == apl->mode)
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
			printf ("ptes 			packet loss test | send packets\n");
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
					puts ("illegal pow val");
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
					puts ("invalid axk val");
				}
			}

			else if (strncmp (buf, "help", 4) == 0)
			{
				apl->help = 0;
			}
			else if (strncmp (buf, "chat", 4) == 0)
			{
				apl->mode = chat;
				/* switch on receiver */
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_RX_CMD;
				downMsg.value = PHY_RX_ON;
				PLMESap (&downMsg);
			}
			else if (strncmp (buf, "rang", 4) == 0)
			{
				apl->mode = request_two_way_range;
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
				apl->mode = request_fast_range;

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
				apl->mode = spray;
				packets_received = 0;

				/* switch on receiver */
				downMsg.prim = PLME_SET_REQUEST;
				downMsg.attribute = PHY_RX_CMD;
				downMsg.value = PHY_RX_ON;
				PLMESap (&downMsg);
			}
			else if (strncmp (buf, "plis", 4) == 0)
			{
				apl->mode = listen_for_range_requests;
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
	else if (chat == apl->mode)
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
				apl->mode = terminal;
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
	else if (send_incoming_chars == apl->mode)
	{
		while (kbhit()) {
			//expect 1 byte less
			apl->remaining_bytes_to_be_sent --;
			//put byte in buffer
			c = getchar();
			downMsg.data[apl->len++] = (MyByte8T)c;
			//if buffer is full or this is the last byte, then send buffer (this will reset buffer index)
			if (   ( apl->len >= CONFIG_PAYLOAD_LEN - 1 )   ||    ( apl->remaining_bytes_to_be_sent == 0 )   )  {
				SendBuffer ();
			}
			//if we are done expecting bytes, exit send mode
			if (0 == apl->remaining_bytes_to_be_sent) {
				sprintf (apl->outpkt.params, "sent");
		        	sendToARM (apl->outpkt.params, strlen(apl->outpkt.params) + 1, ack, Sf_packet_unsolicited_msg_tag);
				apl->mode = talk_to_ARM;
			}
		}	
	}
        else if (request_two_way_range == apl->mode)
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
			apl->mode = terminal;
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
        else if (request_fast_range == apl->mode)
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
				apl->mode = terminal;
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
	else if (spray == apl->mode)
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
			apl->mode = terminal;
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
        else if (listen_for_range_requests == apl->mode)
	{
		while (kbhit())
		{
			c = getchar ();
			if (c == 0x1b)
			{
				apl->mode = terminal;
				apl->len = 0;
				puts ("end listen");
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
