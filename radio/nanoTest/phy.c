/* $Id$ */

/**
 * @file phy.c
 * @date 2007-Dez-4
 * @author S.Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief Functions for data transmission and reception.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This module contains all functions to operate the nanoLOC chip.
 */

/*
 * $Log$
 */
#include <string.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "phy.h"
#include "ntrxutil.h"
#include "nnspi.h"
#include "hwclock.h"
#include "ntrxranging.h"

/**
 * @def SendMsgUp
 * @brief Callback function of upper layer.
 *
 * This function is called when the physical layer wants to forward received
 * data to the next upper layer. This define is used to be able to add
 * another layer.
 *
 */
#define SendMsgUp APLCallback
/**
 * @def SendCfgUp
 * @brief Callback function of upper layer.
 *
 * This function is called when the physical layer wants to send configuration
 * data to the next upper layer. This define is used to be able to add
 * another layer.
 *
 */
#define SendCfgUp(p)

/* SCxET:TRUE:DROPSTART standalone.pc_usb */
/* SCxET:TRUE:DROPSTART standalone.pc_usb, standalone.atmega, sirona.atmega */
/* SCxET:TRUE:DROPSTART standalone.pc_usb, standalone.stm32_generic */

#ifdef CONFIG_TRAFFIC_LED
#include "led.h"

/**
 * @def LED_DELAY
 * @brief on time for LED event indication.
 *
 * This defines the time in milliseconds the LED stays on to indicate an event.
 *
 */
#define LED_DELAY 50

/**
 * @def TRIGGER_LED_TX
 * @brief LED 0 is used for tx event indication.
 *
 * This define starts the timer and switches the LED 0 on. The LED is turned off
 * by UpdateLEDs();
 */
#ifdef CONFIG_USE_TX_LED
MyDword32T txLED; 	/**< timestamp for transmitter LED. */
#	define TRIGGER_LED_TX(); { txLED = hwclock () + LED_DELAY; LED_TX(LED_ON); }
#else
#	define TRIGGER_LED_TX();		{}
#endif

/**
 * @def TRIGGER_LED_RX
 * @brief LED 1 is used for rx event indication.
 *
 * This define starts the timer and switches the LED 1 on. The LED is turned off
 * by UpdateLEDs();
 */
#ifdef CONFIG_USE_RX_LED
MyDword32T rxLED; 	/**< timestamp for receiver LED. */
#	define TRIGGER_LED_RX();	{ rxLED = hwclock () + LED_DELAY; LED_RX(LED_ON); }
#else
#	define TRIGGER_LED_RX();	{}
#endif

/**
 * @def TRIGGER_LED_ERR
 * @brief LED 2 is used for no acknowledge event indication.
 *
 * This define starts the timer and switches the LED 2 on. The LED is turned off
 * by UpdateLEDs();
 */
#ifdef CONFIG_USE_ERR_LED
MyDword32T errLED;	/**< timestamp for no acknowledge LED. */
#	define TRIGGER_LED_ERR(); { errLED = hwclock () + LED_DELAY; LED_ERR(LED_ON); }
#else
#	define TRIGGER_LED_ERR(); { }
#endif

/**
 * @def TRIGGER_LED_CAL
 * @brief LED 3 is used for recalibration indication.
 *
 * This define starts the timer and switches the LED 3 on. The LED is turned off
 * by UpdateLEDs();
 */
#ifdef CONFIG_USE_CAL_LED
MyDword32T calLED;	/**< timestamp for recalibration LED. */
#	define TRIGGER_LED_CAL(); { calLED = hwclock () + LED_DELAY; LED_CAL(LED_ON); }
#else
#	define TRIGGER_LED_CAL(); { }
#endif

/**
 * @def UpdateLEDs
 * @brief supervise the traffic LEDs.
 *
 * This define the supervisary timer to switch the traffic LEDs off automatically.
 */
/****************************************************************************/
void UpdateLEDs (void)
/****************************************************************************/
{
	unsigned long tiStamp;
    tiStamp = hwclock ();

#	ifdef CONFIG_USE_TX_LED
    if (txLED < tiStamp)
	{
		LED_TX(LED_OFF);
	}
#	endif
#	ifdef CONFIG_USE_RX_LED
    if (rxLED < tiStamp)
	{
		LED_RX(LED_OFF);
	}
#	endif
#	ifdef CONFIG_USE_ERR_LED
    if (errLED < tiStamp)
	{
		LED_ERR(LED_OFF);
	}
#	endif

#	ifdef CONFIG_USE_CAL_LED
	if (calLED < tiStamp)
	{
		LED_CAL(LED_OFF);
	}
#	endif
}
#else
#	define TRIGGER_LED_TX();
#   define TRIGGER_LED_RX();
#   define TRIGGER_LED_ERR();
#   define TRIGGER_LED_CAL();
#   define UpdateLEDs();
#endif /* CONFIG_TRAFFIC_LED */

void NTRXInterrupt (void);

extern void APLCallback(MyMsgT *msg);
extern RangingPIB rangingPIB;

/****************************************************************************
 *  Local stuff
 ****************************************************************************/
MyMsgT upMsg;					/**< message struct for all received data. */
MyInt16T rcwd = 0;				/**< retransmission counter for dynamic recalibration. */
MyDword32T tiRecal;			/**< timestamp of last calibration. */
MyDword32T tiPhyRxTimeout;		/**< timestamp of last rx. */
MyBoolT tiPhyRxTimeout_once;	/**< report only once time */
MyByte8T txIrq;				/**< tx interrupt status register. */
MyByte8T rxIrq;				/**< rx interrupt status register. */
MyByte8T lastArqCount;			/**< number of retransmissions of last transmitted message. */
MyMsgT *txSendMsg = NULL;		/**< pointer to store last transmitted message. */
MyBoolT  trxPollMode = TRUE;	/**< flag for irq polling. */
MyByte8T buffSwapped = FALSE;	/**< swap buffer state for accessing the right memory block */

#define BB_STATE_NORMAL_PHASE1	0
#define BB_STATE_NORMAL_PHASE2	1
#define BB_STATE_FAST_PHASE1	2
#define BB_STATE_FAST_PHASE2	3

MyByte8T lState;				/**< ranging stuff cbo. */
MyByte8T rState;				/**< ranging stuff cbo. */
MyByte8T bbState;				/**< ranging stuff cbo. */
MyAddrT  rDest;				/**< ranging stuff cbo. */
MyByte8T bbtimerStatus;		/**< ranging stuff cbo. */
MyByte8T bbIrq;				/**< ranging stuff cbo. */

/** @brief structure for all layer configuration settings  */
PhyPIB phyPIB;

/**
 * @brief Initializing of module global variables.
 *
 * This function initializes all module global variables located in the
 * physical layers parameter information base (phyPIB). The nanoLOC chip
 * has to be already initialized and in ready (idle) mode. The receiver is started
 * and interrupts for CRC2 and Tx End and Rx End are enabled.
 *
 */
