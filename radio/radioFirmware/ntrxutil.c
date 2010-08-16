/* $Id$ */

/**
 * @file ntrxutil.c
 * @date 2007-Dez-4
 * @author S.Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief Functions for the physical layer helper functions.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the source code for the implementation of the
 *    NTRX helper functions
 *
 */

/*
 * $Log$
 */

#include    <stdlib.h>
#include    <string.h>
#include    "config.h"
#include    "ntrxtypes.h"
#include    "ntrxutil.h"
#include    "hwclock.h"
#include    "nnspi.h"
#include    "ntrxiqpar.h"
#include 	"phy.h"
#include 	<avr/interrupt.h>
#ifdef	CONFIG_CONSOLE
#	include 	"usart.h"
#endif

extern MyBoolT NTRXRestart (void);
extern PhyPIB phyPIB;

MyByte8T loTarVal[5][2] = {{0x59, 0x68},			/* current value */
							{0x59, 0x68}, 			/* Channel 0: 2.44175 GHz, center of the band,
																		shall be used for 80 MHz mode */
							{0x66, 0x62}, 			/* Channel 1: 2.412 GHz, Center frequency of
																		nonoverlapping channel no. 1 (Europe or USA) */
							{0x8c, 0x62}, 			/* Channel 15: 2.41275 GHz, Center frequency of
																		nonoverlapping channel no. 1 (Europe or USA) */
							{0x26, 0x6e}}; 			/* Channel 16: 2.47076 GHz, Center frequency of
																		nonoverlapping channel no. 1 (Europe or USA) */

MyByte8T	ntrxShadowReg[SHADOWREGCOUNT];	/**< shadow register for the nanoLOC chip. Needed for write-only register */
PhyModeSetT modeSet;						/**< group of parameters that define a transmission mode. */
MyByte8T 	ntrxIrqStatus[2];				/**< interrupt status register */
MyByte8T   ntrxState;						/**< state of the nanoLOC chip */
//MyByte8T   ntrxCal;						/**< flag for automatic recalibration */
MyByte8T 	lfdma = 0, lsd = 0, lsr = 0;	/**< ranging variables */

MyByte8T 	ntrxInitShadowReg[] PROGMEM =
{
/* 0x00 */   0,   0,   0,   0,   0,   0,   0,   6,
/* 0x08 */   0,   0,   0,   0,   0,   0,   0,   0,
/* 0x10 */   0,   0,   0,   0,   0,   0,  64,   0,
/* 0x18 */  32,  64,   0,  32,   0,   0,   0,   3,
/* 0x20 */   6, 152,  12,   0,   2,  63,  30,   6,
/* 0x28 */   0,   0, 171,  44, 213, 146, 148, 202,
/* 0x30 */ 105, 171,  48,   0,   0,   0,   0,   0,
/* 0x38 */   0,   0,   0,   0, 224,   4,   0,   1,
/* 0x40 */   3,   7,   0,   3,  63,  63,  15,  15,
/* 0x48 */ 115,   0,  16,  16,  67,  20,  16,   0,
/* 0x50 */   0, 127,   0,   0,   0,   0,   0,   0,
/* 0x58 */   0,   0,  11,  95,   5,   7, 213,  98,
/* 0x60 */   0,   0,   0,  12,  10,   0,   0,   0,
/* 0x68 */   0,   0,   0,   0,   0,   0,   0,   0,
/* 0x70 */   0,   0,   0,   0,   0,   0,   0,   0,
/* 0x78 */   0,   0,   0,   0,   0,  80,   0,   0
};

/**
 * @brief resets the transmission mode parameters.
 *
 * This function sets the three parameters that define a transmission mode.
 *
 */
/**************************************************************************/
void NTRXResetSettings (void)
/**************************************************************************/
{
	lfdma 	= 0;
	lsd 	= 0;
	lsr 	= 0;
}

/**
 * @brief initialize shadow registers.
 *
 * This function initializes the shadow register array to the initial
 * values in the transceiver when it is powered up or reset.
 *
 */
/**************************************************************************/
void NTRXInitShadowRegister (void)
/**************************************************************************/
{
    MyByte8T i;

    /*
     * set shadow registers to the initial value of the nanoNET TRX chip
     */
    for (i = 0; i < SHADOWREGCOUNT; i++)
    {
        ntrxShadowReg[i] = ReadRomByte(ntrxInitShadowReg[i]);
    }
}

/**
 * @brief set the logical channel.
 * @param value the logical channel id
 *
 * This function sets the logical channel for transmission and reception
 *
 */
