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
#ifndef SF_NETWORK_H_
#define SF_NETWORK_H_

typedef enum {
    no_packet, 
    waiting_for_rest_of_packet, 
    receiving_packet, 
    packet_received, 
    malformed_packet, 
    packet_timed_out,
 } Read_Sf_packet_state;

 typedef enum {
    illegal_input_size,
    acknowledged,
    not_acknowledged,
    no_response
 } Write_Sf_packet_state;

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
    incomming_data,  // source is same as last source
    incomming_range_indication,
    incomming_data_but_source_changed  // source is prefixed to the data
} radio_msg_type;

typedef struct {
  char header_soh;
  char size;
  char type;
  char tag;
  char params [64 - 4]; //4 bytes are header overhead
} Sf_packet;



void sf_network_init();
void sf_network_wait_until_rx();
//void sf_network_tx_enqueue(); (too low-level, don't expose to the user)
//void sf_network_tx_send();    (too low-level, don't expose to the user)
Write_Sf_packet_state sf_network_send_msg (char * buf, int size);
Write_Sf_packet_state sf_network_set_mac_address (unsigned char address);
Write_Sf_packet_state sf_network_set_destination_mac_address (unsigned char address);
Write_Sf_packet_state sf_network_print_stats ();
Write_Sf_packet_state sf_network_put_radio_in_chat_mode ();
Write_Sf_packet_state sf_network_put_radio_in_listen_for_range_mode ();
Write_Sf_packet_state sf_network_send_range_request (); 
Write_Sf_packet_state sf_network_send_short_msg (char * buf, int size);
Read_Sf_packet_state sf_network_wait_until_packet_rx ( Sf_packet *output_area, char tag, unsigned int timeout );

typedef unsigned char crc;
#define CRC_WIDTH (8 * sizeof(crc))
#define CRC_TOPBIT (1 << (CRC_WIDTH - 1))
#define CRC_POLYNOMIAL 0xD8  /* 11011 followed by 0's */
crc crcFast (unsigned char message [], int message_len, crc previous_partial_cksum); /* From netrino.com */
#define Sf_packet_header_overhead 4
#define Sf_packet_unsolicited_message_tag 255



#endif /* SF_NETWORK_H_ */