/****************************************************************************/
void PHYInit (void)
/****************************************************************************/
{
	phyPIB.currentChannel = PHY_CHANNEL_MIN;
	phyPIB.txPower		  = 0x3f;
	phyPIB.pwrDownMode	  = 2;
	txSendMsg			  = NULL;
	phyPIB.recalInterval  = CONFIG_NTRX_RECAL_DELAY;
	phyPIB.phyRxTimeout   = CONFIG_NTRX_PHY_RX_TIMEOUT;
	tiPhyRxTimeout 		  = hwclock() + phyPIB.phyRxTimeout;
	tiPhyRxTimeout_once	  = FALSE;
	phyPIB.arqMax 		  = CONFIG_MAX_ARQ;
	phyPIB.testmode		  = 0;
	phyPIB.fec			  = FALSE;
	trxPollMode = TRUE;

	/*
     * Clear and reset all interrupt flags of the nanoLOC chip
     */
	ntrxShadowReg[NA_TxIntsReset_O] = 0x3f;
	ntrxShadowReg[NA_RxIntsReset_O] = 0x7f;
	/*
	 * enable tx and rx interrupts
	 */
	ntrxShadowReg[NA_RxIrqEnable_O] |= ((1 << NA_RxIrqEnable_B) | (1 << NA_TxIrqEnable_B));

	NTRXResetSettings ();

	/*
     * enable CRC checks on received messages. This will cause corrupt frames to be droped
     * silently by the nanoLOC chip. The receiver is restarted automatically.
     */
    if ((ntrxShadowReg[NA_RxCrc2Mode_O] & (1<< NA_RxCrc2Mode_B)) !=  0)
	{
		ntrxShadowReg[NA_RxIntsEn_O] = (0x01 << NA_RxEnd_B);
	}
    else
	{
		ntrxShadowReg[NA_RxIntsEn_O] = ((0x01 << NA_RxEnd_B) | (0x01 << NA_RxOverflow_B));
	}
	ntrxShadowReg[NA_TxIntsEn_O] = (1 << NA_TxEnd_B);

	NTRXSPIWrite (NA_RxIrqEnable_O, ntrxShadowReg + 0x0f, 6);
    /*
     * initialize layer global variables
     */
    txIrqStatus = 0;
    rxIrqStatus = 0;
    txIrq = 0;
    rxIrq = 0;
    bbIrq = 0;
	lState = RANGING_READY;
	rState = RANGING_READY;
	ntrxShadowReg[NA_TxIntsReset_O] = 0;
	ntrxShadowReg[NA_RxIntsReset_O] = 0;

    /*
     * start the receiver of the TRX chip
     */
	if (phyPIB.rxState == PHY_RX_ON)
	{
    	NTRXSPIWriteByte (NA_RxCmdStart_O, ntrxShadowReg[NA_RxCmdStart_O]
									| (1 << NA_RxCmdStart_B)
									| (0x03 << NA_RxBufferCmd_LSB));
	}

    ntrxState    = TxIDLE;
}

/**
 * @brief This function processes all data request.
 * @param *msg this is the message pointer
 *
 * This function represents the service access point for data requests and
 * ranging requests. When the primitive is set to @em PD_DATA_REQUEST all
 * necessary actions are taken to copy the payload to the nanoLOC chip
 * and set the MAC header parameter in the corresponding registers.
 * When all this is done the transmission will be initiated.
 * In case of a transmission in progress, @em PDSap will reject the message
 * and indicate a failure with the status @em PHY_BUSY_TX.
 *
 */
/****************************************************************************/
void PDSap (MyMsgT *msg)
/****************************************************************************/
{
    MyByte8T txLen[2];
	static MyAddressT cacheAddr;

	/*
	 * check the message length. If the message length is bigger than
	 * the allowed buffer size the packet will be rejected.
	 */
	if (msg->len > PHY_PACKET_SIZE)
	{
		msg->prim = PD_DATA_CONFIRM;
		msg->status = PHY_BUSY_TX;
		SendMsgUp (msg);
		return;
	}

	switch(msg->prim)
	{
		case PD_DATA_REQUEST :
			/* transmitter still busy */
			if (txSendMsg != NULL)
			{

				msg->prim = PD_DATA_CONFIRM;
				msg->status = PHY_BUSY_TX;
				SendMsgUp (msg);
				return;
			}
			/*
			 * there is no break here !!!
			 * This is intentional and not a mistake.
			 */

		case PD_RANGING_ANSWER1_EXECUTE :
		case PD_RANGING_ANSWER2_EXECUTE :
		case PD_RANGING_REQUEST_EXECUTE :
		case PD_RANGING_FAST_REQUEST_EXECUTE :
		case PD_RANGING_FAST_ANSWER1_EXECUTE :


#	   		ifdef CONFIG_NTRX_AUTO_RECALIB
			if (phyPIB.recalInterval != 0)
			{
				if( tiRecal < hwclock() )
				{
					/* INFO: If the TRX sends a packet, calibration failes!
					 * In this case rcwd is not reset, but tiRecal is.
					 */
					/* normal operation mode */
					if (phyPIB.testmode == 0)
					{
						if (NTRXAllCalibration ())
						{
							tiRecal = hwclock() + phyPIB.recalInterval;
							rcwd = 0;
							TRIGGER_LED_CAL();
						}
					}
				}
			}
#	   		endif /* CONFIG_NTRX_AUTO_RECALIB */
			/* check which buffer is free to transmit data */
			if (buffSwapped ==TRUE)
			{
				/* write user data to transmit buffer in ntrx chip */
				NTRXSetIndexReg (2);
				NTRXSPIWrite ((MyByte8T)(NA_RamTxBuffer_O & 0xff), msg->data, (MyByte8T)(msg->len & 0xff));

			}else
			{
				/* write user data to transmit buffer in ntrx chip */
				NTRXSetIndexReg (3);
				NTRXSPIWrite ((MyByte8T)(NA_RamTxBuffer_O & 0xff), msg->data, (MyByte8T)(msg->len & 0xff));
			}

			NTRXSetIndexReg (0);

			/* copy the destination address to temp buffer */
			if (memcmp (cacheAddr, msg->addr, 6) != 0)
			{
				memcpy (cacheAddr, msg->addr, 6);
				NTRXSPIWrite (NA_RamTxDstAddr_O, cacheAddr, 6);
			}

			/* merge the three bits into the temp buffer */
			txLen[0] = msg->len;
			// txLen[1] = (len & 0x1F00) >> 8;
			txLen[1]  = (lState & 0x01) ? 0x20 : 0;
			txLen[1] |= (lState & 0x02) ? 0x40 : 0;
			txLen[1] |= (lState & 0x04) ? 0x80 : 0;

			NTRXSPIWrite (NA_RamTxLength_O, txLen, 2);
			ntrxState = TxSEND;

			/* mark buffers as valid and start transmission */
			NTRXSPIWriteByte (NA_TxBufferCmd_O, (1 << NA_TxCmdStart_B) | (0x03 << NA_TxBufferCmd_LSB));
			TRIGGER_LED_TX();

			txSendMsg = msg;
			break;

		case PD_RANGING_REQUEST :
		case PD_RANGING_FAST_REQUEST :
			/* transmitter still busy */
			rangingPIB.distance = -1.0;
			rangingPIB.error = STAT_NO_ERROR;

			if (txSendMsg != NULL)
			{

				msg->prim = PD_RANGING_CONFIRM;
				msg->status = PHY_BUSY_TX;
				memcpy(msg->data, (MyByte8T*) &rangingPIB, sizeof(RangingPIB));
				msg->len = sizeof(RangingPIB);
				SendMsgUp (msg);
				return;
			}

			if (lState != RANGING_READY)
			{
				txSendMsg = NULL;

				msg->prim = PD_RANGING_CONFIRM;
				msg->status = PHY_BUSY;
				memcpy(msg->data, (MyByte8T*) &rangingPIB, sizeof(RangingPIB));
				msg->len = sizeof(RangingPIB);
				SendMsgUp (msg);
				return;
			}

			memcpy(rDest, msg->addr, 6);

			if (msg->prim == PD_RANGING_REQUEST)
			{
				lState = RANGING_START;
				RangingMode(RANGING_START, msg->addr);
			}
			else if (msg->prim == PD_RANGING_FAST_REQUEST)
			{
				lState = RANGING_FAST_START;
				RangingMode(RANGING_FAST_START, msg->addr);
			}

			break;

		default:
			break;
	}
}

