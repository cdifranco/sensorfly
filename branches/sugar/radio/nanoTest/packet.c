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
		for(i = 0; i < 10; i++)
		{
			printf("data: %d,",  pkt->data[i]);
		}
		printf("\r\n");
		printf("length: %d \r\n", pkt->length);
		printf("checksum: %u \r\n", pkt->checksum);
		
}		


void PrintPacketLog(Packet *pkt)
{
// id, src, dest, type, data, checksum
    printf(" %d, %d, %d, %c, %s, %d \r\n", pkt->id, pkt->src, pkt->dest, pkt->type, pkt->data, pkt->checksum);
}

void PrintRangingLog(uint8_t src, uint8_t dest, RangingPIB * ranginginfo)
{
//src, dest, distance, error
		printf(" %d, %d, %f, %c \r\n", src, dest, ranginginfo->distance, ranginginfo->error);
}
