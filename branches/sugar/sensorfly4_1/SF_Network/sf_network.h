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

/**
  * Packet Part
  */
typedef unsigned char PByte8T;// Byte used in Packet  

typedef struct
{
  int16_t id;
  PByte8T type;	// r--> ranging; d--> data; s-->setting 
  PByte8T checksum;
  PByte8T dest[2]; // with 6 characters MAC Address
  PByte8T src[2];	// with 6 characters MAC Address
  PByte8T pktdata[6];
} Packet;


void sf_network_init();
void sf_network_wait_until_rx();
void sf_network_tx_enqueue();
void sf_network_tx_send();
Packet *sf_network_create_testing_packet();

#endif /* SF_NETWORK_H_ */