/**
 * @brief Helper function to switch off test modes.
 *
 * This function switches off a previously enabled test mode.
 * Note: Only one test mode at a time can be enabled.
 */
/****************************************************************************/
void TestmodeOff (void )
/****************************************************************************/
{
	switch (phyPIB.testmode)
	{
		case PHY_CONTINUOUS_MODE: 	NTRXSetTestChirpMode (FALSE);
									break;

		case PHY_CARRIER_MODE: 		NTRXSetTestCarrierMode (FALSE);
									break;

		default:					break;
	}
}

/**
 * @brief Physical layer management entety
 * @param *msg this is the the message struct
 *
 * This function represents the service access point to configure the
 * physical layer. All configuration parameters are stored in the physical layer
 * parameter information base @em phyPIB. This function hides all necessarey steps
 * to enable or disable certain features. With this function it is possible to set values but
 * also to query the following settings:
 *
 * @ref PHY_CURRENT_CHANNEL : the channel is a transmission mode defined in PHY_TRX_MODE
 * @ref PHY_TRX_MODE : a set of three parameters bandwidth, symbol duration and symbol rate
 * @ref PHY_LOG_CHANNEL : is a predefined group of chip settings
 * @ref PHY_TX_POWER : output power setting [ 0 - 63 ]
 * @ref PHY_ARQ : hardware acknowledgement
 * @ref PHY_FEC : forward error correction
 * @ref PHY_MAC_ADDRESS1 : 1st MAC address for address matching
 * @ref PHY_MAC_ADDRESS2 : 2nd MAC address for address matching
 * @ref PHY_TX_ADDR_SELECT : select the MAC address as sender address
 * @ref PHY_ADDR_MATCHING : address matching or promiscuous mode
 * @ref PHY_PWR_DOWN_MODE : set operational mode
 * @ref PHY_RECALIBRATION : recalibration interval
 * @ref PHY_FRAME_TYPE : set frame types that will be handled by the nanoLOC chip
 */