/**************************************************************************/
void NTRXSetChannel (MyByte8T value)
/**************************************************************************/
{
	/*
	 * first check for a valid parameter. If the parameter is out of range
	 * do nothing.
	 */
	if (value > NTRX_MAX_CHANNEL_ID)
		return;

	/*
	 * in the first slot of the loTarVal array the currently used channel
	 * (center frequency) is stored. The other 4 are used to hold the
	 * predefined channels. When a new channel is selected, the value
	 * is copied form the list to the first slot. For fdma only the first
	 * channel is stored. The other 14 channels are calculated with the
	 * first channel as the basis.
	 */
	switch (value)
	{
		case 0:	loTarVal[0][0] = loTarVal[1][0];
				loTarVal[0][1] = loTarVal[1][1];
				break;

		case 15:loTarVal[0][0] = loTarVal[3][0];
				loTarVal[0][1] = loTarVal[3][1];
				break;

		case 16:loTarVal[0][0] = loTarVal[4][0];
				loTarVal[0][1] = loTarVal[4][1];
				break;

		default:loTarVal[0][0] = loTarVal[2][0];
				loTarVal[0][1] = loTarVal[2][1] + value - 1;
				break;

	}
}

/**
 * @brief set the index register.
 * @param page address index
 *
 * This function sets the index registers for the transceiver chip.
 * The chip address space is deviced into 4 sections a 1kbyte.
 *
 */
/**************************************************************************/
void NTRXSetIndexReg (MyByte8T page)
/**************************************************************************/
{
    if (page != ntrxShadowReg[NA_RamIndex_O])
    {
        ntrxShadowReg[NA_RamIndex_O] = page;
        NTRXSPIWriteByte (NA_RamIndex_O, page);
    }
}

/**
 * @brief set the index register.
 * @param page address index
 *
 * This function sets the index registers for the transceiver chip
 * The chip address space is deviced into 4 sections a 1kbyte.
 *
 */
/**************************************************************************/
void NTRXSetRamIndex (MyByte8T page)
/**************************************************************************/
{
    page &= 0x03;
    if (page != (ntrxShadowReg[NA_RamIndex_O] & 0x03))
    {
        ntrxShadowReg[NA_RamIndex_O] &= 0xf0;
        ntrxShadowReg[NA_RamIndex_O] |= page;
        page = ntrxShadowReg[NA_RamIndex_O];
        NTRXSPIWriteByte (NA_RamIndex_O, page);
    }
}

/**
 * @brief calibrates the local ocsillator.
 *
 * This function callibrates the local oscillator of the receiver part.
 * It is not called directly but from within the NTRXAllCalibration
 * function.
 *
 */
/**************************************************************************/
void NTRXRxLoCalibration (void)
/**************************************************************************/
{
    MyByte8T data[3];

	NTRXSPIWriteByte (NA_EnableLO_O, (ntrxShadowReg[NA_EnableLO_O] | (1 << NA_EnableLO_B) | (1 << NA_EnableLOdiv10_B)));
	HWDelayms( 4 ); /* ms */
	NTRXSPIWriteByte (NA_UseLoRxCaps_O, (ntrxShadowReg[NA_UseLoRxCaps_O] | (1 << NA_UseLoRxCaps_B)));

    data[0] = 0x03;
	NTRXSPIWriteByte (NA_LoIntsReset_O, data[0]);
    NTRXSPIWrite (NA_LoTargetValue_O, loTarVal[0], 2);

    do
    {
        NTRXSPIReadByte (NA_LoIntsRawStat_O, &data[0]);
    } while ((data[0] & (1 << NA_LoTuningReady_B)) != (1 << NA_LoTuningReady_B));

	/* Read out caps values if needed */
    //NTRXSPIRead (0x16, data, 3);
	NTRXSPIWriteByte (NA_UseLoRxCaps_O, ntrxShadowReg[NA_UseLoRxCaps_O]);
	NTRXSPIWriteByte (NA_EnableLO_O, ntrxShadowReg[NA_EnableLO_O]);
}

/**
 * @brief calibrates the local ocsillator.
 *
 * This function callibrates the local oscillator of the transmitter part.
 * It is not called directly but from within the NTRXAllCalibration
 * function.
 *
 */
/**************************************************************************/
void NTRXTxLoCalibration (void)
/**************************************************************************/
{
    MyByte8T data[3];

	NTRXSPIWriteByte (NA_EnableLO_O, (ntrxShadowReg[NA_EnableLO_O] | (1 << NA_EnableLO_B) | (1 << NA_EnableLOdiv10_B)));
	HWDelayms( 4 ); /* ms */
    data[0] = 0x03;
    NTRXSPIWriteByte (NA_LoIntsReset_O, data[0]);

    NTRXSPIWrite (NA_LoTargetValue_O, loTarVal[0], 2);

    do
    {
        NTRXSPIReadByte (NA_LoIntsRawStat_O, &(data[0]));
    } while ((data[0] & (1 << NA_LoTuningReady_B)) != (1 << NA_LoTuningReady_B));

	/* Read out caps values if needed */
    //NTRXSPIRead (0x16, data, 3);
	NTRXSPIWriteByte (NA_EnableLO_O, ntrxShadowReg[NA_EnableLO_O]);
}

