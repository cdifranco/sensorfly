/* $Id$ */

/**
 * @file ntrxutil.h
 * @date 2007-Dez-11
 * @author S.Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief Utility functions to operate the nanoLOC chip.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This module contains utility functions to operate the nanoLOC chip.
 */

/*
 * $Log$
 */

#ifndef NTRXUTIL_H
#define NTRXUTIL_H

#include 	"config.h"
#include 	"ntrxtypes.h"
#include    "registermap.h"

/** @brief Structure for configuration settings
 *
 * To initialize the nanoLOC chip to a specific transmission mode
 * all necessary parameters are combined in this structure.
 */
typedef struct
{
    MyByte8T    bw;			/**< Bandwidth 					*/
    MyByte8T    sd;			/**< Symbolduration 			*/
    MyByte8T    sr;			/**< Symbolrate 				*/
    MyByte8T    fdma;			/**< Fdma (22MHz) or 80MHz mode */
    MyByte8T    fixnmap;		/**< Fixed or map mode 			*/
	MyByte8T	 fec;
	MyDouble32T rangingConst_FECon;	/**< Ranging constant fec on		*/
	MyDouble32T rangingConst_FECoff;	/**< Ranging constant fec off	*/
	MyWord16T	 rangingTimeout;/**< cbo add comment 			*/
	MyWord16T	 rangingFastTimeout; /**< cbs add comment		*/
}PhyModeSetT;

extern PhyModeSetT modeSet;

/** @brief structure type for all layer configuration settings
 *
 * To be IEEE layer complient, all configuration data is stored
 * in one data structure that can be set or read by an upper layer.
 */
typedef struct
{
    MyByte8T	currentChannel;	/**< selected channel. */
	MyByte8T	logChannel;		/**< logical channel id. */

 	MyByte8T	txPower;		/**< tx power setting for the transmitter. */
	MyByte8T	rxState;		/**< receiver state. */
	MyByte8T	txState;		/**< transmitter state. */

	MyByte8T	macAddr0[6];	/**< 1st MAC address of nanoLOC chip. */
	MyByte8T	macAddr1[6];	/**< 2nd MAC address of nanoLOC chip. */
	MyByte8T	txAddrSel;		/**< selection for MAC addr 0 / 1. */
	MyByte8T	arqMax;			/**< max number of retransmissions before no ack 0 - 14. */
	MyByte8T	arqMode;		/**< hardware acks enabled or disabled */
	MyByte8T	addrMatch; 		/**< promisc / auto mode. */
	MyByte8T	rawMode;		/**< raw / auto mode. */
	MyByte8T	trxMode;		/**< trx mode. */
	MyDword32T recalInterval;  /**< time interval in ms for recalibration */
	MyDword32T phyRxTimeout;	/**< timeout if no packet received [ms] */
	MyByte8T	pwrDownMode;	/**< 0 full 1 pad 2 up */
	MyByte8T	frameType;      /**< supported frame types */
	MyByte8T	testmode;		/**< set cont. chirp or carrier mode */
	MyByte8T	syncword[8];	/**< syncword for communication */
	MyByte8T 	rxOn;			/**< is rx already started */
	MyByte8T	fec;			/**< fec on or off */
} PhyPIB;

// extern MyBoolT bRxCrc2Mode;
extern MyBoolT  ntrxRun;
extern MyByte8T ntrxIrqStatus[2];
// extern MyAddressT ntrxMacAddr;
extern MyByte8T ntrxState;
extern MyByte8T ntrxCal;


#define SHADOWREGCOUNT        128
/* COldfire timer for 66Mhz */
#define DELAY_20us      hwdelay(40) /* 20 */
#define DELAY_100us     hwdelay(130)
#define DELAY_800us     hwdelay(3200) /* 1100 */
#define DELAY_20ms  { hwdelay(8000); hwdelay(8000); hwdelay (8000); hwdelay (3000); }
#define RfTxOutputPowerData_DEFAULT         0x3f
#define RfTxOutputPowerReq_DEFAULT          0x3f
#define NTRX_MAX_CHANNEL_ID	16

#define TxIDLE      0x00
#define TxSEND      0x01
#define TxWAIT      0x02

#define NoCAL       0X00
#define TxCAL       0x01
#define RxCAL       0x02
#define AllCAL      0x03
#define NA_MAX_ARQ_CNT  0x0A

#include <avr/pgmspace.h>
#define ReadRomByte(p) pgm_read_byte(&(p))
#define ReadRomWord(p) pgm_read_word(&(p))

#define txIrqStatus (ntrxIrqStatus[0])
#define rxIrqStatus (ntrxIrqStatus[1])


enum PWROUTTYPE
{
    pwrOutData = 0,
    pwrOutReqst = 1
};

extern MyByte8T ntrxShadowReg[SHADOWREGCOUNT];

void NTRXSetIndexReg (MyByte8T page);
void NTRXSetChannel (MyByte8T value);
void NTRXPowerdownMode (MyByte8T mode, MyDword32T seconds);
void NTRXResetSettings (void);
void NTRXStartBbTimer (MyInt16T startvalue);
void NTRXStopBbTimer (void);
void NTRXSetupTrxMode (MyByte8T fdma, MyByte8T sd, MyByte8T sr);
MyBoolT NTRXAllCalibration (void);
void NTRXSetRxIqMatrix (MyByte8T bandwidth, MyByte8T symbolDur);
void NTRXSetTxIqMatrix (MyByte8T bandwidth, MyByte8T symbolDur);
void NTRXInitShadowRegister (void);
MyBoolT NTRXCheckVerRev (void);
void NTRXGetPayload(MyByte8T *payload, MyByte8T len);
void NTRXSetTestChirpMode (MyBoolT value);
void NTRXSetTestCarrierMode (MyBoolT value);
void NTRXSetSyncWord (MyByte8T *value);


/* this defines are defined in ranging.c*/
#define NA_80MHz            0
#define NA_22MHz            1

#define NA_125k_S  NA_SymbolRate125kSymbols_VC_C
#define NA_250k_S  NA_SymbolRate250kSymbols_VC_C
#define NA_500k_S  NA_SymbolRate500kSymbols_VC_C
#define NA_1M_S    NA_SymbolRate1MSymbols_VC_C
#define NA_2M_S    NA_SymbolRate2MSymbols_VC_C

#define NA_500ns    NA_SymbolDur500ns_C
#define NA_1us      NA_SymbolDur1000ns_C
#define NA_2us      NA_SymbolDur2000ns_C
#define NA_4us      NA_SymbolDur4000ns_C
#define NA_8us      NA_SymbolDur8000ns_C
#define NA_16us     NA_SymbolDur16000ns_C

#define NA_MAP_MODE 0
#define NA_FIX_MODE 1

/* timeout for ranging in [ms] */
#define RANGING_TIMEOUT_13MS (MyWord16T)13000 / 2
#define RANGING_TIMEOUT_9MS  (MyWord16T)9000 / 2
#define RANGING_TIMEOUT_7MS  (MyWord16T)7000 / 2
#define RANGING_TIMEOUT_6MS  (MyWord16T)6000 / 2
#define RANGING_TIMEOUT_5MS  (MyWord16T)5000 / 2
#define RANGING_TIMEOUT_4MS  (MyWord16T)4000 / 2

/* recalibrationtime [ms] */
#define RECAL_TIME	(MyWord16T)20000 / 2



#endif /* NTRXUTIL_H */