/****************************************************************************/
void PLMESap (MyMsgT *msg)
/****************************************************************************/
{
	MyByte8T bw = 0, sd = 0, br = 0, cf = 0;


	switch (msg->prim)
	{
		case PLME_GET_REQUEST:  msg->prim = PLME_GET_CONFIRM;
								msg->status = PHY_SUCCESS;
								switch (msg->attribute)
								{
									case PHY_CURRENT_CHANNEL: 		msg->value = phyPIB.currentChannel;
																	break;

									case PHY_TRX_MODE:				msg->value = phyPIB.trxMode;
																	break;

									case PHY_LOG_CHANNEL:			msg->value = phyPIB.logChannel;
																	break;

									case PHY_TX_POWER: 				msg->value = phyPIB.txPower;
																	break;

									case PHY_ARQ:					msg->value = phyPIB.arqMode;
																	break;

									case PHY_ARQ_MAX:				msg->value = phyPIB.arqMax;
																	break;

									case PHY_FEC:					msg->value = phyPIB.fec;
																	break;

									case PHY_MAC_ADDRESS1:			memcpy (msg->data, phyPIB.macAddr0, 6);
																	break;

									case PHY_MAC_ADDRESS2:			memcpy (msg->data, phyPIB.macAddr0, 6);
																	break;

									case PHY_TX_ADDR_SELECT:		msg->value = phyPIB.txAddrSel;
																	break;

									case PHY_ADDR_MATCHING:			msg->value = phyPIB.addrMatch;
																	break;


									case PHY_PWR_DOWN_MODE:			msg->value = phyPIB.pwrDownMode;
																	break;

									case PHY_RECALIBRATION:			msg->value = phyPIB.recalInterval / 1000;
																	break;

									case PHY_FRAME_TYPE:			msg->value = phyPIB.frameType;
																	break;

									case PHY_TESTMODE:				msg->value = phyPIB.testmode;
																	break;

									case PHY_RX_STATE:				msg->value = phyPIB.rxState;
																	break;

									default: 						msg->status = PHY_UNSUPPORTED_ATTRIBUTE;
																	break;
								}
								SendCfgUp (msg);
								break;

		case PLME_SET_REQUEST:  msg->prim = PLME_SET_CONFIRM;
								msg->status = PHY_SUCCESS;
								switch (msg->attribute)
								{
									case PHY_CURRENT_CHANNEL: 		if (msg->value > 16)
																	{
																		msg->status = PHY_INVALID_PARAMETER;
																	}
																	else
																	{
																		phyPIB.currentChannel = msg->value;
																		switch (phyPIB.currentChannel)
																		{
																			case 0:	bw = NA_80MHz; sd = NA_1us; br = NA_1M_S;
																					break;
																			default:bw = NA_22MHz; sd = NA_4us; br = NA_250k_S;
																					break;
																		}
																		NTRXSetChannel (phyPIB.currentChannel);
																		NTRXSetupTrxMode (bw, sd, br);
																	}
																	break;

									case PHY_LOG_CHANNEL:			if (msg->value > CONFIG_MAX_LOG_CHANNEL - 1)
																	{
																		msg->status = PHY_INVALID_PARAMETER;
																	}
																	else
																	{
																		phyPIB.logChannel = msg->value;
																		switch (phyPIB.logChannel)
																		{
																			case 1:  bw = NA_22MHz; sd = NA_4us; br = NA_250k_S;
																					 cf = 1;
																					 ntrxShadowReg[NA_UseFec_O] &= ~(1 << NA_UseFec_B);
																					 break;

																			case 2:  bw = NA_22MHz; sd = NA_4us; br = NA_250k_S;
																					 cf = 7;
																					 ntrxShadowReg[NA_UseFec_O] &= ~(1 << NA_UseFec_B);
																					 break;

																			case 3:  bw = NA_22MHz; sd = NA_4us; br = NA_250k_S;
																					 cf = 13;
																					 ntrxShadowReg[NA_UseFec_O] &= ~(1 << NA_UseFec_B);
																					 break;

																			default: bw = NA_80MHz; sd = NA_1us; br = NA_1M_S;
																					 cf = 0;
																					 ntrxShadowReg[NA_UseFec_O] &= ~(1 << NA_UseFec_B);
																					 break;
																		}
																		NTRXSPIWriteByte (NA_UseFec_O, ntrxShadowReg[NA_UseFec_O]);
																		NTRXSetChannel (cf);
																		NTRXSetupTrxMode (bw, sd, br);
																	}
																	break;

									case PHY_TRX_MODE:
																	phyPIB.trxMode = msg->value;
																	switch (msg->value)
																	{
#																		ifdef CONFIG_NTRX_80MHZ_500NS
																		case 10: 	bw = NA_80MHz; sd = NA_500ns; br = NA_125k_S; break;
																		case 11: 	bw = NA_80MHz; sd = NA_500ns; br = NA_1M_S; break;
																		case 12: 	bw = NA_80MHz; sd = NA_500ns; br = NA_2M_S; break;
#																		endif /* CONFIG_NTRX_80MHZ_500NS */

#																		ifdef CONFIG_NTRX_80MHZ_1000NS
																		case 20: 	bw = NA_80MHz; sd = NA_1us; br = NA_500k_S; break;
																		case 21: 	bw = NA_80MHz; sd = NA_1us; br = NA_1M_S; break;
#																		endif /* CONFIG_NTRX_80MHZ_1000NS */

#																		ifdef CONFIG_NTRX_80MHZ_2000NS
																		case 30: 	bw = NA_80MHz; sd = NA_2us; br = NA_125k_S; break;
																		case 31: 	bw = NA_80MHz; sd = NA_2us; br = NA_250k_S; break;
																		case 32: 	bw = NA_80MHz; sd = NA_2us; br = NA_500k_S; break;
#																		endif /* CONFIG_NTRX_80MHZ_2000NS */

#																		ifdef CONFIG_NTRX_80MHZ_4000NS
																		case 40: 	bw = NA_80MHz; sd = NA_4us; br = NA_125k_S; break;
																		case 41: 	bw = NA_80MHz; sd = NA_4us; br = NA_250k_S; break;
#																		endif /* CONFIG_NTRX_80MHZ_4000NS */

#																		ifdef CONFIG_NTRX_22MHZ_1000NS
																		case 50:	bw = NA_22MHz; sd = NA_1us; br = NA_1M_S; break;
#																		endif /* CONFIG_NTRX_22MHZ_1000NS */

#																		ifdef CONFIG_NTRX_22MHZ_2000NS
																		case 60:	bw = NA_22MHz; sd = NA_2us; br = NA_125k_S; break;
																		case 61:	bw = NA_22MHz; sd = NA_2us; br = NA_250k_S; break;
																		case 62:	bw = NA_22MHz; sd = NA_2us; br = NA_500k_S; break;
#																		endif /* CONFIG_NTRX_22MHZ_2000NS */

#																		ifdef CONFIG_NTRX_22MHZ_4000NS
																		case 70:	bw = NA_22MHz; sd = NA_4us; br = NA_125k_S; break;
																		case 71:	bw = NA_22MHz; sd = NA_4us; br = NA_250k_S; break;
#																		endif /* CONFIG_NTRX_22MHZ_4000NS */
																		default: 	printf ("Valid modes: 10-43 (80Mhz) 50-84 (22Mhz) 90-93 (22HR)\n");
																		msg->status = PHY_INVALID_PARAMETER;
																		SendCfgUp (msg);
																		return;
																	}
																	NTRXSetupTrxMode (bw, sd, br);

																	switch (bw)
																	{
																		case NA_80MHz: printf ("New Mode 80 Mhz "); break;
																		case NA_22MHz: printf ("New Mode 22 Mhz "); break;
																		default: printf ("Unknown mode ??? ");
																	}
																	switch (sd)
																	{
																		case NA_500ns: printf ("500 ns, "); break;
																		case NA_1us:   printf ("1 us, "); break;
																		case NA_2us:   printf ("2 us, "); break;
																		case NA_4us:   printf ("4 us, "); break;
																		default: break;
																	}
																	switch (br)
																	{
																		case NA_125k_S: printf ("125 kSym\n"); break;
																		case NA_250k_S: printf ("250 kSym\n"); break;
																		case NA_500k_S: printf ("500 kSym\n"); break;
																		case NA_1M_S: printf ("1 MSym\n"); break;
																		case NA_2M_S: printf ("2 MSym\n"); break;
																		default: break;
																	}
																	break;

									case PHY_TX_POWER:		 		if (msg->value > 63)
																	{
																		msg->status = PHY_INVALID_PARAMETER;
																	}
																	else
																	{
																		phyPIB.txPower = ((MyByte8T)(msg->value) & 0x3f);
																		ntrxShadowReg[NA_TxOutputPower0_O] &= ~(0x3f << NA_TxOutputPower0_LSB);
																		ntrxShadowReg[NA_TxOutputPower0_O] |= (phyPIB.txPower << NA_TxOutputPower0_LSB);
																		if (phyPIB.txPower != 0x3f)
																		{	/* This way no table for the transmission output power is necessary */
																			ntrxShadowReg[NA_TxOutputPower0_O] &= ~(1 << NA_TxOutputPower0_LSB);
																		}
																		NTRXSPIWriteByte (NA_TxOutputPower0_O, ntrxShadowReg[NA_TxOutputPower0_O]);

																		ntrxShadowReg[NA_TxOutputPower1_O] &= ~(0x3f << NA_TxOutputPower1_LSB);
																		ntrxShadowReg[NA_TxOutputPower1_O] |= (phyPIB.txPower << NA_TxOutputPower1_LSB);
																		if (phyPIB.txPower != 0x3f)
																		{	/* This way no table for the transmission output power is necessary */
																			ntrxShadowReg[NA_TxOutputPower1_O] &= ~(1 << NA_TxOutputPower1_LSB);
																		}
																		NTRXSPIWriteByte (NA_TxOutputPower1_O, ntrxShadowReg[NA_TxOutputPower1_O]);
																	}
																	break;

									case PHY_ARQ:					phyPIB.rxOn = FALSE;
																	NTRXSPIWriteByte (NA_RxCmdStop_O, ntrxShadowReg[NA_RxCmdStop_O] | (1 << NA_RxCmdStop_B));
																	if( msg->value == 0 )
																	{
																		ntrxShadowReg[NA_TxArq_O] &= ~(1 << NA_TxArq_B);
																		ntrxShadowReg[NA_RxArqMode_O] &= ~((0x03 << NA_RxArqMode_LSB)
																										| (1 << NA_RxCrc2Mode_B));
																	}
																	else
																	{
																		ntrxShadowReg[NA_TxArq_O] |= (1 << NA_TxArq_B);
																		ntrxShadowReg[NA_RxArqMode_O] |= ((NA_RxArqModeCrc2_VC_C << NA_RxArqMode_LSB)
																										| (NA_RxCrc2ModeTrigOn_BC_C << NA_RxCrc2Mode_B));
																	}
																	NTRXSPIWriteByte (NA_TxArq_O, ntrxShadowReg[NA_TxArq_O]);
																	NTRXSPIWriteByte (NA_RxArqMode_O, ntrxShadowReg[NA_RxArqMode_O]);
																	if (phyPIB.rxState == PHY_RX_ON)
																	{
																		phyPIB.rxOn = TRUE;
																		NTRXSPIWriteByte (NA_RxCmdStart_O, ntrxShadowReg[NA_RxCmdStart_O]
																												| (1 << NA_RxCmdStart_B)
																												| (0x03 << NA_RxBufferCmd_LSB));
																	}
																	break;

									case PHY_ARQ_MAX:				if (msg->value > 14)
																	{
																		msg->status = PHY_INVALID_PARAMETER;
																	}
																	else
																	{
																		NTRXSPIWriteByte (NA_RxCmdStop_O, ntrxShadowReg[NA_RxCmdStop_O] | (1 << NA_RxCmdStop_B));
																		phyPIB.arqMax = (MyByte8T)(msg->value);
																		if (phyPIB.arqMax == 0)
																		{
																			ntrxShadowReg[NA_TxArqMax_O] &= ~(0x0f << NA_TxArqMax_LSB);
																		}
																		else
																		{
																			ntrxShadowReg[NA_TxArqMax_O] &= ~(0x0f << NA_TxArqMax_LSB);
																			ntrxShadowReg[NA_TxArqMax_O] |= ((MyByte8T)(msg->value << NA_TxArqMax_LSB));
																		}
																		NTRXSPIWriteByte (NA_TxArqMax_O, ntrxShadowReg[NA_TxArqMax_O]);
																		if (phyPIB.rxState == PHY_RX_ON)
																		{
																			NTRXSPIWriteByte (NA_RxCmdStart_O, ntrxShadowReg[NA_RxCmdStart_O]
																													| (1 << NA_RxCmdStart_B)
																													| (0x03 << NA_RxBufferCmd_LSB));
																		}
																	}
																	break;

									case PHY_FEC:					NTRXSPIWriteByte (NA_RxCmdStop_O, ntrxShadowReg[NA_RxCmdStop_O] | (1 << NA_RxCmdStop_B));
																	if (msg->value == TRUE)
																	{
																		ntrxShadowReg[NA_UseFec_O] |= (1 << NA_UseFec_B);
																	}
																	else
																	{
																		ntrxShadowReg[NA_UseFec_O] &= ~(1 << NA_UseFec_B);
																	}
																	NTRXSPIWriteByte (NA_UseFec_O, ntrxShadowReg[NA_UseFec_O]);

																	phyPIB.fec = msg->value;
																	modeSet.fec = msg->value;

																	if (phyPIB.rxState == PHY_RX_ON)
																	{
																		NTRXSPIWriteByte (NA_RxCmdStart_O, ntrxShadowReg[NA_RxCmdStart_O]
																											| (1 << NA_RxCmdStart_B)
																											| (0x03 << NA_RxBufferCmd_LSB));
																	}
																	break;

									case PHY_MAC_ADDRESS1:			NTRXSPIWriteByte (NA_RxCmdStop_O, ntrxShadowReg[NA_RxCmdStop_O] | (1 << NA_RxCmdStop_B));
																	memcpy (phyPIB.macAddr0, msg->data, 6);
																	NTRXSetIndexReg (0);
																	NTRXSPIWrite (NA_RamStaAddr0_O, msg->data , 6);
																	if (phyPIB.rxState == PHY_RX_ON)
																	{
																		NTRXSPIWriteByte (NA_RxCmdStart_O, ntrxShadowReg[NA_RxCmdStart_O]
																											| (1 << NA_RxCmdStart_B)
																											| (0x03 << NA_RxBufferCmd_LSB));
																	}
																	break;

									case PHY_MAC_ADDRESS2:			NTRXSPIWriteByte (NA_RxCmdStop_O, ntrxShadowReg[NA_RxCmdStop_O] | (1 << NA_RxCmdStop_B));
																	memcpy (phyPIB.macAddr1, msg->data, 6);
																	NTRXSetIndexReg (0);
																	NTRXSPIWrite (NA_RamStaAddr1_O, msg->data, 6);
																	if (phyPIB.rxState == PHY_RX_ON)
																	{
																		NTRXSPIWriteByte (NA_RxCmdStart_O, ntrxShadowReg[NA_RxCmdStart_O]
																											| (1 << NA_RxCmdStart_B)
																											| (0x03 << NA_RxBufferCmd_LSB));
																	}
																	break;

									case PHY_TX_ADDR_SELECT:		NTRXSPIWriteByte (NA_RxCmdStop_O, ntrxShadowReg[NA_RxCmdStop_O] | (1 << NA_RxCmdStop_B));
																	phyPIB.txAddrSel = (MyByte8T)(msg->value);
																	if (msg->value == 0)
																	{
																		ntrxShadowReg[NA_TxAddrSlct_O] &= ~(1 << NA_TxAddrSlct_B);
																	}
																	else
																	{
																		ntrxShadowReg[NA_TxAddrSlct_O] |= (1 << NA_TxAddrSlct_B);
																	}
																	NTRXSPIWriteByte (NA_TxAddrSlct_O, ntrxShadowReg[NA_TxAddrSlct_O]);
																	if (phyPIB.rxState == PHY_RX_ON)
																	{
																		NTRXSPIWriteByte (NA_RxCmdStart_O, ntrxShadowReg[NA_RxCmdStart_O]
																											| (1 << NA_RxCmdStart_B)
																											| (0x03 << NA_RxBufferCmd_LSB));
																	}
																	break;

									case PHY_ADDR_MATCHING:			NTRXSPIWriteByte (NA_RxCmdStop_O, ntrxShadowReg[NA_RxCmdStop_O] | (1 << NA_RxCmdStop_B));
																	phyPIB.addrMatch = (MyByte8T)(msg->value);
																	if (msg->value == 0)
																	{
																		/* promiscuous mode */
																		ntrxShadowReg[NA_RxAddrMode_O] &= ~(1 << NA_RxAddrMode_B);
																	}
																	else
																	{
																		ntrxShadowReg[NA_RxAddrMode_O] |= (1 << NA_RxAddrMode_B);
																	}
																	NTRXSPIWriteByte (NA_RxAddrMode_O, ntrxShadowReg[NA_RxAddrMode_O]);
																	if (phyPIB.rxState == PHY_RX_ON)
																	{
																		NTRXSPIWriteByte (NA_RxCmdStart_O, ntrxShadowReg[NA_RxCmdStart_O]
																											| (1 << NA_RxCmdStart_B)
																											| (0x03 << NA_RxBufferCmd_LSB));
																	}
																	break;

									case PHY_RECALIBRATION:			/* delay time for recalibration */
																	phyPIB.recalInterval = msg->value * 1000;
																	break;

									case PHY_PWR_DOWN_MODE:			if (msg->value > 1)
																	{
																		msg->status = PHY_INVALID_PARAMETER;
																	}
																	else
																	{
																		trxPollMode = (phyPIB.recalInterval == 0) ? FALSE : TRUE;
																		phyPIB.pwrDownMode = msg->value;
																		NTRXResetSettings ();
																		NTRXPowerdownMode (msg->value, *(MyDword32T*)(msg->data));
																	}
																	break;

									case PHY_FRAME_TYPE:			phyPIB.frameType = ( msg->value & 0x07 );
																	ntrxShadowReg[NA_RxDataEn_O] = ~( 0x07 );
																	ntrxShadowReg[NA_RxDataEn_O] |= phyPIB.frameType;
																	NTRXSPIWriteByte (NA_RxDataEn_O , ntrxShadowReg[NA_RxDataEn_O]);
																	break;

									case PHY_TESTMODE:				if (msg->value > 2)
																	{
																		msg->status = PHY_INVALID_PARAMETER;
																	}
																	else
																	{
																		switch (msg->value)
																		{
																			case 1:	TestmodeOff ();
																					NTRXSetTestChirpMode (TRUE);
																					/* this blocks the transmission path of the PDSap */
																					txSendMsg = &upMsg;
																					break;

																			 case 2: TestmodeOff ();
																					NTRXSetTestCarrierMode (TRUE);
																					/* this blocks the transmission path of the PDSap */
																					txSendMsg = &upMsg;
																					break;

																			default:/* this enables the transmission path of the PDSap */
																					TestmodeOff ();
																					txSendMsg = NULL;
																		}

																		phyPIB.testmode = msg->value;

																	}
																	break;

									case PHY_RX_CMD:
																	switch (msg->value)
																	{
																		case PHY_TRX_OFF:
																		case PHY_RX_ON:
																							phyPIB.rxState = msg->value;
																							/*stop the receiver and clear the buffers */
																							NTRXSPIWriteByte (NA_RxCmdStop_O, ntrxShadowReg[NA_RxCmdStop_O]
																															| (1 << NA_RxCmdStop_B)
																															| (0x03 << NA_RxBufferCmd_LSB));
																							/* clear interrupts */
																							NTRXSPIWriteByte (NA_RxIntsReset_O, 0x7F);
																							rxIrq = 0;
#																							ifdef CONFIG_NTRX_SNIFFER
																							upMsg.count=0;
#																							endif
																							if (msg->value == PHY_RX_ON)
																							{
																								/*start the receiver*/
																								NTRXSPIWriteByte (NA_RxCmdStart_O, ntrxShadowReg[NA_RxCmdStart_O]
																																| (1 << NA_RxCmdStart_B));
																							}
																							break;

																		default: 			msg->status = PHY_INVALID_PARAMETER;
																							break;
																	}
																	break;

									case PHY_RX_STATE:				switch (msg->value)
																	{
																		case PHY_RX_ON:
																		case PHY_TRX_OFF: 	phyPIB.rxState = msg->value;
																							break;

																		default: 			msg->status = PHY_INVALID_PARAMETER;
																							break;
																	}
																	break;

									case PHY_SYNCWORD:				NTRXSPIWriteByte (NA_RxCmdStop_O, ntrxShadowReg[NA_RxCmdStop_O] | (1 << NA_RxCmdStop_B));
																	memcpy (phyPIB.syncword, (MyByte8T *)(msg->value), 8);
																	NTRXSetSyncWord(phyPIB.syncword);
																	if (phyPIB.rxState == PHY_RX_ON)
																	{
																		NTRXSPIWriteByte (NA_RxCmdStart_O, ntrxShadowReg[NA_RxCmdStart_O]
																											| (1 << NA_RxCmdStart_B)
																											| (0x03 << NA_RxBufferCmd_LSB));
																	}
																	break;

									default: 						msg->status = PHY_UNSUPPORTED_ATTRIBUTE;
																	break;
								}
								SendCfgUp (msg);
								break;
		default:				break;
	}
	/* ieee config stuff */
}

