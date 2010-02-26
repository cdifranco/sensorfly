/* $Id$ */
/**
 * @file hwclock.h
 * @date 2007-Dez-11
 * @author S. Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief Timer support for AVR.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the function definitions for the utility functions
 *    of the AVR hardware timer.
 */
/*
 * $Log$
 */

#ifndef	_HWCLOCK_H
#define	_HWCLOCK_H

#include "config.h"
#include "ntrxtypes.h"

extern	void		hwclock_init(void);
extern  void 		hwclock_stop (void);
extern  void 		StartTimer2 (void);
extern	MyDword32T	hwclock(void);
extern	void		hwdelay(MyDword32T t);
extern  void 		HWDelayms( MyWord16T ms );
extern  void 		HWDelayus( MyWord16T us );

void	hwclockRestart(MyDword32T start);
void 	SetWdtCounter (MyDword32T value);

#define ENTER_TASK {unsigned char cSREG=SREG; cli();
#define LEAVE_TASK  SREG=cSREG; sei();}
#ifdef CONFIG_IMPULSE_COUNTER
MyDword32T GetImpuleCounter (MyByte8T i);
MyDword32T GetAndResetImpuleCounter (MyByte8T i);
#endif


#ifdef CONFIG_OPTO_COUNTER
MyDword32T GetOptoCounter (void);
MyDword32T GetAndResetOptoCounter (void);
#endif
#endif	/* _HWCLOCK_H */
