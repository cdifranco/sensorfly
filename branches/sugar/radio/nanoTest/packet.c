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
		printf("******Packet %d ******\n", pkt->id);
		printf("destination: ", pkt->dest);
		for(i = 0; i < 6; i++)
		{
				if (i == 5) printf(" %d \n",pkt->dest);
				else printf(" %d :",0);
		}
		printf("source: ", pkt->src);
		for(i = 0; i < 6; i++)
		{
				if (i == 5) printf(" %d \n",pkt->src);
				else printf(" %d :",0);
		}
		printf("type: %c \n", pkt->type);
		printf("length: %d \n", pkt->length);
		printf("checksum: %d \n", pkt->checksum);
		
}	




