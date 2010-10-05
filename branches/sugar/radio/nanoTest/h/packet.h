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
  int16_t id;
  PByte8T type;	// r--> ranging; d--> data; s-->setting 
  PByte8T checksum;
  PByte8T dest[2]; // with 6 characters MAC Address
  PByte8T src[2];	// with 6 characters MAC Address
  PByte8T data[30];
} Packet;

Packet *CreateTestingPacket(void);

void PrintPacket(Packet *pkt);

Packet *DecodePacket(char * pkt_bytes);

#endif /* PACKET_H */
