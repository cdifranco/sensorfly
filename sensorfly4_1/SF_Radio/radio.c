/*
 * radio.c
 *
 *  Created on: Oct 2, 2009
 *      Author: Aveek
 */

#include "sensorfly.h"
#include "radio.h"
#include "uart.h"

uint8_t RadioRxBuffer[BUFSIZE];
uint8_t RadioPacketReceived;
uint8_t RadioPacketLength;

/**
 * Radio Init
 */
void RadioInit(){
	RadioPacketReceived = 0;
	RadioPacketLength = 0;
}

/**
 * Check if packet received
 */
uint8_t isRadioPacketReceived(){
	if(RadioPacketReceived == 1)
		return 1;
	return 0;
}

/**
 * Releases Rx Packet to allow more RX
 */
void RadioPacketRelease() {
	RadioPacketReceived = 0;
	RadioPacketLength = 0;
}

uint8_t RadioGetPacket(uint8_t *rx_buf, uint8_t *len) {

	int i = 0;

	if(RadioPacketReceived == 0)
		return RADIO_RX_NONE;
	if(RadioPacketLength == 0)
		return RADIO_RX_ERROR;

	for(i=0;i<RadioPacketLength;i++) {
		rx_buf[i] = RadioRxBuffer[i];
	}

	*len = RadioPacketLength;
	//Release Packet After Reading
	RadioPacketRelease();

	return RADIO_RX_SUCCESS;
}

/**
 * Send Packet
 */
int8_t SendSerialPacket(uint8_t pkt_len, uint8_t *send_pkt) {

	uint8_t val, i;
	uint8_t new_pkt_len = 0;

	if(pkt_len == 0)
		return RADIO_TX_ERROR;

	if(pkt_len > RADIO_MAX_PKT_LEN)
			return RADIO_TX_ERROR;

	uart0_putc(START);
	new_pkt_len++;

	for(i = 0; i < pkt_len; i++) {

		val = *send_pkt++;

		if((val == START) || (val == ESC) || (val == END)) {
			uart0_putc(ESC);
			new_pkt_len++;
		}

		uart0_putc(val);
		new_pkt_len++;
	}

	uart0_putc(END);
	new_pkt_len++;

	return RADIO_TX_SUCCESS;
}

/**
 * Send Ranging Request
 */
int8_t SendRangingRequest(uint8_t *mac, uint8_t *buf) {

	uint8_t pkt_len;

	// Write MAC Address
	buf[MAC] = mac[0];
	buf[MAC+1] = mac[1];
	buf[MAC+2] = mac[2];
	buf[MAC+3] = mac[3];
	buf[MAC+4] = mac[4];
	buf[MAC+5] = mac[5];

	// Type
	buf[TYPE] = RADIO_MSGTYPE_RANGEREQ;

	pkt_len = RADIO_PKT_HEADER_LEN;
	buf[DATA_LEN] = 0;

	return SendSerialPacket(pkt_len, buf);
}


/**
 * Send Data
 */
int8_t SendData(uint8_t *mac, uint8_t data_len, uint8_t *buf) {

	uint8_t pkt_len;

	// Write MAC Address
	buf[MAC] = mac[0];
	buf[MAC+1] = mac[1];
	buf[MAC+2] = mac[2];
	buf[MAC+3] = mac[3];
	buf[MAC+4] = mac[4];
	buf[MAC+5] = mac[5];

	// Type
	buf[TYPE] = RADIO_MSGTYPE_DATA;
	buf[DATA_LEN] = data_len;

	// Data already filled in

	pkt_len = RADIO_PKT_HEADER_LEN + data_len;
	return SendSerialPacket(pkt_len, buf);
}
