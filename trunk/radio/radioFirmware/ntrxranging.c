/* $Id$ */
/**
 * @file ntrxranging.c
 * @date 2007-Dez-11
 * @author C. Bock
 * @c (C) 2007 Nanotron Technologies
 * @brief Ranging support functions.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the source code for the implementation of the
 *    NTRX ranging functions
 */

/*
 * $Log$
 */

#include	"ntrxtypes.h"
#include	"registermap.h"
#include	"ntrxutil.h"
#include	"ntrxranging.h"
#include	"nnspi.h"
#include	"phy.h"
#include	<string.h>

#define	PHASEOFFSETACK		0
#define	TOAOFFSETMEANACK_L	1
#define	TOAOFFSETMEANACK_H	2
#define	TXRESPTIME_L		3
#define	TXRESPTIME_H		4
#define	PHASEOFFSETDATA		5
#define	TOAOFFSETMEANDATA_L	6
#define	TOAOFFSETMEANDATA_H	7
#define	PREVRXID		8
#define	PREVTXID		9
#define ACTTXID			10
#define RSSI			11

/* length of ranging packet */
#define ARRYLEN  12

/* speed of our signal in AIR or CABLE [um/ps] */
#define SPEED_OF_AIR 299.792458
#define SPEED_OF_CABLE 299.792458*0.66

/* Measurement difference between remote and local station isnt allowed to
 * be bigger than MAX_DIFF_ALLOWED
 *
 * If its bigger than N [m], it will droped */
#define MAX_DIFF_ALLOWED 6

static MyMsgT	rangeMsg;
static MyDouble32T dist(MyByte8T *p1, MyByte8T *p2);
static MyDouble32T delay(MyByte8T *p);

static MyByte8T lrv[ARRYLEN]; /* Local ranging values  */
static MyByte8T rrv[ARRYLEN]; /* Remote ranging values */
static MyByte8T set1[ARRYLEN], set2[ARRYLEN]; /* this holds the collected data for calc */

MyByte8T rangingLastCmd = RANGING_START;

extern PhyModeSetT modeSet;

/* ranging informations */
RangingPIB rangingPIB;

/**
 * @brief collects ranging data after receiving a ranging packet
 * @par payload data of received packet
 * @par val length of payload
 *
 * the functions is automaticly called in NTRXReceive
 *
 */
void RangingCallback_Rx(MyByte8T *payload, MyByte8T len)
{
	MyByte8T	ToaOffsetMeanDataValid;

	lrv[ACTTXID]=0x00;

	/* Read Rx Ranging Registers */
	NTRXSPIReadByte (NA_ToaOffsetMeanDataValid_O, &ToaOffsetMeanDataValid);
	ToaOffsetMeanDataValid &= (0x01 << NA_ToaOffsetMeanDataValid_B);
	ToaOffsetMeanDataValid = ToaOffsetMeanDataValid>>NA_ToaOffsetMeanDataValid_B;


	if(ToaOffsetMeanDataValid != 1)
	{
		if(rrv[ACTTXID] == 1)
		{
			rangingPIB.error |= STAT_PACKET_ERROR_RX1;
		}
		else
		{
			rangingPIB.error |= STAT_PACKET_ERROR_RX2;
		}
	}
	else
	{

		if (len==ARRYLEN)
		{
			/* Get the remote ranging values */
			memcpy(rrv,payload,ARRYLEN);

			NTRXSPIReadByte (NA_AgcGain_O, &lrv[RSSI]);
			lrv[RSSI] &= (0x3F << NA_AgcGain_LSB);
			lrv[RSSI] = lrv[RSSI] >> NA_AgcGain_LSB;
			lrv[RSSI] = 0x3F - lrv[RSSI];

			/* Check what matches our local values */
			switch(rrv[ACTTXID])
			{
				case 1 :
					/* combine data for locally initiated round trip */
					set1[PHASEOFFSETACK]=lrv[PHASEOFFSETACK];
					set1[TOAOFFSETMEANACK_H]=lrv[TOAOFFSETMEANACK_H];
					set1[TOAOFFSETMEANACK_L]=lrv[TOAOFFSETMEANACK_L];
					set1[TXRESPTIME_H]=lrv[TXRESPTIME_H];
					set1[TXRESPTIME_L]=lrv[TXRESPTIME_L];

					set1[PHASEOFFSETDATA]=rrv[PHASEOFFSETDATA];
					set1[TOAOFFSETMEANDATA_H]=rrv[TOAOFFSETMEANDATA_H];
					set1[TOAOFFSETMEANDATA_L]=rrv[TOAOFFSETMEANDATA_L];

					set1[RSSI]=rrv[RSSI];
					set2[RSSI]=lrv[RSSI];

					break;
				case 2 :
					/* combine data for remotely initiated round trip */
					set2[PHASEOFFSETACK]=rrv[PHASEOFFSETACK];
					set2[TOAOFFSETMEANACK_H]=rrv[TOAOFFSETMEANACK_H];
					set2[TOAOFFSETMEANACK_L]=rrv[TOAOFFSETMEANACK_L];
					set2[TXRESPTIME_H]=rrv[TXRESPTIME_H];
					set2[TXRESPTIME_L]=rrv[TXRESPTIME_L];

					set2[PHASEOFFSETDATA]=lrv[PHASEOFFSETDATA];
					set2[TOAOFFSETMEANDATA_H]=lrv[TOAOFFSETMEANDATA_H];
					set2[TOAOFFSETMEANDATA_L]=lrv[TOAOFFSETMEANDATA_L];

					return; /* dont need to read out the local values again */
				default:
					break;
			}

			/* now we can update our local ranging values */
			lrv[PREVRXID]=rrv[ACTTXID];

			NTRXSPIReadByte (NA_PhaseOffsetData_O, &lrv[PHASEOFFSETDATA]);
			lrv[PHASEOFFSETDATA] &= (0x07 << NA_PhaseOffsetData_LSB);
			lrv[PHASEOFFSETDATA] = (lrv[PHASEOFFSETDATA] >> NA_PhaseOffsetData_LSB);

			NTRXSPIRead (NA_ToaOffsetMeanData_O, &lrv[TOAOFFSETMEANDATA_L], 2);
			lrv[TOAOFFSETMEANDATA_H] &= 0x1f;
		}
	}
}