/**
 * @brief Callback function for received messages
 *
 * This function reads out the payload of a received message and
 * calls the upper layer/application
 *
 */
/****************************************************************************/
void PDCallback (void)
/****************************************************************************/
{
    MyByte8T       status;
    MyByte8T       reg[2];

	/*
	 * use 1 led to indicate message reception on the devBoard
	 * The led will stay on for a 50 ms.
     */
#   ifdef CONFIG_TRAFFIC_LED
    TRIGGER_LED_RX ();
#   endif /* CONFIG_TRAFFIC_LED */

	/*
	 * read the crc2 status register
	 */
    NTRXSPIReadByte (NA_RxCrc2Stat_O, &status);
    rxIrq = 0;

    /* check if data is valid */
    if ((status & (1 << NA_RxCrc2Stat_B)) != 0)
    {
        NTRXSetIndexReg (0);
        /* read source address */
        NTRXSPIRead (NA_RamRxSrcAddr_O, upMsg.addr, 6);
        /* read length plus additionl bits */
        NTRXSPIRead (NA_RamRxLength_O, reg, 2);

        rState = reg[1]>>5;

        /* read destination address */
#		ifdef CONFIG_NTRX_SNIFFER
        NTRXSPIRead (NA_RamRxDstAddr_O, upMsg.rxAddr, 6);
		upMsg.count++;
		upMsg.extBits = rState;
		upMsg.frameType = (status & 0x0f);
#		endif

		upMsg.len = reg[0];
        if (upMsg.len > PHY_PACKET_SIZE)
        {
		    /* restart receiver */
			if (phyPIB.rxState == PHY_RX_ON)
			{
    			NTRXSPIWriteByte (NA_RxCmdStart_O, ntrxShadowReg[NA_RxCmdStart_O]
									| (1 << NA_RxCmdStart_B)
									| (0x03 << NA_RxBufferCmd_LSB));
			}
        }
        else
        {

			if (buffSwapped == TRUE)
			{
				buffSwapped = FALSE;
				/* SWAP BUFFER for receive*/
				ntrxShadowReg[NA_SwapBbBuffers_O] &= ~(1 << NA_SwapBbBuffers_B);
				NTRXSPIWriteByte (NA_SwapBbBuffers_O, ntrxShadowReg[NA_SwapBbBuffers_O]);

				NTRXSetIndexReg (3);
			}
			else
			{
				buffSwapped = TRUE;
				/* SWAP BUFFER for receive*/
				ntrxShadowReg[NA_SwapBbBuffers_O] |= (1 << NA_SwapBbBuffers_B);
				NTRXSPIWriteByte (NA_SwapBbBuffers_O, ntrxShadowReg[NA_SwapBbBuffers_O]);

				NTRXSetIndexReg (2);
			}

			tiPhyRxTimeout_once = FALSE;
			tiPhyRxTimeout = hwclock() + phyPIB.phyRxTimeout;

			/*
			 * restart receiver and than read rx buffer. This is ok because we use
			 * buffer swapping.
			 */
			if (phyPIB.rxState == PHY_RX_ON)
			{
    			NTRXSPIWriteByte (NA_RxCmdStart_O, ntrxShadowReg[NA_RxCmdStart_O]
									| (1 << NA_RxCmdStart_B)
									| (0x03 << NA_RxBufferCmd_LSB));
			}

            NTRXSPIRead ((MyByte8T)(NA_RamRxBuffer_O & 0xFF), upMsg.data, upMsg.len);
			NTRXSetIndexReg (0);
#ifdef		CONFIG_NTRX_SNIFFER
			upMsg.value = 0xff;
			upMsg.prim = PD_DATA_INDICATION;
			SendMsgUp (&upMsg);
#else
			/*
			 * if address matching off, ignore rangingstates
			 * this path is used for normal data reception
			 * ranging is handled in the else path
			 */
			if (((ntrxShadowReg[NA_RxAddrMode_O] & (1<<NA_RxAddrMode_B)) == 0) ||
				(lState == RANGING_READY && rState == RANGING_READY))
			{
				upMsg.value = 0xff;
				upMsg.prim = PD_DATA_INDICATION;
				SendMsgUp (&upMsg);
			}
			else if((lState == RANGING_READY) && (rState == RANGING_START || rState == RANGING_FAST_START))
			{
				memcpy(rDest, upMsg.addr, 6);

				RangingCallback_Rx(upMsg.data, upMsg.len);

				if(rState == RANGING_START)
				{
					lState = RANGING_ANSWER1;
					/* send ranging packet */
					RangingMode(RANGING_ANSWER1, rDest);
				}else if (rState == RANGING_FAST_START)
				{
					lState = RANGING_FAST_ANSWER1;
					/* send ranging packet */
					RangingMode(RANGING_FAST_ANSWER1, rDest);
				}
			}
			else if((memcmp(rDest, upMsg.addr, 6) == 0) && lState == RANGING_ANSWER1 && rState == RANGING_ANSWER1)
			{
				/* received ranging data to RangingCallback_Rx
				 * (without protocol header stuff)
				 */
				RangingCallback_Rx(upMsg.data, upMsg.len);
				lState = RANGING_ANSWER2;
			}
			else if((memcmp(rDest, upMsg.addr, 6) == 0) &&
					((lState == RANGING_ANSWER2 && rState == RANGING_ANSWER2) ||
					(lState == RANGING_FAST_ANSWER1 && rState == RANGING_FAST_ANSWER1)))
			{
				/* ranging was successfull, stop timeout */
				NTRXStopBbTimer();

				/* received ranging data to RangingCallback_Rx
				 *(without protocol header stuff)
				 */
				RangingCallback_Rx(upMsg.data, upMsg.len);

				/* calculate the distance */
				rangingPIB.distance = getDistance();
				rangingPIB.rssi = getRSSI();

				if (rangingPIB.distance < 0.0)
					rangingPIB.error = STAT_RANGING_VALUE_ERROR;

				upMsg.value = 0xff;
				memcpy(upMsg.data, (MyByte8T*) &rangingPIB, sizeof(RangingPIB));
				upMsg.len  = sizeof(RangingPIB);

				if (rState == RANGING_ANSWER2)
					upMsg.prim = PD_RANGING_INDICATION;
				else if (rState == RANGING_FAST_ANSWER1)
					upMsg.prim = PD_RANGING_FAST_INDICATION;

				/* set ready for new ranging */
				lState = RANGING_READY;
				SendMsgUp (&upMsg);
			}
#			endif
        }
    }
}

