/* $Id$ */
/**
 * @file ntrxinit.c
 * @date 2007-Dez-4
 * @author S.Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief Functions for the initialization of the nanoLOC transceiver.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the source code for the implementation of the
 * NA5TR1 initialisation.
 *
 */

/*
 * $Log$
 */

#include	"config.h"
#include	"ntrxtypes.h"
#include    "ntrxutil.h"
#include    "hwclock.h"
#include    "nnspi.h"
#include	<string.h>

extern PhyPIB phyPIB;
extern void error_handler (MyInt16T e);

MyBoolT	first = TRUE;			/**< Initialize certain registers only one time. */
/*
 * do not change this syncword, since not every 64 bit sequence is valid
 */
const MyByte8T TRX_SYNC_WORD[] =  	/**< Default sync word for transceiver. */
{
	0xAB,0x69,0xCA,0x94,0x92,0xD5,0x2C,0xAB
};

/**
 * @brief Initializing of the transceiver chip.
 *
 * This function initializes all registers of the nanoLOC transceiver chip.
 * This function should only be called once.
 *
 * If this function is called for the first time, the structure settingVal is
 * set to the predefined default mode values. After setting up the SPI interface
 * this function compares the version and revision register of the chip with
 * the values expected by the driver. If these don't match, the software will
 * call an error function and halts. The reason for this drastic reaction
 * is that the driver can not garantie a non interfering behaviour.
 *
 */
/**************************************************************************/
void NTRXInit(void)
/**************************************************************************/
{
	if (first == TRUE)
	{
		first = FALSE;


#		ifdef CONFIG_DEFAULT_TRX_22MHZ_1000NS
		modeSet.bw = NA_22MHz;
		modeSet.sd = NA_1us;
		modeSet.sr = NA_1M_S;
		modeSet.fdma= TRUE;
		modeSet.fec = FALSE;
		modeSet.fixnmap=NA_FIX_MODE;
		modeSet.rangingConst_FECon=0.0; /* no ranging allowed */
		modeSet.rangingTimeout=0; /* no ranging allowed */
		modeSet.rangingFastTimeout=0;   /* no ranging allowed */
#		endif /* CONFIG_DEFAULT_TRX_22MHZ_1000NS */

#		ifdef CONFIG_DEFAULT_TRX_22MHZ_2000NS
		modeSet.bw = NA_22MHz;
		modeSet.sd = NA_2us;
		modeSet.sr = NA_500k_S;
		modeSet.fdma= TRUE;
		modeSet.fec = FALSE;
		modeSet.fixnmap=NA_FIX_MODE;
		modeSet.rangingConst_FECon=0.0; /* no ranging allowed */
		modeSet.rangingTimeout=0; /* no ranging allowed */
		modeSet.rangingFastTimeout=0;   /* no ranging allowed */
#		endif /* CONFIG_DEFAULT_TRX_22MHZ_2000NS */

#		ifdef CONFIG_DEFAULT_TRX_22MHZ_4000NS
		modeSet.bw = NA_22MHz;
		modeSet.sd = NA_4us;
		modeSet.sr = NA_250k_S;
		modeSet.fdma= TRUE;
		modeSet.fec = FALSE;
		modeSet.fixnmap=NA_FIX_MODE;
		modeSet.rangingConst_FECon=0.0; /* no ranging allowed */
		modeSet.rangingTimeout=0; /* no ranging allowed */
		modeSet.rangingFastTimeout=0;   /* no ranging allowed */
#		endif /* CONFIG_DEFAULT_TRX_22MHZ_4000NS */

#		ifdef CONFIG_DEFAULT_TRX_80MHZ_500NS
		modeSet.bw = NA_80MHz;
		modeSet.sd = NA_500ns;
		modeSet.sr = NA_2M_S;
		modeSet.fdma= FALSE;
		modeSet.fec = FALSE;
		modeSet.fixnmap=NA_FIX_MODE;
		modeSet.rangingConst_FECon=68.929336; /**/
		modeSet.rangingConst_FECoff=53.991800; /**/
		modeSet.rangingTimeout		=	RANGING_TIMEOUT_6MS;
		modeSet.rangingFastTimeout	=	RANGING_TIMEOUT_4MS;
#		endif /* CONFIG_DEFAULT_TRX_80MHZ_500NS */

#		ifdef CONFIG_DEFAULT_TRX_80MHZ_1000NS
		modeSet.bw = NA_80MHz;
		modeSet.sd = NA_1us;
		modeSet.sr = NA_1M_S;
		modeSet.fdma= FALSE;
		modeSet.fec = FALSE;
		modeSet.fixnmap=NA_FIX_MODE;
		modeSet.rangingConst_FECon=122.554497; /**/
		modeSet.rangingConst_FECoff=92.554000; /**/
		modeSet.rangingTimeout		=	RANGING_TIMEOUT_7MS;
		modeSet.rangingFastTimeout	=	RANGING_TIMEOUT_5MS;
#		endif /* CONFIG_DEFAULT_TRX_80MHZ_1000NS */

#		ifdef CONFIG_DEFAULT_TRX_80MHZ_2000NS
		modeSet.bw = NA_80MHz;
		modeSet.sd = NA_2us;
		modeSet.sr = NA_500k_S;
		modeSet.fdma= FALSE;
		modeSet.fec = FALSE;
		modeSet.fixnmap=NA_FIX_MODE;
		modeSet.rangingConst_FECon=229.552500; /**/
		modeSet.rangingConst_FECoff=169.552500; /**/
		modeSet.rangingTimeout		=	RANGING_TIMEOUT_9MS;
		modeSet.rangingFastTimeout	=	RANGING_TIMEOUT_7MS;
#		endif /* CONFIG_DEFAULT_TRX_80MHZ_2000NS */

#		ifdef CONFIG_DEFAULT_TRX_80MHZ_4000NS
		modeSet.bw = NA_80MHz;
		modeSet.sd = NA_4us;
		modeSet.sr = NA_250k_S;
		modeSet.fdma= FALSE;
		modeSet.fec = FALSE;
		modeSet.fixnmap=NA_FIX_MODE;
		modeSet.rangingConst_FECon=445.647000; /**/
		modeSet.rangingConst_FECoff=325.647000; /**/
		modeSet.rangingTimeout		=	RANGING_TIMEOUT_13MS;
		modeSet.rangingFastTimeout	=	RANGING_TIMEOUT_9MS;
#		endif /* CONFIG_DEFAULT_TRX_80MHZ_4000NS */

    	/* init SPI HW&SW */
    	InitSPI();
    	NanoReset();

	    /* initialize shadow registers */
    	NTRXInitShadowRegister ();
		/* configure SPI output of chip MSB first / push pull */
		SetupSPI ();

    	HWDelayus( 1 ); //!! Some short delay seems necessary here??
	    /* check connection and firmware version and revision */
    	if (!NTRXCheckVerRev())
    	{
    		error_handler (506);
    	}
	}
	else
	{
		/* configure SPI output of chip MSB first / push pull */
		SetupSPI ();
	    /* initialize shadow registers */
    	NTRXInitShadowRegister ();
	}
	memcpy (phyPIB.syncword, TRX_SYNC_WORD, 8 );
    NTRXSetupTrxMode (modeSet.bw, modeSet.sd, modeSet.sr);

}
