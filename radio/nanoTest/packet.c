/**
 * Methods implements about packet
 */ 


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "packet.h"


void PrintPacket(Packet *pkt)
{
		int i;
		printf("******Packet %d ******\r\n", pkt->id);
		printf("destination: ");
		for(i = 0; i < 6; i++)
		{
				if (i == 5) printf(" %d \r\n",pkt->dest);
				else printf(" %d :",0);
		}
		printf("source: ");
		for(i = 0; i < 6; i++)
		{
				if (i == 5) printf(" %d \r\n",pkt->src);
				else printf(" %d :",0);
		}
		printf("type: %c \r\n", pkt->type);
		printf("data: %s \r\n", pkt->data);
		printf("length: %d \r\n", pkt->length);
		printf("checksum: %d \r\n", pkt->checksum);
		
}

void PrintRangingPacket(Packet *pkt)
{
		int i;
		printf("******Packet %d ******\n", pkt->id);
		printf("destination: ");
		for(i = 0; i < 6; i++)
		{
				if (i == 5) printf(" %d \n",pkt->dest);
				else printf(" %d :",0);
		}
		printf("source: ");
		for(i = 0; i < 6; i++)
		{
				if (i == 5) printf(" %d \n",pkt->src);
				else printf(" %d :",0);
		}
		printf("type: %c \n", pkt->type);
		double distance;
		int error;
		memcpy(&distance,pkt->data,4);
		memcpy(&error,&(pkt->data[4]),1);
		printf("distance : %f \n", distance);
		printf("error: %c \n", error);
		printf("length: %d \n", pkt->length);
		printf("checksum: %d \n", pkt->checksum);
		
}		


void PrintPacketLog(Packet *pkt)
{
// id, src, dest, type, data, checksum
    printf(" %d, %d, %d, %c, %s, %d \r\n", pkt->id, pkt->src, pkt->dest, pkt->type, pkt->data, pkt->checksum);
}

void PrintRangingLog(uint8_t dest, RangingPIB * ranginginfo)
{
// dest, distance, error
		printf(" %d, %f, %c \r\n", dest, ranginginfo->distance, ranginginfo->error);
}