/**
 * @brief polls the nanoLOC chip for incomming messages
 *
 * This function is the main part of the the physical layer. It processes
 * all interrupt flags and supervises the periodic recalibration.
 *
 */
/***************************************************************************/
void PHYPoll (void)
/***************************************************************************/
{
    MyByte8T arqCount;
	MyMsgT *tMsg;

	if (tiPhyRxTimeout < hwclock() && tiPhyRxTimeout_once == FALSE)
	{
		tiPhyRxTimeout_once = TRUE;

		upMsg.prim = PD_STATUS_INDICATION;
		upMsg.attribute = PHY_RX_TIMEOUT;
		SendMsgUp (&upMsg);
	}

    if (trxPollMode == TRUE)
    {
        NTRXInterrupt ();
    }

    UpdateLEDs ();

    if (ntrxState == TxWAIT)
    {
		/*
		 * get number of transmissions needed to last message
		 * This information is used to determine if the last
		 * transmission was successfull or failed.
		 */
		NTRXSPIReadByte (NA_TxArqCnt_O, &arqCount);

		if ((arqCount > phyPIB.arqMax) && (ntrxState == TxWAIT))
		{
#	   		ifdef CONFIG_NTRX_AUTO_RECALIB
			if(( ++rcwd > 3 ) && ( phyPIB.recalInterval != 0 ))
			{
				/* INFO: If the TRX sends a packet, calibration failes!
				 * In this case rcwd is not reset, but tiRecal is.
				 */
				/* normal operation mode */
				if (phyPIB.testmode == 0)
				{
					if (NTRXAllCalibration ())
					{
						tiRecal = hwclock() + phyPIB.recalInterval;
						rcwd = 0;
						TRIGGER_LED_CAL();
					}
				}
				else
				{
					/*
 					 * in case of an enabled testmode recalibration is a bit trickier.
					 * We first have to disable the testmode, recalibrate and then
 					 * enable the testmode again.
					 */

					if (phyPIB.testmode == 1)
					{
						NTRXSetTestChirpMode (FALSE);
						NTRXAllCalibration ();
						TRIGGER_LED_CAL();
						NTRXSetTestChirpMode (TRUE);
					}
					else
					{
						NTRXSetTestCarrierMode	(FALSE);
						NTRXAllCalibration ();
						TRIGGER_LED_CAL();
						NTRXSetTestCarrierMode	(TRUE);
					}
				}
			}
#      		endif

			TRIGGER_LED_ERR();
			txSendMsg->status = PHY_NO_ACK;
		}
		else
		{
			txSendMsg->status = PHY_SUCCESS;
		}

		txIrq &= ~(0x01 << NA_TxEnd_B);
		ntrxState = TxIDLE;
		txSendMsg->value = arqCount;

		switch (txSendMsg->prim)
		{
			case PD_RANGING_REQUEST_EXECUTE:
				txSendMsg->prim = PD_RANGING_CONFIRM;

				bbState = BB_STATE_NORMAL_PHASE1;

				if (txSendMsg->status == PHY_SUCCESS)
				{
					RangingCallback_Ack(arqCount);
					lState = RANGING_ANSWER1;
					NTRXStartBbTimer(modeSet.rangingTimeout);
				}
				else
				{
					rangingPIB.error |= STAT_NO_REMOTE_STATION;
					lState = RANGING_READY;
				}

				memcpy(txSendMsg->data, (MyByte8T*) &rangingPIB, sizeof(RangingPIB));
				txSendMsg->len = sizeof(RangingPIB);

				tMsg = txSendMsg;
				txSendMsg = NULL;
				SendMsgUp (tMsg);
				break;
			case PD_RANGING_FAST_REQUEST_EXECUTE:
				txSendMsg->prim = PD_RANGING_FAST_CONFIRM;

				bbState = BB_STATE_FAST_PHASE1;

				if (txSendMsg->status == PHY_SUCCESS)
				{
					RangingCallback_Ack(arqCount);
					lState = RANGING_FAST_ANSWER1;
					NTRXStartBbTimer(modeSet.rangingFastTimeout);
				}
				else
				{
					rangingPIB.error |= STAT_NO_REMOTE_STATION;
					lState = RANGING_READY;
				}

				memcpy(txSendMsg->data, (MyByte8T*) &rangingPIB, sizeof(RangingPIB));
				txSendMsg->len = sizeof(RangingPIB);

				tMsg = txSendMsg;
				txSendMsg = NULL;
				SendMsgUp (tMsg);
				break;
			case PD_RANGING_ANSWER1_EXECUTE:

				if(txSendMsg->status == PHY_SUCCESS)
				{
					RangingCallback_Ack(arqCount);

					txSendMsg = NULL;

					lState = RANGING_ANSWER2;

					/* send ranging packet with TxEnd information
					 * from RangingMode before
					 */
					RangingMode(RANGING_ANSWER2, rDest);
				}
				else
				{
					lState = RANGING_READY;
					txSendMsg = NULL;
				}

				break;

			case PD_RANGING_FAST_ANSWER1_EXECUTE:
			case PD_RANGING_ANSWER2_EXECUTE:
				lState = RANGING_READY;

				txSendMsg = NULL;
				break;

			default:
				txSendMsg->prim = PD_DATA_CONFIRM;

				tMsg = txSendMsg;
				txSendMsg = NULL;
				SendMsgUp (tMsg);
				break;
		}

    }

    /* check if nanoNET TRX chip has received valid data */
    if (ntrxState == TxIDLE)
    {
        if ((rxIrq & (0x01 << NA_RxEnd_B)) != 0 )
        {
            PDCallback ();
        }
    }

#   ifdef CONFIG_NTRX_AUTO_RECALIB
    if (phyPIB.recalInterval != 0)
    {
        if ((rcwd > 3) || (tiRecal < hwclock()))
        {
			/* INFO: If the TRX sends a packet, calibration failes!
			 * In this case rcwd is not reset, but tiRecal is. */
			if (phyPIB.testmode == 0)
			{
				if (NTRXAllCalibration ())
				{
		            tiRecal = hwclock() + phyPIB.recalInterval;
					rcwd = 0;
					TRIGGER_LED_CAL();
				}
			}
			else
			{
				/*
					* in case of an enabled testmode recalibration is a bit trickier.
					* We first have to disable the testmode, recalibrate and then
					* enable the testmode again.
					*/
				if (phyPIB.testmode == 1)
				{
					NTRXSetTestChirpMode (FALSE);
					NTRXAllCalibration ();
					TRIGGER_LED_CAL();
					NTRXSetTestChirpMode (TRUE);
				}
				else
				{
					NTRXSetTestCarrierMode	(FALSE);
					NTRXAllCalibration ();
					TRIGGER_LED_CAL();
					NTRXSetTestCarrierMode	(TRUE);
				}
			}
        }
    }
#   endif /* CONFIG_NTRX_AUTO_RECALIB */
}

