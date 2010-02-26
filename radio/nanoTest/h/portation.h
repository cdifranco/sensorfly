/* $Id$ */

/*****************************************************************************
 *
 * Copyright 2002
 * Nanotron Technologies
 *
 * Author: S. Rohdemann
 *
 * BuildNumber = "BuildNumber : 2267";
 *
 * Description :
 *    This file contains the type- / data- and function definitions
 *    for the skeleton portation
 *
 ****************************************************************************/

/*
 * $Log$
 */

#ifndef	_PORTATION_H
#define	_PORTATION_H

#include	"iohigh.h"
#include    "iolow.h"
#include	"hwclock.h"
#include	"nnirq.h"
#include	"nnreset.h"
#include	"nnspi.h"
#include    "board.h"

#ifndef	CONFIG_APP_NONE
extern	void	InitApplication(void);
extern	void	PollApplication(void);
#endif	/* CONFIG_APP_NONE */

#ifdef	CONFIG_OS_CROSS
extern	MyWord16T	hw_stack_usage(void);
#endif	/* CONFIG_OS_CROSS */

#endif	/* _PORTATION_H */
