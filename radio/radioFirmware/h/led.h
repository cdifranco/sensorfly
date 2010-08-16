/* $Id$ */
/**
 * @file led.h
 * @date 2008-Apr-16
 * @author S.Radtke
 * @c (C) 2008 Nanotron Technologies
 * @brief Macros for LED handling.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the macros for the nanoLOC SModule
 *       to access the LEDs.
 */

/*
 * $Log$
 */
#ifndef	_LED_H
#define	_LED_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include "avrport.h"

#define	LED_OFF		0
#define	LED_ON		1
#define CONFIG_LED_INVERS 1

#ifdef CONFIG_LED_INVERS
#define	LED0(flag);	{ if( flag != LED_ON ) { LED0_PORT |= LED0_PIN; } else { LED0_PORT &= ~LED0_PIN; }}
#define	LED1(flag);	{ if( flag != LED_ON ) { LED1_PORT |= LED1_PIN; } else { LED1_PORT &= ~LED1_PIN; }}
#define	LED2(flag);	{ if( flag != LED_ON ) { LED2_PORT |= LED2_PIN; } else { LED2_PORT &= ~LED2_PIN; }}
#define	LED3(flag);	{ if( flag != LED_ON ) { LED3_PORT |= LED3_PIN; } else { LED3_PORT &= ~LED3_PIN; }}
#define	LED4(flag);	{ if( flag != LED_ON ) { LED4_PORT |= LED4_PIN; } else { LED4_PORT &= ~LED4_PIN; }}
#define	LED5(flag);	{ if( flag != LED_ON ) { LED5_PORT |= LED5_PIN; } else { LED5_PORT &= ~LED5_PIN; }}
#define	LED6(flag);	{ if( flag != LED_ON ) { LED6_PORT |= LED6_PIN; } else { LED6_PORT &= ~LED6_PIN; }}
#define	LED7(flag);	{ if( flag != LED_ON ) { LED7_PORT |= LED7_PIN; } else { LED7_PORT &= ~LED7_PIN; }}

#define	LED_TX(flag);	{ if( flag != LED_ON ) { LED_TX_PORT |= LED_TX_PIN; } else { LED_TX_PORT &= ~LED_TX_PIN; }}
#define	LED_RX(flag);	{ if( flag != LED_ON ) { LED_RX_PORT |= LED_RX_PIN; } else { LED_RX_PORT &= ~LED_RX_PIN; }}
#define	LED_ERR(flag);	{ if( flag != LED_ON ) { LED_ERR_PORT |= LED_ERR_PIN; } else { LED_ERR_PORT &= ~LED_ERR_PIN; }}
#define	LED_CAL(flag);	{ if( flag != LED_ON ) { LED_CAL_PORT |= LED_CAL_PIN; } else { LED_CAL_PORT &= ~LED_CAL_PIN; }}
#define	LED_ALIVE(flag);	{ if( flag != LED_ON ) { LED_ALIVE_PORT |= LED_ALIVE_PIN; } else { LED_ALIVE_PORT &= ~LED_ALIVE_PIN; }}
#else
#define	LED0(flag);	{ if( flag == LED_ON ) { LED0_PORT |= LED0_PIN; } else { LED0_PORT &= ~LED0_PIN; }}
#define	LED1(flag);	{ if( flag == LED_ON ) { LED1_PORT |= LED1_PIN; } else { LED1_PORT &= ~LED1_PIN; }}
#define	LED2(flag);	{ if( flag == LED_ON ) { LED2_PORT |= LED2_PIN; } else { LED2_PORT &= ~LED2_PIN; }}
#define	LED3(flag);	{ if( flag == LED_ON ) { LED3_PORT |= LED3_PIN; } else { LED3_PORT &= ~LED3_PIN; }}
#define	LED4(flag);	{ if( flag == LED_ON ) { LED4_PORT |= LED4_PIN; } else { LED4_PORT &= ~LED4_PIN; }}
#define	LED5(flag);	{ if( flag == LED_ON ) { LED5_PORT |= LED5_PIN; } else { LED5_PORT &= ~LED5_PIN; }}
#define	LED6(flag);	{ if( flag == LED_ON ) { LED6_PORT |= LED6_PIN; } else { LED6_PORT &= ~LED6_PIN; }}
#define	LED7(flag);	{ if( flag == LED_ON ) { LED7_PORT |= LED7_PIN; } else { LED7_PORT &= ~LED7_PIN; }}

#define	LED_TX(flag);	{ if( flag == LED_ON ) { LED_TX_PORT |= LED_TX_PIN; } else { LED_TX_PORT &= ~LED_TX_PIN; }}
#define	LED_RX(flag);	{ if( flag == LED_ON ) { LED_RX_PORT |= LED_RX_PIN; } else { LED_RX_PORT &= ~LED_RX_PIN; }}
#define	LED_ERR(flag);	{ if( flag == LED_ON ) { LED_ERR_PORT |= LED_ERR_PIN; } else { LED_ERR_PORT &= ~LED_ERR_PIN; }}
#define	LED_CAL(flag);	{ if( flag == LED_ON ) { LED_CAL_PORT |= LED_CAL_PIN; } else { LED_CAL_PORT &= ~LED_CAL_PIN; }}
#define	LED_ALIVE(flag);	{ if( flag == LED_ON ) { LED_ALIVE_PORT |= LED_ALIVE_PIN; } else { LED_ALIVE_PORT &= ~LED_ALIVE_PIN; }}
#endif
#endif	/* _LED_H */