/**
 * @brief interrupt service routine for the nanoLOC chip
 *
 * This function is an interrupt service routine of the nanochip.
 * It updates the TX and RX status flags.
 *
 */
/**************************************************************************/
void NTRXInterrupt (void)
/**************************************************************************/
{
	/* we have received an interrupt and neede to find out what caused it */
	NTRXSPIRead (NA_TxIntsRawStat_O, ntrxIrqStatus, 2);
	/* check if it was the transmitter */
	if (txIrqStatus != 0)
	{
		/* clear interrupts */
		NTRXSPIWriteByte (NA_TxIntsReset_O, txIrqStatus);
		txIrq |= txIrqStatus & (0x01 << NA_TxEnd_B);
		if ((txIrq & (0x01 << NA_TxEnd_B)) != 0)
		{
			ntrxState = TxWAIT;
		}
	}
	/* check if it was the receiver */
	if (rxIrqStatus != 0)
	{
		/* clear interrupts */
		NTRXSPIWriteByte (NA_RxIntsReset_O, rxIrqStatus);
		rxIrq |= rxIrqStatus & (0x01 << NA_RxEnd_B);
	}

	//BASEBAND
	NTRXSPIRead(NA_BbTimerIrqStatus_O,&bbtimerStatus,1);
	/* check if it was the basebandtimer */
	if ((bbtimerStatus & 0x40) != 0)
	{
		NTRXStopBbTimer();

		switch(bbState)
		{
			case BB_STATE_NORMAL_PHASE1:
				bbState = BB_STATE_NORMAL_PHASE2;
				NTRXStartBbTimer(modeSet.rangingTimeout);
				break;
			case BB_STATE_FAST_PHASE1:
				bbState = BB_STATE_FAST_PHASE2;
				NTRXStartBbTimer(modeSet.rangingFastTimeout);
				break;
			default:
				if(lState == RANGING_ANSWER1)
				{
					rangingPIB.error |= STAT_NO_ANSWER1;
					rangingPIB.error |= STAT_NO_ANSWER2;
				}else if(lState == RANGING_ANSWER2)
				{
					rangingPIB.error |= STAT_NO_ANSWER2;
				}

				lState = RANGING_DONE;

				upMsg.value = 0xff;
				memcpy(upMsg.data, (MyByte8T*) &rangingPIB, sizeof(RangingPIB));
				upMsg.len  = sizeof(RangingPIB);
				upMsg.prim = PD_RANGING_INDICATION;
				SendMsgUp (&upMsg);

				/* set ready for new ranging */
				lState = RANGING_READY;

				bbIrq = 1;
				bbtimerStatus = 0;

				break;
		}
	}
}
