/**
 * Methods implements about packet
 */ 


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "packet.h"


Packet *CreateTestingPacket(void)
{
		Packet *pkt = (Packet *)malloc(sizeof(Packet));
		
		// 
		PByte8T temp[2]	= {0,1};	
		pkt->id = 1;
		memcpy(pkt->dest, temp, 2);
		temp[1]= 2;		
		memcpy(pkt->src, temp, 2);
		pkt->type = 'd';

		//
		PByte8T temp2[6] = "aaa~";
		memcpy(pkt->data, temp2, 6);
		pkt->checksum = 0;		
		return pkt;
}

void PrintPacket(Packet *pkt)
{
		int i;
		printf("******Packet %d ******\n", pkt->id);
		printf("destination: ", pkt->dest);
		for(i = 0; i < 6; i++)
		{
				if (i == 5) printf(" %x \n",pkt->dest[1]);
				else if(i == 4) printf(" %x :",pkt->dest[0]);		
				else printf(" %x :",0);
		}
		printf("source: ", pkt->src);
		for(i = 0; i < 6; i++)
		{
				if (i == 5) printf(" %x \n",pkt->src[1]);
				else if(i == 4) printf(" %x :",pkt->src[0]);		
				else printf(" %x :",0);		
		}
		printf("type: %c \n", pkt->type);
		printf("data: %s \n", pkt->data);
		printf("checksum: %d \n", pkt->checksum);
		
}	




