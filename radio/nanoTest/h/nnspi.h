/* $Id$ */
/**
 * @file nnspi.h
 * @date 2007-Dez-11
 * @author B. Jozefini, S. Radtke
 * @c (C) 2007 Nanotron Technologies
 * @brief SPI bus access functions.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the prototypes for the SPI bus
 *       control functions of the AVR controller.
 */

/*
 * $Log$
 */

#ifndef  _NNSPI_H
#define  _NNSPI_H

#include "config.h"
#include "ntrxtypes.h"


typedef enum
{
    READ_CMD = 0x00,                          /* SPI read command */
    WRITE_CMD = 0x80                          /* SPI write command */
} CMDT;

#define NANONETRESETDELAY   1000             /* reset of TRX in us */
//#define CONFIG_SPI_TRACE 1

void NanoReset	(void);
void InitSPI	(void);
void SetupSPI 	(void);
void NTRXSPIRead(MyByte8T address, MyByte8T *buffer, MyByte8T len);
void NTRXSPIWrite(MyByte8T address, MyByte8T *buffer, MyByte8T len);
void NTRXSPIReadByte(MyByte8T address, MyByte8T *buffer);
void NTRXSPIWriteByte(MyByte8T address, MyByte8T buffer);
void SpiTraceOn (MyBoolT flag);


#ifdef CONFIG_SPI_TRACE
#define TRACE_DATA_LEN 2
#define TRACE_BUFFER_SIZE 200
typedef struct traceS
{
	unsigned char cmd;
	unsigned char len;
	unsigned char addr;
	unsigned char data[TRACE_DATA_LEN];
	int	id;
} traceT;

void printSpiTrace (int min, int max);
void traceSpiReset (void);
#endif /* CONFIG_SPI_TRACE */

#endif   /* _NNSPI_H */
