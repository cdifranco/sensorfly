/* $Id$ */
/**
 * @file ntrxranging.h
 * @date 2007-Dez-11
 * @author C. Bock
 * @c (C) 2007 Nanotron Technologies
 * @brief Ranging support functions.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the definitions of the ranging support
 *    and calculation of a ranging cycle with the nanoLOC chip.
 *
 */

/*
 * $Log$
 */

#ifndef NTRXRANGING_H
#define NTRXRANGING_H

#include "ntrxtypes.h"

/** @brief Structure for the ranging result.
 *
 * When a ranging cycle is finished a ranging result message will
 * be send to the upper layer. The message will have the following
 * format.
 */
typedef struct
{
	MyByte8T    error;		/**< status value of the ranging cycle */
	MyByte8T	 rssi;		/**< agcGain value */
	MyDouble32T distance;	/**< measurement result	*/
} RangingPIB;

/**
 * getDistance(..)
 *
 * If one ranging cycle is complete, getDistance returns the distance
 * in [m] to the destination which was chosen in RangingMode chosen.
 *
 * Return: distance in [m] to RangingMode destination address
 */
double getDistance(void);

/**
 * getRSSI(..)
 *
 * If one ranging cycle is complete, getRSSI returns the RSSI-Value.
 *
 * Return: rssi
 */
MyByte8T getRSSI(void);

/**
 * RangingCallback_Rx(..):
 *
 * Received ranging packets must be given to this function.
 * The function collects the ranging answer of a RangingMode packet.
 *
 * MyByte8T *payload : payload of ranging packet
 * MyInt16T val : length of RangingPacket
 *
 * Returns: nothing
 */
void RangingCallback_Rx(MyByte8T *payload, MyByte8T len);

/**
 * RangingCallback_Ack(..):
 *
 * After each RangingPacket (RangingMode) has been sent, we need to collect
 * data. The function must be called after the ranging transmission.
 *
 * MyByte8T arqCount : how many retransmissions (successfully sent or not?)
 *
 * Returns: nothing
 */
void RangingCallback_Ack(MyByte8T arqCount);

/**
 * RangingMode(..):
 *
 * The function sends out a RangingPacket to the given destination.
 *
 * MyAddrT dest : 6 Byte Address; when dest is changed , all collected data will
 *			be deleted
 *
 * Returns: nothing
 */
void RangingMode(MyByte8T cmd, MyAddrT dest);



#endif /* NTRXRANGING_H */
