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

#define Sf_packet_header_overhead 4

typedef struct {
    char header_soh;
    char size; 
    char type;
    char  tag;
    char params[256 - Sf_packet_header_overhead]; //max size of a packet is 256 because the size field is one 8-bit byte long.
} Sf_packet;

typedef enum {no_packet, waiting_for_rest_of_packet, receiving_packet, packet_received, malformed_packet, packet_timed_out } Read_sf_state;

typedef struct {
    Read_sf_state state;
    int nbytes;
}Read_sf_status;

Read_sf_status    readSfPacket(Sf_packet * outputArea);

typedef unsigned char crc;
#define CRC_WIDTH (8 * sizeof(crc))
#define CRC_TOPBIT (1 << (CRC_WIDTH - 1))
#define CRC_POLYNOMIAL 0xD8  /* 11011 followed by 0's */
crc crcFast (unsigned char message [], int message_len, crc previous_partial_cksum); /* From netrino.com */

#endif	/* CONFIG_CONSOLE */

#endif	/* _IOHIGH_H */
