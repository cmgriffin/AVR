/*! \file uartsw.h \brief Software Interrupt-driven UART Driver. */
//*****************************************************************************
//
// File Name	: 'uartsw.h'
// Title		: Software Interrupt-driven UART Driver
// Author		: Pascal Stang - Copyright (C) 2002-2004
// Created		: 7/20/2002
// Revised		: 4/27/2004
// Version		: 0.1
// Target MCU	: Atmel AVR Series (intended for the ATmega16 and ATmega32)
// Editor Tabs	: 4
//
///	\ingroup driver_sw
/// \defgroup uartsw Software Interrupt-driven UART Driver (uartsw.c)
/// \code #include "uartsw.h" \endcode
/// \par Overview
///		This uart library emulates the operation of a UART (serial port)
/// using 	the AVR's hardware timers, I/O pins, and some software.
///
///	Specifically, this code uses:
///		-Timer 1 Output Compare A for transmit timing
///		-Timer 1 Output Compare B for receive timing
///		-Timer 1 Input Capture for receive triggering
///
///	The above resources cannot be used for other purposes while this
/// software 	UART is enabled.  The overflow interrupt from Timer1 can still
/// be used for 	other timing, but the prescaler for Timer1 must not be
/// changed.
///
///	Serial output from this UART can be routed to any I/O pin.  Serial input
///	for this UART must come from the Timer1 Input Capture (IC1) I/O pin.
///	These options should be configured by editing your local copy of
///	"uartswconf.h".
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
#ifndef __UARTSW___
#define __UARTSW__

//*****************************************************************************
#include "global.h"
#include <avrlibdefs.h>
// constants/macros/typdefs

// functions

//! enable and initialize the software uart
void UARTSW_init(void);
//! turns off software UART
void UARTSW_off(void);

//! sends a single byte over the uart
bool UARTSW_TransmitByte(uint8_t c, bool blocking);

//! gets a single byte from the uart receive buffer
// Function returns TRUE if data was available, FALSE if not.
// Actual data is returned in variable pointed to by "data".
// example usage:
// char myReceivedByte;
// UARTSW_ReceiveByte( &myReceivedByte );
bool UARTSW_ReceiveByte(uint8_t *c, bool blocking);

#endif // __UARTSW__