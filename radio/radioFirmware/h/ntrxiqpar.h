/* $Id$ */
/**
 * @file ntrxiqpar.h
 * @date 2007-Dez-11
 * @author S. Radtke, O. Tekyar
 * @c (C) 2007 Nanotron Technologies
 * @brief Utility functions for nanoLOC transmit mode setting.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the prototypes for setting the
 *       transmission modes of the nanoLOC chip.
 */

/*
 * $Log$
 */

#ifndef NTRXIQPAR_H
#define NTRXIQPAR_H

#include    "config.h"
#include    "ntrxtypes.h"

void NTRXSetAgcValues (MyByte8T bandwidth, MyByte8T symbolDur, MyByte8T symbolRate);
void NTRXSetTxIqMatrix (MyByte8T bandwidth, MyByte8T symbolDur);
void NTRXSetRxIqMatrix (MyByte8T bandwidth, MyByte8T symbolDur);
void NTRXSetCorrThreshold (MyByte8T bandwidth, MyByte8T symbolDur);

#endif
