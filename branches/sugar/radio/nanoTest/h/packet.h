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


typedef unsigned char PByte8T;// Byte used in Packet  

typedef struct
{
  uint8_t id;
  uint8_t type;	// r--> ranging; d--> data; s-->setting
  uint8_t checksum;
  uint8_t dest; // with 6 characters MAC Address
  uint8_t src;	// with 6 characters MAC Address
  uint8_t length;
} Packet;

Packet *CreateTestingPacket(void);

void PrintPacket(Packet *pkt);

Packet *DecodePacket(char * pkt_bytes);

#endif /* PACKET_H */
