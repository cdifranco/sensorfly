/*
 * radio.h
 *
 *  Created on: Oct 2, 2009
 *      Author: Aveek
 */

#ifndef RADIO_H_
#define RADIO_H_

#define RADIO_RX_ERROR   2
#define RADIO_RX_NONE    3
#define RADIO_RX_SUCCESS 4
#define RADIO_TX_ERROR	 5
#define RADIO_TX_SUCCESS 6

// MSG TYPES
#define RADIO_MSGTYPE_DATA 7
#define RADIO_MSGTYPE_RANGEREQ 8
#define RADIO_MSGTYPE_RANGE 9

// PACKET HEADER & BODY
#define MAC 		0
#define TYPE 		6
#define DATA_LEN 	7
#define DATA 		8

#define RADIO_PKT_HEADER_LEN 8

#define RADIO_MAX_DATA_LEN 128
#define RADIO_MAX_PKT_LEN 254

// Reserved characters
#define START 193
#define END 192
#define ESC 219

#define BUFSIZE		0xFF

extern uint8_t RadioRxBuffer[BUFSIZE];
extern uint8_t RadioPacketReceived;
extern uint8_t RadioPacketLength;

int8_t SendData(uint8_t *mac, uint8_t data_len, uint8_t *buf);
int8_t SendRangingRequest(uint8_t *mac, uint8_t *buf);

void RadioInit();
uint8_t RadioGetPacket(uint8_t *rx_buf, uint8_t *len);
void RadioPacketRelease();
uint8_t isRadioPacketReceived();

int8_t SendSerialPacket(uint8_t pkt_len, uint8_t *send_pkt);

#endif /* RADIO_H_ */
