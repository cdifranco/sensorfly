/**
 * In this header, the definition related to the packet sending
 * is recorded
 * 09-28-2010
 */
#ifndef	_PACKET_H
#define	_PACKET_H

#define PKT_TYPE_RANGING 	'g'
#define PKT_TYPE_RESULT 	'r'
#define PKT_TYPE_REQUEST 	'q'
#define PKT_TYPE_TERMINAL 	't'
#define PKT_TYPE_DATA    	'd'
#define PKT_TYPE_SETTING 	's'

#include	"ntrxranging.h"

typedef struct _Packet
{
  uint8_t id;
  uint8_t type;	// r--> ranging; d--> data; s-->setting
  uint8_t checksum;
  uint8_t dest; // last char in 6 characters MAC Address
  uint8_t src;	// last char in 6 characters MAC Address
  uint8_t length;
  uint16_t data_int[5];
  float data_double[2];
} Packet __attribute__((packed));

void PrintPacket(Packet *pkt);
void PrintPacketLog(Packet *pkt);
void PrintRangingLog(uint8_t src, uint8_t dest, RangingPIB * ranginginfo);
void PacketGeneration(Packet *pkt, uint8_t id, uint8_t type, uint8_t checksum, uint8_t dest, uint8_t src);
#endif /* PACKET_H */