/**
 * @brief collects rangingdata after sending an rangingpacket
 * @par arqCount amount of retransmissions for the last transmitted packet
 *
 * the functions is automaticly called in NTRXTxEnd
 *
 */
void RangingCallback_Ack(MyByte8T arqCount)
{
	MyByte8T	ToaOffsetMeanAckValid;
	/* Read Tx Ranging Registers */
    NTRXSPIReadByte (NA_ToaOffsetMeanAckValid_O, &ToaOffsetMeanAckValid);
	ToaOffsetMeanAckValid &= (0x01 << NA_ToaOffsetMeanAckValid_B);
	ToaOffsetMeanAckValid = ToaOffsetMeanAckValid>>NA_ToaOffsetMeanAckValid_B;

	if(ToaOffsetMeanAckValid != 1)
	{
		rangingPIB.error |= STAT_PACKET_ERROR_TX;
	}
	else
	{
		NTRXSPIReadByte (NA_PhaseOffsetAck_O, &lrv[PHASEOFFSETACK]);
		lrv[PHASEOFFSETACK] &= (0x07 << NA_PhaseOffsetAck_LSB);
		lrv[PHASEOFFSETACK] = lrv[PHASEOFFSETACK]>>NA_PhaseOffsetAck_LSB;

		NTRXSPIRead (NA_ToaOffsetMeanAck_O, &lrv[TOAOFFSETMEANACK_L], 2);
		lrv[TOAOFFSETMEANACK_H] &= 0x1f;

        NTRXSPIRead ( NA_TxRespTime_O, &lrv[TXRESPTIME_L], 2);
	}

	lrv[PREVTXID]=lrv[ACTTXID];

	if ( 	(rangeMsg.prim == PD_RANGING_ANSWER2_EXECUTE) ||
			(rangeMsg.prim == PD_RANGING_FAST_ANSWER1_EXECUTE)	)
	{
		memset(lrv,'\0',ARRYLEN);
		memset(rrv,'\0',ARRYLEN);
		memset(set1,'\0',ARRYLEN);
		memset(set2,'\0',ARRYLEN);
	}
	
}


/**
 * @brief start ranging
 * @par dest destination address
 *
 * RangingAnswer handles the start and continue of ranging at all.
 *
 */
void RangingMode(MyByte8T cmd, MyAddrT dest)
{
	lrv[ACTTXID]++;

	memcpy (rangeMsg.addr, dest, 6);
	memcpy (rangeMsg.data, lrv, ARRYLEN);

	switch(cmd)
	{
		case RANGING_START :
			memset(lrv,'\0',ARRYLEN);
			memset(rrv,'\0',ARRYLEN);
			memset(set1,'\0',ARRYLEN);
			memset(set2,'\0',ARRYLEN);
			rangeMsg.prim = PD_RANGING_REQUEST_EXECUTE;
			rangingLastCmd = RANGING_START;
			break;
		case RANGING_ANSWER1 : rangeMsg.prim = PD_RANGING_ANSWER1_EXECUTE; break;
		case RANGING_ANSWER2 : rangeMsg.prim = PD_RANGING_ANSWER2_EXECUTE; break;
		case RANGING_FAST_START : 
			memset(lrv,'\0',ARRYLEN);
			memset(rrv,'\0',ARRYLEN);
			memset(set1,'\0',ARRYLEN);
			memset(set2,'\0',ARRYLEN);
			rangeMsg.prim = PD_RANGING_FAST_REQUEST_EXECUTE;
			rangingLastCmd = RANGING_FAST_START;
			break;
		case RANGING_FAST_ANSWER1 : rangeMsg.prim = PD_RANGING_FAST_ANSWER1_EXECUTE; break;
	}

	rangeMsg.len = ARRYLEN;
	PDSap (&rangeMsg);
}


