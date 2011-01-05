/**
 * Methods implements about packet
 */ 


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "packet.h"


void PrintPacket(Packet *pkt)
{
// print data in packet
		int i;
		printf("******Packet %u ******\r\n", pkt->id);
		printf("destination: ");
		for(i = 0; i < 6; i++)
		{
			if (i == 5) printf(" %d \r\n",pkt->dest);
			else printf(" %d :",0);
		}
		printf("source: ");
		for(i = 0; i < 6; i++)
		{
			if (i == 5) printf(" %u \r\n",pkt->src);
			else printf(" %u :",0);
		}
		printf("type: %c \r\n", pkt->type);
		for(i = 0; i < 4; i++)
		{
			printf("data: %d,",  pkt->data_int[i]);
		}
		printf("\r\n");
		printf("length: %d \r\n", pkt->length);
		printf("checksum: %u \r\n", pkt->checksum);
		
}		


void PrintPacketLog(Packet *pkt)
{
// id, src, dest, type, data, length ,checksum
	printf(" %d, %c, %d, %d, %d, %d, %d, %d, %d, %f, %f\r\n", pkt->id, pkt->type, pkt->checksum, pkt->dest, pkt->src, pkt->length, pkt->data_int[0],pkt->data_int[1],pkt->data_int[2], pkt->data_double[0], pkt->data_double[1]);
}


void PrintRangingLog(uint8_t src, uint8_t dest, RangingPIB * ranginginfo)
{
//src, dest, distance, error
	printf(" %d, %d, %f, %c \r\n", src, dest, ranginginfo->distance, ranginginfo->error);
}

void PacketGeneration(Packet *pkt, uint8_t id, uint8_t type, uint8_t checksum, uint8_t dest, uint8_t src)
{
    pkt->id = id;
    pkt->type = type;
    pkt->checksum = checksum;
    pkt->dest = dest;
    pkt->src = src;
    pkt->length = sizeof(Packet);
    pkt->data_int[0] = '\0';
    pkt->data_double[0] = '\0';
}
