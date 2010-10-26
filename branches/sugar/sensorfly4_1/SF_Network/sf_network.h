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
    
#define PKT_TYPE_RANGING 'r'
#define PKT_TYPE_DATA    'd'
#define PKT_TYPE_SETTING 's'

typedef struct
{
  uint8_t id;
  uint8_t type;	// r--> ranging; d--> data; s-->setting 
  uint8_t checksum;
  uint8_t dest; // with 6 characters MAC Address
  uint8_t src;	// with 6 characters MAC Address
  uint8_t length;
  int16_t data[10];
} Packet;


void sf_network_init();
Packet * sf_network_pkt_receive();
void sf_network_pkt_send(Packet *pkt);
void sf_network_pkt_release();


#endif /* SF_NETWORK_H_ */