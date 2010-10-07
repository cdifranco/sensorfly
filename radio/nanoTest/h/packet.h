/**
 * In this header, the definition related to the packet sending
 * is recorded
 * 09-28-2010
 */
#ifndef	_PACKET_H
#define	_PACKET_H

/**
 * This is the packet that shall be sent as a whole
 * 09-28-2010
 */

#define PKT_TYPE_RANGING 'r'
#define PKT_TYPE_DATA    'd'
#define PKT_TYPE_SETTING 's'


typedef struct
{
  uint8_t id;
  uint8_t type;	// r--> ranging; d--> data; s-->setting
  uint8_t checksum;
  uint8_t dest; // last char in 6 characters MAC Address
  uint8_t src;	// last char in 6 characters MAC Address
  uint8_t length;
  uint8_t data[2];
} Packet;

void PrintPacket(Packet *pkt);

#endif /* PACKET_H */