/**
 * @brief userinterface for distance calculation
 * @return distance in [m] or [-1] in case of error
 *
 * getDistance calculates indirecty the distance.
 *
 */
MyDouble32T getDistance(void)
{
	MyDouble32T distance;

	distance = dist(&set1[0],&set2[0]);

	/* measurements between 0.00[m] and -0.99[m] will changed to 0.0[m]
	 * measurements below -0.99[m] will changed to -1.0[m] (ERROR) */
	return distance;
}

/**
 * @brief userinterface for rssi
 * @return rssi value
 *
 * getRSSI returns the measured RSSI value
 *
 */
MyByte8T getRSSI(void)
{
	return ( set1[RSSI] + set2[RSSI] ) / 2;
}

/**
 * @brief distance calculation
 * @par p1 is one set of data which is necessary for calculation distance1
 * @par p2 is one set of data which is necessary for calculation distance2
 * @return distance in [m]
 *
 * dist calculates the distance.
 *
 */
MyDouble32T dist(MyByte8T *p1, MyByte8T *p2)
{
	MyDouble32T avg = -1.0;
	MyDouble32T speedofmedium = SPEED_OF_AIR;

	/* calculate the one way airtime for local station */
	MyDouble32T distanceD2R = delay(p1);
	MyDouble32T distanceR2D = 0.0;

	/* calculate the distance in [m] for local station */
	distanceD2R *=(speedofmedium);

	/* second set is empty if its FAST ranging */
	if (rangingLastCmd == RANGING_START)
	{
		/* calculate the one way airtime for remote station */
		distanceR2D = delay(p2);

		/* calculate the distance in [m] for remote station */
		distanceR2D *=(speedofmedium);


		/* the difference between the measurement results from local-
		* and remote station should not be to large */
		if(distanceR2D>distanceD2R)
		{
			if((distanceR2D-distanceD2R)>MAX_DIFF_ALLOWED) rangingPIB.error |= STAT_RANGING_VALUE_ERROR;
		}else{
			if((distanceD2R-distanceR2D)>MAX_DIFF_ALLOWED) rangingPIB.error |= STAT_RANGING_VALUE_ERROR;
		}

		avg  = ((distanceR2D + distanceD2R )/2.0); /* [m] */
	}
	else if (rangingLastCmd == RANGING_FAST_START)
	{
		avg = distanceD2R;
	}
	else
	{
		/* should not occure */
		avg = 0.0;
	}

	return avg;
}


/**
 * @brief delay calculation
 * @par p is one set of data which is necessary for calculation
 * @return delay
 *
 * Delay calculates the airtime of packet WITHOUT processing time in chip.
 * Its a helper function for dist(..,..);
 *
 */
MyDouble32T delay(MyByte8T *p)
{
	/* clock period [MHz] */
	const MyDouble32T clk_4MHz  = 4;
	const MyDouble32T clk_32MHz = 32;
	/* Scaled 1:20 divider's clock period [MHz] */
	const MyDouble32T clk_lod20 = (2000.0/244175);

	const MyWord16T PulseDetUcMax = 5;
	const MyWord16T PulseDetUcMax_table[16] = {1, 2, 4, 8, 16, 24, 32, 40, 48, 56, 64, 1, 1, 1, 1, 1};

	MyDouble32T res;

	MyDword32T TxRespTime = (p[TXRESPTIME_H]        << 8) | (p[TXRESPTIME_L]);
	MyLong32T  RxUcSum    = (p[TOAOFFSETMEANACK_H]  << 8) | (p[TOAOFFSETMEANACK_L]);
	MyLong32T  TxUcSum    = (p[TOAOFFSETMEANDATA_H] << 8) | (p[TOAOFFSETMEANDATA_L]);

	MyLong32T  RxGateOff  = p[PHASEOFFSETACK]  == 7 ? 7 : 6 - p[PHASEOFFSETACK];
	MyLong32T  TxGateOff  = p[PHASEOFFSETDATA] == 7 ? 7 : 6 - p[PHASEOFFSETDATA];

	MyDouble32T rangingConst = (modeSet.fec) ? modeSet.rangingConst_FECon : modeSet.rangingConst_FECoff;

	if (modeSet.fdma == 0)
	{
		res = ((TxRespTime)/clk_4MHz
        	 	- ((TxGateOff+RxGateOff))/clk_32MHz
			- (TxUcSum+RxUcSum)*clk_lod20/(2.0*PulseDetUcMax_table[PulseDetUcMax]))/2.0
			- rangingConst;
	}else{
		res = (TxRespTime/clk_4MHz - (TxGateOff+RxGateOff)/clk_32MHz
			-(TxUcSum+RxUcSum)/clk_32MHz*(2.0*PulseDetUcMax_table[PulseDetUcMax]))/2.0
			- rangingConst;
	}

	return res;
}


