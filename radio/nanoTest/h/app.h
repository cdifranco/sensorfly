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

/** @brief Structure type for all layer configuration settings
 *
 * To be IEEE layer complient, all configuration data is stored
 * in one data structure.
 */

typedef struct
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

#endif /* APPL_H */
