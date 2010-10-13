/* $Id$ */
/**
 * @file app.h
 * @date 2007-Dez-11
 * @author S.Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief Application layer of this demo..
 *
 * @todo Making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the source code for the demo application.
 */

/*
 * $Log$
 */

#ifndef APP_H
#define APP_H

#include "ntrxtypes.h"

#define RTS_CTS_ENABLE 1

/** @brief Structure type for all layer configuration settings
 *
 * To be IEEE layer complient, all configuration data is stored
 * in one data structure.
 */

typedef struct AplMemT
{
	MyDword32T hwclock; 	/**< Timestamp to determine a timeout condition for a data request */
	MyAddrT    dest;		/**< MAC address of the peer device */
	MyAddrT    src;		/**< MAC address of this device */
	MyByte8T 	len;		/**< Number of bytes in send buffer */
} AplMemT;

void APLMECallback (MyMsgT *msg);
void APLCallback (MyMsgT *msg);
void APLInit (void);
void APLPoll (void);

void SetAVR(Packet *setPkt);
void SetSrcAddr (MyAddrT *src);
void SetDestAddr (MyAddrT *dest);
int SetChannel(MyByte8T channel);
int SetPower(MyByte8T power);
int SetAutoAck(MyByte8T autoAck);
void SetStartComm(void);
void SetSendRangeReq(void);
void SetSendFastRangeReq(void);


#endif /* APPL_H */