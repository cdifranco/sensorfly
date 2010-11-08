/**
 * In this header, the definition related to the packet sending
 * is recorded
 * 09-28-2010
 */
#ifndef	_PACKET_H
#define	_PACKET_H

#define PKT_TYPE_RANGING 'r'
#define PKT_TYPE_DATA    'd'
#define PKT_TYPE_SETTING 's'
#include	"ntrxranging.h"

typedef struct _Packet
{
  uint8_t id;
  uint8_t checksum;
  uint8_t type;	// r--> ranging; d--> data; s-->setting
  uint8_t dest; // last char in 6 characters MAC Address
  uint8_t src;	// last char in 6 characters MAC Address
  uint8_t length;
  uint8_t data[10];
} Packet;

void PrintPacket(Packet *pkt);
void PrintPacketLog(Packet *pkt);
void PrintRangingLog(uint8_t src, uint8_t dest, RangingPIB * ranginginfo);

#endif /* PACKET_H */
