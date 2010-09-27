/* $Id$ */
/**
 * @file usart.h
 * @date 2007-Dez-11
 * @author S. Rohdemann, S. Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief Console support functions.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the prototypes for the console
 *       and serial port support functions.
 */

/*
 * $Log$
 */

#ifndef	_IOHIGH_H
#define	_IOHIGH_H

#include "config.h"
#ifdef	CONFIG_CONSOLE
void console_init(void);
int	kbhit(void);
char    *read_line(char buf[]);
#endif	/* CONFIG_CONSOLE */

#endif	/* _IOHIGH_H */
