/* $Id$ */
/**
 * @file config.h
 * @date 2007-Dez-11
 * @author S.Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief Project global definitions.
 *
 * @todo Making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains  the global project defines for the standalone atmega
 *    ntrx driver.
 */

/*
 * $Log$
 */

#ifndef _CONFIG_H
#define _CONFIG_H

/* use the console interface for communication with the application */
#define CONFIG_CONSOLE 1

/* allow some printf output during program execution */
#define CONFIG_PRINTF 1

/* size in char of the USART's input queue */
#define CONFIG_CONSOLE_QUEUE_SIZE (16)

/* size in char of the maximal printf line */
#define CONFIG_PRINTF_LINE_SIZE (80)

/* size in char of the maximal console line */
#define CONFIG_CONSOLE_LINE_SIZE (80)

/* select the time between to nanoNET TRX chip recalibrations in ms */
#define CONFIG_NTRX_RECAL_DELAY (3000)

/* select the time between last packet and timeout indication in ms */
#define CONFIG_NTRX_PHY_RX_TIMEOUT (5000)

/* version.revision number of the firmware */
#define CONFIG_REG_MAP_NR (501)
/* use only necessary shadow registers */
// #define CONFIG_NTRX_NO_SREG 1

/* use leds for tx/rx indication */
#define CONFIG_TRAFFIC_LED 1

/* use led for recalibration indication */
// #define CONFIG_RECALIB_LED 1
#define CONFIG_USE_TX_LED 1
#define CONFIG_USE_RX_LED 1
// #define CONFIG_USE_ERR_LED 1
#define CONFIG_USE_CAL_LED 1

/* old board */
#define CONFIG_PORTATION_ATMEGA128_BOARD 1
// #define CONFIG_AVR_BOARD_10 1
#define CONFIG_AVR_BOARD_11 1

/* set clock for uart */
#define F_CPU 7372800UL
#define CONFIG_CLK_73_MHZ 1
// #define FCPU 8000000
// #define CONFIG_CLK_80_MHZ 1

/* use auto recalibration */
#define CONFIG_NTRX_AUTO_RECALIB 1

/* use when external power amplifier is present */
#define CONFIG_NTRX_RF_TX_EXT_PAMP_OUT 1

/* use syncword */
#define CONFIG_DEFAULT_SYNCWORD {0xab,0x2c,0xd5,0x92,0x94,0xca,0x69,0xab}

/* All available trx modes */
#define CONFIG_NTRX_22MHZ_1000NS  1
#define CONFIG_NTRX_22MHZ_2000NS  1
#define CONFIG_NTRX_22MHZ_4000NS  1
#define CONFIG_NTRX_80MHZ_500NS   1
#define CONFIG_NTRX_80MHZ_1000NS  1
#define CONFIG_NTRX_80MHZ_2000NS  1
#define CONFIG_NTRX_80MHZ_4000NS  1
// #define CONFIG_DEFAULT_TRX_22MHZ_1000NS 1
// #define CONFIG_DEFAULT_TRX_22MHZ_2000NS 1
// #define CONFIG_DEFAULT_TRX_22MHZ_4000NS 1
// #define CONFIG_DEFAULT_TRX_80MHZ_500NS  1
#define CONFIG_DEFAULT_TRX_80MHZ_1000NS 1
// #define CONFIG_DEFAULT_TRX_80MHZ_2000NS 1
// #define CONFIG_DEFAULT_TRX_80MHZ_4000NS 1
#if 	  (!defined (CONFIG_DEFAULT_TRX_22MHZ_1000NS) \
		&& !defined (CONFIG_DEFAULT_TRX_22MHZ_2000NS) \
		&& !defined (CONFIG_DEFAULT_TRX_22MHZ_4000NS) \
		&& !defined (CONFIG_DEFAULT_TRX_80MHZ_500NS) \
		&& !defined (CONFIG_DEFAULT_TRX_80MHZ_1000NS) \
		&& !defined (CONFIG_DEFAULT_TRX_80MHZ_2000NS) \
		&& !defined (CONFIG_DEFAULT_TRX_80MHZ_4000NS))
#ifdef CONFIG_NTRX_80MHZ_1000NS
#define CONFIG_DEFAULT_TRX_80MHZ_1000NS 1
#warning No default mode set. Set to 80Mhz 1us!
#else
#error Please select a default mode in config.h
#endif
#endif

#define     CONFIG_80MHz_1us_1M_S 1
#define    CONFIG_FIXmode 1

#define CONFIG_MAX_ARQ 		14
#define CONFIG_TX_PWR 		0x3f
#define CONFIG_MAX_LOG_CHANNEL	4
#define CONFIG_PAYLOAD_LEN  128
#define NKEYS 1
#endif /* _CONFIG_H */