/**
 * @brief calibrates the local ocsillator.
 *
 * This function callibrates the local oscillator for both the transmitter
 * and the receiver part. This function should be called regularly to maintain
 * a stable and in spec transmission.
 *
 */
/**************************************************************************/
MyBoolT NTRXAllCalibration (void)
/**************************************************************************/
{
    MyByte8T value;

	/*
	 * make sure the we are not currently transmitting. This would destroy
	 * the partialy send message.
	 */
	if (ntrxState == TxSEND) return FALSE;

  	/*
	 * enable fast tuning
	 */
	value = (1 << NA_LoEnableFastTuning_B) | (1 << NA_LoEnableLsbNeg_B) | (4 << NA_LoFastTuningLevel_LSB);
    NTRXSPIWriteByte (NA_LoEnableFastTuning_O, value);
    ntrxShadowReg[NA_LoEnableFastTuning_O] = value;

	/*
	 * stop the receiver and clear the buffers
	 */
	NTRXSPIWriteByte (NA_RxCmdStop_O, ntrxShadowReg[NA_RxCmdStop_O] | (1 << NA_RxCmdStop_B) | (0x03 << NA_RxBufferCmd_LSB));
	NTRXSPIWriteByte (NA_RxIntsReset_O, 0x7F);

    rxIrqStatus = 0;

	/*
	 * calibrate transmitter and receiver
	 */
    NTRXRxLoCalibration ();
    NTRXTxLoCalibration ();

	/*
	 * start receiver if it was started before calibration
	 */
	if (phyPIB.rxState == PHY_RX_ON)
	{
		NTRXSPIWriteByte (NA_RxCmdStart_O, ntrxShadowReg[NA_RxCmdStart_O] | (1 << NA_RxCmdStart_B));
	}
//    ntrxCal = 0;

	return TRUE;
}

/**
 * @brief read out version an revision of the transceiver chip.
 * @returns TRUE if equal, False if not equal or SPI access failed.
 *
 * This function compares the version and revision register with
 * the expected Version and Revision number from the software.
 *
 */
