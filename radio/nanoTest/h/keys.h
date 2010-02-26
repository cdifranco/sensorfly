/* $Id$ */
/**
 * @file keys.h
 * @date 2007-Dez-11
 * @author S. Rohdemann, S.Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief Macros for key handling.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the macros for nanoLOC devkit board
 *       for accessing the keys.
 */

/*
 * $Log$
 */
#ifndef	_KEYS_H
#define	_KEYS_H
#include "config.h"
#include "ntrxtypes.h"

extern	MyBoolT	key_flags[];

#define	key_k1()	key_flags[0]
#define	key_k2()	key_flags[1]
#define	key_k3()	key_flags[2]

#endif	/* _KEYS_H */
