/* $Id$ */
/**
 * @file avrport.h
 * @date 2008-Apr-16
 * @author S. Radtke
 * @c (C) 2008 Nanotron Technologies
 * @brief support for AVR.
 *
 * @todo making this file doxygen compatible.
 *
 * @note BuildNumber = "BuildNumber : 2267";
 *
 * @note This file contains the port definitions for the AVR uC
 */
/*
 * $Log$
 */

#ifndef AVRPORT_H
#define AVRPORT_H

#include "config.h"

// ATmega644
#define AVR_TCCR0	TCCR0B
#define AVR_TIMSK	TIMSK0
#define AVR_TCNT0	TCNT0

#define AVR_UDR1	UDR0
#define AVR_UBRR1H	UBRR0H
#define AVR_UBRR1L	UBRR0L
#define AVR_UCSR1C	UCSR0C
#define AVR_UCSR1B	UCSR0B
#define AVR_UCSZ11	UCSZ01
#define AVR_UCSZ10	UCSZ00
#define AVR_RXCIE1	RXCIE0
#define AVR_RXEN1	RXEN0
#define AVR_TXEN1	TXEN0
#define AVR_UCSR1A	UCSR0A
#define AVR_UDRE1	UDRE0
#define AVR_U2X1    U2X0
#define AVR_USART_RECV	SIG_USART_RECV

#define SPI_PORT	PORTB
#define SPI_DIR		DDRB
#define SPI_SS		( 1 << PORTB4 )                         /* SPI Slave select     */
#define SPI_CLK     ( 1 << PORTB7 )                         /* SPI clock            */
#define SPI_MOSI	( 1 << PORTB5 )							/* SPI MOSI				*/
#define SPI_MISO	( 1 << PORTB6 )							/* SPI MISO				*/
#define SPI_INT		INT0

#ifdef CONFIG_WINAVR
#define AVR_SPCR	SPCR0
#define AVR_SPE		SPE0
#define AVR_MSTR	MSTR0
#define AVR_DORD	DORD0
#define AVR_SPSR	SPSR0
#define AVR_SPI2X	SPI2X0
#define AVR_SPDR	SPDR0
#define AVR_SPIF	SPIF0
#else
#define AVR_SPCR	SPCR
#define AVR_SPE		SPE
#define AVR_MSTR	MSTR
#define AVR_DORD	DORD
#define AVR_SPSR	SPSR
#define AVR_SPI2X	SPI2X
#define AVR_SPDR	SPDR
#define AVR_SPIF	SPIF
#endif

#define PONRESET_PORT	PORTD
#define PONRESET_DIR	DDRD
#define PONRESET_PIN	( 1 << PORTD3 )

#define LED0_DIR		DDRA
#define LED0_PORT		PORTA
#define LED0_PIN		( 1 << PORTA7 )

#define LED1_DIR		DDRC
#define LED1_PORT		PORTC
#define LED1_PIN		( 1 << PORTC7 )

#define LED2_DIR		DDRC
#define LED2_PORT		PORTC
#define LED2_PIN		( 1 << PORTC6 )

#define LED3_DIR		DDRC
#define LED3_PORT		PORTC
#define LED3_PIN		( 1 << PORTC1 )

#define LED_TX_DIR		LED0_DIR
#define LED_TX_PORT		LED0_PORT
#define LED_TX_PIN		LED0_PIN

#define LED_RX_DIR		LED1_DIR
#define LED_RX_PORT		LED1_PORT
#define LED_RX_PIN		LED1_PIN

#define LED_ERR_DIR		LED2_DIR
#define LED_ERR_PORT	LED2_PORT
#define LED_ERR_PIN		LED2_PIN

#define LED_CAL_DIR		LED2_DIR
#define LED_CAL_PORT	LED2_PORT
#define LED_CAL_PIN		LED2_PIN

#define LED_ALIVE_DIR	LED3_DIR
#define LED_ALIVE_PORT	LED3_PORT
#define LED_ALIVE_PIN	LED3_PIN

#define SW0_DIR			DDRC
#define SW0_PORT		PORTC
#define SW0_PIN			( 1 << PORTC0 )
#define SW0_IN			( PINC & ( SW0_PIN ))

#endif