/**************************************************************************/
MyBoolT NTRXCheckVerRev (void)
/**************************************************************************/
{
    MyByte8T buff[2];

    NTRXSPIRead (NA_Version_O, buff, 1);
    NTRXSPIRead (NA_Revision_O, &(buff[1]), 1);

#   if ((CONFIG_PRINTF) && (CONFIG_LOGO))
    printf("ver:%d, rev:%d\n", buff[0], buff[1]);
#   endif

    if (buff[0] == NA_Version_I && buff[1] == NA_Revision_I)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
 * @brief read out number of retransmission attempts.
 * @returns tx arq maximum value in the range of (0x0...0xE).
 *
 * This function returns the number of packet retransmissions used by
 * the nanoLOC chip.
 * The value is taken from the shadow ram.
 *
 */
/**************************************************************************/
MyByte8T NTRXGetTxArqMax (void)
/**************************************************************************/
{
	return ((ntrxShadowReg[NA_TxArqMax_O] >> 4) & 0x0f);
}

/**
 * @brief returns the setting vor the CRC-2 mode of the transceiver chip.
 * @returns On (TRUE) or Off (FALSE).
 *
 * This function returns the CRC-2 mode settings in the transceiver chip.
 * The value is taken from the shadow ram.
 */
/**************************************************************************/
MyBoolT NTRXGetRxCrc2Mode (void)
/**************************************************************************/
{
    return ((MyBoolT)((ntrxShadowReg[NA_RxCrc2Mode_O] & (1<< NA_RxCrc2Mode_B)) >> NA_RxCrc2Mode_B));
}



/**
 * @brief sets the capacitors to tune the frequency.
 *
 * This function executes the recommended procedure to tune the frequency
 * by en- and disableing capacitors.
 */
/**************************************************************************/
void NTRXFctCal (void)
/**************************************************************************/
{
    int capsVal = 7;
    int targetValMax = 174;
    int targetValMin = 152;
    int fctSum = 0;
    int ctref;
    unsigned char payload;
    // unsigned long t;

    payload = ( 1 << NA_FctClockEn_B ) | capsVal;
    NTRXSPIWriteByte (NA_ChirpFilterCaps_O, payload);
	HWDelayus( 10 ); /* 10 us */

    while ((capsVal < 16) && (capsVal >= 0))
    {
        fctSum = 0;
        for (ctref = 0; ctref < 4; ctref++)
        {
    		payload = ( 1 << NA_FctClockEn_B ) | ( 1 << NA_StartFctMeasure_B ) | capsVal;
		    NTRXSPIWriteByte (NA_ChirpFilterCaps_O, payload);
			HWDelayus( 15 ); /* 10 us */

            NTRXSPIReadByte (NA_FctPeriod_O, &payload);
            fctSum += payload;
        }
        if (fctSum >= targetValMax)
        {
            capsVal--;
        }
        else if (fctSum <= targetValMin)
        {
            capsVal++;
        }
        else
        {
            payload = capsVal;
		    NTRXSPIWriteByte (NA_ChirpFilterCaps_O, payload);
			return;
        }
    }
    payload = 6;
    NTRXSPIWriteByte (NA_ChirpFilterCaps_O, payload);
}

/**
 * @brief sets the sync word in the transceiver.
 * @param value a pointer to the 8 byte syncword.
 *
 * This function writes the syncword in the syncword register.
 * The syncword is used for add a transceivers to a logical
 * group. Only messages with the right syncword are detected
 * (received) and processed.
 */
/**************************************************************************/
void NTRXSetSyncWord (MyByte8T *value)
/**************************************************************************/
{
	NTRXSPIWrite (NA_SyncWord_O, value, 8);
}

/**
 * @brief sets transmission mode in the transceiver.
 * @param fdma this parameter determines the the mode (fdma or 80 MHz)
 * @param sd this parameter determines the symbol duration between 500ns and 8us)
 * @param sr this parameter determines the symbol rate between 125kbit and 2Mbit)
 *
 * Based on the three parameters the transceiver is initialized.
 * The iq parameter are set and the nanoLOC chip is calibrated.
 *
 */
/**************************************************************************/
void NTRXSetupTrxMode (MyByte8T fdma, MyByte8T sd, MyByte8T sr)
/**************************************************************************/
{
	static MyByte8T firstTime = 0;

	if ((lfdma == fdma) && (lsd == sd) && (lsr == sr))
	{
	    NTRXAllCalibration ();
		return;
	}

	modeSet.bw = fdma;
	modeSet.sd = sd;
	modeSet.sr = sr;
	if (fdma == NA_22MHz)
	{
    	modeSet.fdma= TRUE;
    	modeSet.fixnmap=NA_FIX_MODE;
	}
	else
	{
    	modeSet.fdma= FALSE;
    	modeSet.fixnmap=NA_FIX_MODE;
	}

	if (firstTime++ != 0)
	{
		if (ntrxShadowReg[NA_RamIndex_O] != 0)
		{
			NTRXSPIWriteByte (NA_RamIndex_O, 0);
		}
		NTRXSPIWriteByte (NA_RxCmdStop_O, (1 << NA_RxCmdStop_B) | (0x03 << NA_RxBufferCmd_LSB));
		NTRXSPIWriteByte (NA_RxIntsReset_O, (0x7f << NA_RxIntsReset_LSB));
		NTRXSPIWriteByte (NA_ResetBbRadioCtrl_O, (1 << NA_ResetBbRadioCtrl_B));
		ntrxShadowReg[NA_ResetBbRadioCtrl_O] = 0;
		ntrxShadowReg[NA_EnableBbClock_O] &= ~(1 << NA_EnableBbClock_B);
		NTRXSPIWrite (NA_ResetBbClockGate_O, ntrxShadowReg + NA_ResetBbClockGate_O, 2);
		ntrxShadowReg[NA_ResetBbClockGate_O] |= (1 << NA_ResetBbClockGate_B);
		ntrxShadowReg[NA_EnableBbClock_O] &= ~(1 << NA_EnableBbCrystal_B);
		NTRXSPIWrite (NA_ResetBbClockGate_O, ntrxShadowReg + NA_ResetBbClockGate_O, 2);

	}else 	phyPIB.rxState = PHY_RX_ON;


    /* first part of initial register setting */
	ntrxShadowReg[NA_ResetBbClockGate_O] |= (1 << NA_ResetBbClockGate_B);
	ntrxShadowReg[NA_EnableBbCrystal_O] |= (1 << NA_EnableBbCrystal_B);
	NTRXSPIWrite (NA_ResetBbClockGate_O, ntrxShadowReg + NA_ResetBbClockGate_O, 2);
	HWDelayms( 5 ); /* 5 ms */
	ntrxShadowReg[NA_ResetBbClockGate_O] &= ~(1 << NA_ResetBbClockGate_B);
	ntrxShadowReg[NA_EnableBbClock_O] |= (1 << NA_EnableBbClock_B);
	NTRXSPIWrite (NA_ResetBbClockGate_O, ntrxShadowReg + NA_ResetBbClockGate_O, 2);
	NTRXSPIWriteByte (NA_ResetBbRadioCtrl_O, (1 << NA_ResetBbRadioCtrl_B));
	NTRXSPIWriteByte (NA_ResetBbClockGate_O, 0);

	ntrxShadowReg[NA_HoldAgcInBitSync_O] &= ~(0x7f << NA_HoldAgcInBitSync_LSB);
	ntrxShadowReg[NA_HoldAgcInFrameSync_O] |= ((1 << NA_HoldAgcInFrameSync_B)
							| (24 << NA_HoldAgcInBitSync_LSB));
	NTRXSPIWriteByte (NA_HoldAgcInFrameSync_O, ntrxShadowReg[NA_HoldAgcInFrameSync_O]);

	ntrxShadowReg[NA_ChirpFilterCaps_O] &= ~(0x0f << NA_ChirpFilterCaps_LSB);
	NTRXSPIWriteByte (NA_ChirpFilterCaps_O, ntrxShadowReg[NA_ChirpFilterCaps_O]);

    /* Tx Part */
	ntrxShadowReg[NA_TxArqMax_O] = (CONFIG_MAX_ARQ << NA_TxArqMax_LSB);
	ntrxShadowReg[NA_CsqUsePhaseShift_O] &= ~((1 << NA_CsqUsePhaseShift_B)
							| (1 << NA_CsqAsyMode_B));
	ntrxShadowReg[NA_CsqUseRam_O] |= (1 << NA_CsqUseRam_B);
	NTRXSPIWrite (NA_TxArqMax_O, ntrxShadowReg + NA_TxArqMax_O, 2);

	ntrxShadowReg[NA_EnableLO_O] &= ~((1 << NA_EnableLO_B) | (1 << NA_EnableLOdiv10_B));
	ntrxShadowReg[NA_EnableExtPA_O] |= ((1 << NA_EnableExtPA_B)
							| (1 << NA_EnableCsqClock_B));
	ntrxShadowReg[NA_RfRxCompValueI_O] &= ~(0x0f << NA_RfRxCompValueI_LSB);
	ntrxShadowReg[NA_RfRxCompValueI_O] |= (0x0e << NA_RfRxCompValueI_LSB);
	ntrxShadowReg[NA_RfRxCompValueQ_O] &= ~(0x0f << NA_RfRxCompValueQ_LSB);
	ntrxShadowReg[NA_RfRxCompValueQ_O] |= (0x0e << NA_RfRxCompValueQ_LSB);
	NTRXSPIWrite (NA_EnableExtPA_O, ntrxShadowReg + NA_EnableExtPA_O, 7);
	ntrxShadowReg[NA_TxScrambEn_O] |= (1 << NA_TxScrambEn_B);
	ntrxShadowReg[NA_TxAddrSlct_O] &= ~(1 << NA_TxAddrSlct_B);
	NTRXSPIWrite (NA_TxArq_O, ntrxShadowReg + NA_TxArq_O , 7);
	ntrxShadowReg[NA_RxCrc2Mode_O] |= (NA_RxCrc2ModeTrigOn_BC_C << NA_RxCrc2Mode_B);
	ntrxShadowReg[NA_RxTimeBCrc1Mode_O] &= ~(NA_RxTimeBCrc1ModeOn_BC_C << NA_RxTimeBCrc1Mode_B);
	NTRXSPIWriteByte( NA_RxArqMode_O, ntrxShadowReg[NA_RxArqMode_O] );

#	ifdef CONFIG_NTRX_SNIFFER
	ntrxShadowReg[NA_RxAddrMode_O] &= ~(1 << NA_RxAddrMode_B);
//	ntrxShadowReg[0x5b] |= (1 << NA_RxDataEn_B) | (1 << NA_RxBrdcastEn_B) | (1 << NA_RxTimeBEn_B); // set by default
	NTRXSPIWriteByte( NA_RxAddrMode_O, ntrxShadowReg[NA_RxAddrMode_O] );
#	else
	ntrxShadowReg[NA_RxAddrMode_O] |= (1 << NA_RxAddrMode_B);
#	endif

	ntrxShadowReg[NA_GateSizeFramesync_O]  = ((NA_GateSize5Slots_VC_C << NA_GateSizeFramesync_LSB)
							| (NA_GateSizeUnsync_I << NA_GateSizeUnsync_LSB)
							| (NA_GateSizeBitsync_I << NA_GateSizeBitsync_LSB)
							| ( 1 << NA_GateAdjBitsyncEn_B )
							);
	NTRXSPIWriteByte( NA_GateSizeFramesync_O, ntrxShadowReg[NA_GateSizeFramesync_O] );



	ntrxShadowReg[NA_AgcValue_O] &= ~(0x3f << NA_AgcValue_LSB);
	ntrxShadowReg[NA_AgcValue_O] |= (23 << NA_AgcValue_LSB);
	NTRXSPIWriteByte (NA_AgcValue_O, ntrxShadowReg[NA_AgcValue_O]);

	HWDelayms( 2 ); /* 2 ms */

	NTRXFctCal ();

    NTRXSetRxIqMatrix (fdma, sd);
	NTRXSetCorrThreshold (fdma, sd);
   	NTRXSetTxIqMatrix (fdma, sd);
	NTRXSetSyncWord(phyPIB.syncword);

    NTRXAllCalibration ();

   	NTRXSetAgcValues (fdma, sd, sr);

	PHYInit ();

	lfdma 	= fdma;
	lsd   	= sd;
	lsr		= sr;
}


/**
 * @brief read out the real time clock of the transceiver.
 * @param force == TRUE lets the function read the RTC anyway
 * @returns the time in 1/32768 seconds (ca. 30 us)
 *
 * This function reads out the real time clock of the transceiver
 *
 */
/**************************************************************************/
MyDword32T NTRXGetRtc (MyBoolT force)
/**************************************************************************/
{
	/*FIXME disabled for debugging!*/
	static MyDword32T currentRTC;					/**< stores the last RTC value*/
	static MyDword32T currentHwclock = 0xFFFFFFFF;	/**< stores the hwclock value when RTC was read*/
	MyDword32T now = hwclock();

	/*
	Avoid redundant access to RTC to save time.
	force == TRUE lets the function read the RTC anyway.
	*/
	if ((now != currentHwclock) || (force))
	{
		MyByte8T rtc[5];
		MyByte8T reg;


		/*read current RTC*/
		ENTER_TASK;
		reg = ntrxShadowReg[NA_RtcCmdRd_O] | ( 1 << NA_RtcCmdRd_B );
		NTRXSPIWriteByte(NA_RtcCmdRd_O, reg);
		HWDelayus( 130 );
		NTRXSPIRead(NA_RamRtcReg_O + 1, rtc, 4);
		reg = 0;
		NTRXSPIWriteByte(NA_RtcCmdRd_O, reg);

		/*update current RTC and hwclock entries*/
		currentRTC = (
				(((MyDword32T)rtc[0])   <<  0)
				| (((MyDword32T)rtc[1]) <<  8)
				| (((MyDword32T)rtc[2]) << 16)
				| (((MyDword32T)rtc[3]) << 24)
		);
		/*FIXME what can we do with the most significant byte?!*/
		//currentRTC = (currentRTC >> 7) | (rtc[5] << 1);
		currentHwclock = now;
		LEAVE_TASK;
	}
	return currentRTC;
}

/**
 * @brief read out the real time clock of the transceiver.
 * @param mode 0 is full and 1 is for pad mode
 * @param seconds wake up time from now
 *
 * This function starts the alarm clock if the parameter seconds is
 * greater than 0 and brings the transceiver into the requested powerdown mode.
 * There are two powerdown modes supported: powerdown mode full and
 * powerdown mode pad. The second mode can be used to supervise the dig-IO
 * pins of the nanoLOC chip.
 *
 */
/**************************************************************************/
void NTRXPowerdownMode (MyByte8T mode, MyDword32T seconds)
/**************************************************************************/
{
	MyByte8T 	value, valNull = 0;
	MyByte8T 	i;
	MyByte8T 	wakeupTime8;
	MyDword32T wakeupTime32;

	/*Stop receiver*/
	NTRXSPIWriteByte (NA_RxCmdStop_O, ntrxShadowReg[NA_RxCmdStop_O] | (1 << NA_RxCmdStop_B));

	if (seconds > 0)
	{
		wakeupTime32 = (seconds << 7) + NTRXGetRtc(TRUE);

		/*write down wake up time*/
		for (i = NA_WakeUpTimeWe_LSB; i <= NA_WakeUpTimeWe_MSB; i++)
		{

			/*convert 32bit integer wakeup time -> RTC value*/
			wakeupTime8 = (MyByte8T)(wakeupTime32 & 0xFF);
			wakeupTime32 >>= 8;

			/*write one of the 3 bytes into the RtcWakeUpTime Register*/
			NTRXSPIWriteByte( NA_WakeUpTimeByte_O, wakeupTime8 );

			/*generate Strobe to store value in the right place*/
			value = 1 << (i);
			NTRXSPIWriteByte( NA_WakeUpTimeWe_O, value );
			NTRXSPIWriteByte( NA_WakeUpTimeWe_O, valNull );
		}
	}

	value = ( 0x00 | ( 1 << NA_ResetBbClockGate_B ));
	NTRXSPIWriteByte( NA_PowerDown_O, value );
	value = 0;
	NTRXSPIWriteByte( 0x8, value );
	/*set wakeup parameter*/
	if (seconds > 0)
	{
		value = (0x00
			| ( 1 << NA_EnableWakeUpRtc_B )
			| ( 1 << NA_PowerUpTime_LSB )
			| (( 0x01 & mode ) << NA_PowerDownMode_B ));
	}
	else
	{
		value = (( 0x01 & mode ) << NA_PowerDownMode_B );
	}
	NTRXSPIWriteByte( NA_EnableWakeUpRtc_O, value );

	value =(0x00
  			| ( 1 << NA_PowerDown_B )
			| ( 1 << NA_ResetBbClockGate_B ));
	NTRXSPIWriteByte( NA_PowerDown_O, value );
}

/**
 * @brief starts the baseband timer.
 * @param startvalue countdown value in us
 *
 * Starts a countdown from startvalue to zero ( in us )
 *
 */
/**************************************************************************/
void NTRXStartBbTimer (MyInt16T startvalue)
/**************************************************************************/
{
    NTRXSPIWrite (NA_BasebandTimerStartValue_O, (MyByte8T *)(&startvalue), 2);
}

/**
 * @brief stops the baseband timer.
 *
 * Stops the baseband timer. The baseband timer is recommended for very
 * short timer intervals < 1 ms
 *
 */
/**************************************************************************/
void NTRXStopBbTimer (void)
/**************************************************************************/
{
	NTRXSPIWriteByte (NA_ClearBasebandTimerInt_O, ntrxShadowReg[NA_ClearBasebandTimerInt_O] | (1 << NA_ClearBasebandTimerInt_B));
}

/**
 * @brief Testmode for chip measurement.
 *
 * This function enables or disables the chip test mode. In this mode
 * a continuous chirp is transmitted. This requires to swich off the receiver.
 * Note: This function should be interrupted periodically and the transmitter
 * recalibrated. Otherwise the frequency might be out of range.
 */
/**************************************************************************/
void NTRXSetTestChirpMode (MyBoolT value)
/**************************************************************************/
{
	NTRXSPIWriteByte (NA_RxCmdStop_O, ntrxShadowReg[NA_RxCmdStop_O] | (1 << NA_RxCmdStop_B));
	NTRXSPIWriteByte (NA_RxIntsReset_O, 0x7f);

	if (value == TRUE)
	{
		ntrxShadowReg[NA_CsqUsePhaseShift_O] |= (1 << NA_CsqUsePhaseShift_B);
		NTRXSPIWriteByte (NA_CsqUsePhaseShift_O, ntrxShadowReg[NA_CsqUsePhaseShift_O]);

		ntrxShadowReg[NA_EnableLO_O] |= ((1 << NA_EnableLO_B) | (1 << NA_EnableLOdiv10_B) | (1 << NA_EnableCsqClock_B));
		NTRXSPIWriteByte (NA_EnableLO_O, ntrxShadowReg[NA_EnableLO_O]);

		ntrxShadowReg[NA_EnableTx_O] |= (1 << NA_EnableTx_B);
		NTRXSPIWriteByte (NA_EnableTx_O, ntrxShadowReg[NA_EnableTx_O]);

		ntrxShadowReg[NA_EnableExtPA_O] |= 0x20;
		NTRXSPIWriteByte (NA_EnableExtPA_O, ntrxShadowReg[NA_EnableExtPA_O]);

		ntrxShadowReg[NA_CsqUseRam_O] |= 0x80;
		NTRXSPIWriteByte (NA_CsqUseRam_O, ntrxShadowReg[NA_CsqUseRam_O]);
	}
	else
	{
		ntrxShadowReg[NA_CsqUseRam_O] &= ~(0x80);
		NTRXSPIWriteByte (NA_CsqUseRam_O, ntrxShadowReg[NA_CsqUseRam_O]);

		ntrxShadowReg[NA_EnableExtPA_O] &= ~(0x20);
		NTRXSPIWriteByte (NA_EnableExtPA_O, ntrxShadowReg[NA_EnableExtPA_O]);

		ntrxShadowReg[NA_EnableTx_O] &= ~(1 << NA_EnableTx_B);
		NTRXSPIWriteByte (NA_EnableTx_O, ntrxShadowReg[NA_EnableTx_O]);

		ntrxShadowReg[NA_EnableCsqClock_O] &= ~(1 << NA_EnableCsqClock_B);
		ntrxShadowReg[NA_EnableLOdiv10_O] &= ~(1 << NA_EnableLOdiv10_B);
		ntrxShadowReg[NA_EnableLO_O] &= ~(1 << NA_EnableLO_B);
		NTRXSPIWriteByte (NA_EnableLO_O, ntrxShadowReg[NA_EnableLO_O]);

		ntrxShadowReg[NA_CsqUsePhaseShift_O] &= ~(1 << NA_CsqUsePhaseShift_B);
		NTRXSPIWriteByte (NA_CsqUsePhaseShift_O, ntrxShadowReg[NA_CsqUsePhaseShift_O]);
	}
}

/**
 * @brief Carriermode for chip measurement.
 *
 *
 *
 * This function enables or disables the unmodulated carrier signal test mode by
 * sending a continuous carrier. This requires to swich off the receiver.
 * Note: This function should be periodically interrupted and the transmitter
 * recalibrated. Otherwise the frequency might be out of range.
 */
/**************************************************************************/
void NTRXSetTestCarrierMode (MyBoolT value)
/**************************************************************************/
{
	static MyByte8T fdma;

	NTRXSPIWriteByte (NA_RxCmdStop_O, ntrxShadowReg[NA_RxCmdStop_O] | (1 << NA_RxCmdStop_B));
	NTRXSPIWriteByte (NA_RxIntsReset_O, 0x7f);

	if (value == TRUE)
	{
		fdma = ntrxShadowReg[NA_FdmaEnable_O];
		ntrxShadowReg[NA_FdmaEnable_O] &= ~(1 << NA_FdmaEnable_B);
		NTRXSPIWriteByte (NA_FdmaEnable_O, ntrxShadowReg[NA_FdmaEnable_O]);

		ntrxShadowReg[NA_EnableLO_O] |= ((1 << NA_EnableLO_B) | (1 << NA_EnableLOdiv10_B));
		NTRXSPIWriteByte (NA_EnableLO_O, ntrxShadowReg[NA_EnableLO_O]);

		ntrxShadowReg[NA_CsqSetValue_O] = 0x1F;
		NTRXSPIWriteByte (NA_CsqSetValue_O, ntrxShadowReg[NA_CsqSetValue_O]);

		ntrxShadowReg[NA_CsqSetValue_O] |= (1 << 6);
		NTRXSPIWriteByte (NA_CsqSetValue_O, ntrxShadowReg[NA_CsqSetValue_O]);

		ntrxShadowReg[NA_CsqSetValue_O] = 0x3F;
		NTRXSPIWriteByte (NA_CsqSetValue_O, ntrxShadowReg[NA_CsqSetValue_O]);

		ntrxShadowReg[NA_CsqSetValue_O] |= (1 << 7);
		NTRXSPIWriteByte (NA_CsqSetValue_O, ntrxShadowReg[NA_CsqSetValue_O]);

		ntrxShadowReg[NA_EnableTx_O] |= ((1 << NA_EnableTx_B) | (0x07 << NA_ChirpFilterCaps_LSB));
		NTRXSPIWriteByte (NA_EnableTx_O, ntrxShadowReg[NA_EnableTx_O]);

		ntrxShadowReg[NA_EnableExtPA_O] |= 0x20;
		NTRXSPIWriteByte (NA_EnableExtPA_O, ntrxShadowReg[NA_EnableExtPA_O]);
	}
	else
	{
		ntrxShadowReg[NA_EnableExtPA_O] &= ~(0x20);
		NTRXSPIWriteByte (NA_EnableExtPA_O, ntrxShadowReg[NA_EnableExtPA_O]);

		ntrxShadowReg[NA_EnableTx_O] &= ~(1 << NA_EnableTx_B);
		NTRXSPIWriteByte (NA_EnableTx_O, ntrxShadowReg[NA_EnableTx_O]);
		ntrxShadowReg[NA_EnableLO_O] &= ~((1 << NA_EnableLO_B)
											| (1 << NA_EnableLOdiv10_B)
											| (1 << NA_EnableCsqClock_B));
		NTRXSPIWriteByte (NA_EnableLO_O, ntrxShadowReg[NA_EnableLO_O]);

		ntrxShadowReg[NA_FdmaEnable_O] = fdma;
		NTRXSPIWriteByte (NA_FdmaEnable_O, ntrxShadowReg[NA_FdmaEnable_O]);
	}
}

