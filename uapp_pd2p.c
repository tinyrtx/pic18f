//*******************************************************************************
// tinyRTX Filename: uapp_pd2p.c (User APPlication for PICdem2+ boards)
//
// Copyright 2014 Sycamore Software, Inc.  ** www.tinyRTX.com **
// Distributed under the terms of the GNU Lesser General Purpose License v3
//
// This file is part of tinyRTX. tinyRTX is free software: you can redistribute
// it and/or modify it under the terms of the GNU Lesser General Public License
// version 3 as published by the Free Software Foundation.
//
// tinyRTX is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// (filename copying.lesser.txt) and the GNU General Public License (filename
// copying.txt) along with tinyRTX.  If not, see <http://www.gnu.org/licenses/>.
//
// Revision history:
//  15Oct03  SHiggins@tinyRTX.com  Created from scratch.
//  31Oct03  SHiggins@tinyRTX.com  Split out USER interface calls.
//  27Jan04  SHiggins@tinyRTX.com  Split out UADC, updated comments.
//  30Jan04  SHiggins@tinyRTX.com  Refined initialization.
//  29Jul14  SHiggins@tinyRTX.com  Moved UAPP_Timer1Init to MACRO to save stack.
//  13Aug14  SHiggins@tinyRTX.com  Converted from PIC16877 to PIC18F452.
//  14Apr15  Stephen_Higgins@KairosAutonomi.com
//               Converted from PIC18F452 to PIC18F2620.
//  29Apr15  Stephen_Higgins@KairosAutonomi.com
//               Added support for 2010 PICDEM2+ demo board (no 4 Mhz crystal).
//  05May15  Stephen_Higgins@KairosAutonomi.com
//               Added support for Kairos Autonomi 280B board.
//  14May15  Stephen_Higgins@KairosAutonomi.com  
//               Substitute #include <ucfg.inc> for <p18f452.inc>.
//  20May15  Stephen_Higgins@KairosAutonomi.com  
//               Fix UAPP_Timer1Init by adding terminating return.
//  28May15 Stephen_Higgins@KairosAutonomi
//               Move most logic from SUSR.asm to here in preparation
//               of turning this into UAPP.C.
//  29May15 Stephen_Higgins@KairosAutonomi
//               Create uapp_pd2p.asm from uapp.asm to support PICdem2+ boards.
//               Internal names are all still UAPP_xxx.
//               Create uapp_pd2p.c from uapp_pd2p.asm to allow user apps in C.
//  15Jul15 Stephen_Higgins@KairosAutonomi.
//               For compatibility with other tinyRTX upgrades, added null routines
//               UAPP_BkgdTask, UAPP_PutByteRxBuffer, UAPP_ParseRxMsg.
//  29Jul15 Stephen_Higgins@KairosAutonomi.
//               For compatibility with other tinyRTX upgrades, remove all CONFIG's
//               and replace with #include <uapp_ucfg.inc>.
//
//*******************************************************************************
//
// Complete PIC18F452 (40-pin device) pin assignments for PICDEM 2 Plus (2002) Demo Board
//   OR PICDEM 2 Plus (2010) Demo Board:
//
//  1) MCLR*/Vpp         = Reset/Programming connector(1): (active low, with debounce H/W)
//  2) RA0/AN0           = Analog In: Potentiometer Voltage 
//  3) RA1/AN1           = Discrete Out: LCD E (SLCD_CTRL_E)
//  4) RA2/AN2/Vref-     = Discrete Out: LCD RW (SLCD_CTRL_RW)
//  5) RA3/AN3/Vref+     = Discrete Out: LCD RS (SLCD_CTRL_RS)
//  6) RA4/TOCKI         = Discrete In:  Pushbutton S2 (active low, no debounce H/W)
//  7) RA5/AN4/SS*       = No Connect: (configured as Discrete In)
//  8) RE0/RD*/AN5       = No Connect: (configured as Discrete In)
//  9) RE1/WR*/AN6       = No Connect: (configured as Discrete In)
// 10) RE2/CS*/AN7       = No Connect: (configured as Discrete In)
// 11) Vdd               = Programming connector(2) (+5 VDC) 
// 12) Vss               = Programming connector(3) (Ground) 
//
//   External 4 Mhz crystal installed in Y2, PICDEM 2 Plus (2002).
//
// 13) OSC1/CLKIN        = 4 MHz clock in (4 MHz/4 = 1 MHz = 1us instr cycle)
//
//   External 4 Mhz crystal NOT installed in Y2, PICDEM 2 Plus (2010) PN 02-01630-1.
//   Note that Jumper J7 should not be connected.
//
// 13) OSC1/CLKIN        = No Connect.
//
// 14) OSC2/CLKOUT       = (non-configurable output)
// 15) RC0/T1OSO/T1CKI   = No Connect: (configured as Discrete In) (possible future Timer 1 OSO)
// 16) RC1/T1OSI         = No Connect: (configured as Discrete In) (possible future Timer 1 OSI)
// 17) RC2/CCP1          = Discrete Out: Peizo Buzzer (when J9 in place) (TEMPORARILY DISCRETE IN)
// 18) RC3/SCK/SCL       = I2C SCL: MSSP implementation of I2C requires pin as Discrete In. (Not used for SPI.)
// 19) RD0/PSP0          = Discrete Out/Discrete In: LCD data bit 4
// 20) RD1/PSP1          = Discrete Out/Discrete In: LCD data bit 5
// 21) RD2/PSP2          = Discrete Out/Discrete In: LCD data bit 6
// 22) RD3/PSP3          = Discrete Out/Discrete In: LCD data bit 7
// 23) RC4/SDI/SDA       = I2C SDA: MSSP implementation of I2C requires pin as Discrete In. (Not used for SPI.)
// 24) RC5/SDO           = No Connect: (configured as Discrete In) (Not used for SPI.)
// 25) RC6/TX/CK         = USART TX: RS-232 driver, USART control of this pin requires pin as Discrete In.
// 26) RC7/RX/DT         = USART RX: RS-232 driver, USART control of this pin requires pin as Discrete In.
// 27) RD4/PSP4          = No Connect: (configured as Discrete In)
// 28) RD5/PSP5          = No Connect: (configured as Discrete In)
// 29) RD6/PSP6          = No Connect: (configured as Discrete In)
// 30) RD7/PSP7          = No Connect: (configured as Discrete In)
// 31) Vss               = Programming connector(3) (Ground)
// 32) Vdd               = Programming connector(2) (+5 VDC)
// 33) RB0/INT           = Discrete Out: LED RB0 (when J6 in place)
//                       = Discrete In: RB0/INT also Pushbutton S3 (active low, with debounce H/W)
// 34) RB1               = Discrete Out: LED RB1 (when J6 in place)
// 35) RB2               = Discrete Out: LED RB2 (when J6 in place)
// 36) RB3/PGM           = Discrete Out: LED RB3 (when J6 in place)
// 37) RB4               = No Connect: (configured as Discrete In)
// 38) RB5               = No Connect: (configured as Discrete In)
// 39) RB6/PGC           = Programming connector(5) (PGC) ICD2 control of this pin requires pin as Discrete In.
// 40) RB7/PGD           = Programming connector(4) (PGD) ICD2 control of this pin requires pin as Discrete In.
//
//*******************************************************************************
//
// Complete PIC18F2620 (28-pin device) pin assignments for PICDEM 2 Plus (2002) Demo Board
//   OR PICDEM 2 Plus (2010) Demo Board:
//
//  1) MCLR*/Vpp         = Reset/Programming connector(1): (active low, with debounce H/W)
//  2) RA0/AN0           = Analog In: Potentiometer Voltage 
//  3) RA1/AN1           = Discrete Out: LCD E (SLCD_CTRL_E)
//  4) RA2/AN2/Vref-     = Discrete Out: LCD RW (SLCD_CTRL_RW)
//  5) RA3/AN3/Vref+     = Discrete Out: LCD RS (SLCD_CTRL_RS)
//  6) RA4/TOCKI         = Discrete In:  Pushbutton S2 (active low, no debounce H/W)
//  7) RA5/AN4/SS*       = No Connect: (configured as Discrete In)
//  8) Vss               = Programming connector(3) (Ground)
//
//   External 4 Mhz crystal installed in Y2, PICDEM 2 Plus (2002).
//
//  9) OSC1/CLKIN        = 4 MHz clock in (4 MHz/4 = 1 MHz = 1us instr cycle)
//
//   External 4 Mhz crystal NOT installed in Y2, PICDEM 2 Plus (2010) PN 02-01630-1.
//   Note that Jumper J7 should not be connected.
//
//  9) OSC1/CLKIN        = No Connect.
//
// 10) OSC2/CLKOUT       = (non-configurable output)
// 11) RC0/T1OSO/T1CKI   = No Connect: (configured as Discrete In) (possible future Timer 1 OSO)
// 12) RC1/T1OSI         = No Connect: (configured as Discrete In) (possible future Timer 1 OSI)
// 13) RC2/CCP1          = Discrete Out: Peizo Buzzer (when J9 in place) (TEMPORARILY DISCRETE IN)
// 14) RC3/SCK/SCL       = I2C SCL: MSSP implementation of I2C requires pin as Discrete In. (Not used for SPI.)
// 15) RC4/SDI/SDA       = I2C SDA: MSSP implementation of I2C requires pin as Discrete In. (Not used for SPI.)
// 16) RC5/SDO           = No Connect: (configured as Discrete In) (Not used for SPI.)
// 17) RC6/TX/CK         = USART TX: RS-232 driver, USART control of this pin requires pin as Discrete In.
// 18) RC7/RX/DT         = USART RX: RS-232 driver, USART control of this pin requires pin as Discrete In.
// 19) Vss               = Programming connector(3) (Ground)
// 20) Vdd               = Programming connector(2) (+5 VDC)
// 21) RB0/INT           = Discrete Out: LED RB0 (when J6 in place)
//                       = Discrete In: RB0/INT also Pushbutton S3 (active low, with debounce H/W)
// 22) RB1               = Discrete Out: LED RB1 (when J6 in place)
// 23) RB2               = Discrete Out: LED RB2 (when J6 in place)
// 24) RB3/PGM           = Discrete Out: LED RB3 (when J6 in place)
// 25) RB4               = No Connect: (configured as Discrete In)
// 26) RB5               = No Connect: (configured as Discrete In)
// 27) RB6/PGC           = Programming connector(5) (PGC) ICD2 control of this pin requires pin as Discrete In.
// 28) RB7/PGD           = Programming connector(4) (PGD) ICD2 control of this pin requires pin as Discrete In.
//
//*******************************************************************************

#include "ucfg.h"           // includes processor definitions.
#include "uapp_ucfg.h"      // includes #config definitions, only include in uapp*.c.
#include "si2c.h"
#include "ssio.h"
#include "usio.h"
#include "slcd.h"
#include "uadc.h"
#include "ui2c.h"
#include "ulcd.h"

extern void UAPP_POR_Init_PhaseB( void );
extern void UAPP_POR_Init_PhaseA( void );
extern void UAPP_Timer1Init( void );
extern void UAPP_Task1( void );
extern void UAPP_Task2( void );
extern void UAPP_Task3( void );
extern void UAPP_TaskADC( void );
extern void UAPP_BkgdTask( void );
extern void UAPP_PutByteRxBuffer( void );
extern void UAPP_ParseRxMsg( void );

//*******************************************************************************
//
#if UCFG_BOARD==UCFG_PD2P_2002
//
//   UCFG_PD2P_2002 specified.
//   *************************
//
// Hardware: PICdem 2 Plus 2002 board with 4 MHz external crystal.
//           TC74 digital temperature meter with I2C bus clocked at 100 kHz.
//
// Functions:
//  1) Read 1 A/D channel, convert A/D signal to engineering units and ASCII.
//  2) Read TC74 temperature value using I2C bus, convert to ASCII.
//  3) If 40-pin part, send ASCII text and commands to LCD display using 4-bit bus.
//  4) Send ASCII text to RS-232 port.  Receive and echo RS-232 bytes.
//
// User APP defines.
//
#define UAPP_OSCCON_VAL  0x00
//
//   Use primary oscillator/clock input pin
//
// bit 7 : N/A   : 0 : (unimplemented, don't care)
// bit 6 : N/A   : 0 : (unimplemented, don't care)
// bit 5 : N/A   : 0 : (unimplemented, don't care)
// bit 4 : N/A   : 0 : (unimplemented, don't care)
// bit 3 : N/A   : 0 : (unimplemented, don't care)
// bit 2 : N/A   : 0 : (unimplemented, don't care)
// bit 1 : N/A   : 0 : (unimplemented, don't care)
// bit 0 : SCS0  : 0 : System Clock Select, use primary oscillator/clock input pin
//
#define UAPP_PORTA_VAL  0x00
//
// PORTA cleared so any bits later programmed as output initialized to 0.
//
//   NOTE: ADCON1 must have ??? to ensure RA3:RA0 and RA5 not set as analog inputs.
//       On Power-On Reset they are configured as analog inputs.
//
//   NOTE: CMCON must have ??? to ensure the comparators are off and use RA3:RA0 as discrete in.
//
// bit 7 : OSC1/CLKIN/RA7            : 0 : Using OSC1 (don't care)
// bit 6 : OSC2/CLKOUT/RA6           : 0 : Using OSC2 (don't care)
// bit 5 : RA5/AN4/SS*/HLVDIN/C2OUT  : 0 : DiosPro TX TTL: (unused, configured as Discrete In)
// bit 4 : RA4/T0KI/C1OUT            : 0 : DiosPro RX TTL: (unused, configured as Discrete In)
// bit 3 : RA3/AN2/Vref+             : 0 : RA3 input (don't care) 
// bit 2 : RA2/AN2/Vref-/CVref       : 0 : (unused, configured as RA2 input) (don't care)
// bit 1 : RA1/AN1                   : 0 : (unused, configured as RA1 input) (don't care)
// bit 0 : RA0/AN0                   : 0 : (unused, configured as RA0 input) (don't care)
//
#define UAPP_TRISA_VAL  0x3f
//
// Set all PORTA to inputs.
//
// bit 7 : TRISA7 : 0 : Using OSC1, overridden by CONFIG1H (don't care)
// bit 6 : TRISA6 : 0 : Using OSC2, overridden by CONFIG1H (don't care)
// bit 5 : DDRA5  : 1 : Disrete In
// bit 4 : DDRA4  : 1 : Disrete In
// bit 3 : DDRA3  : 1 : Disrete In
// bit 2 : DDRA2  : 1 : Disrete In
// bit 1 : DDRA1  : 1 : Disrete In
// bit 0 : DDRA0  : 1 : Disrete In
//
#define UAPP_PORTB_VAL  0x00
//
// PORTB cleared so any bits later programmed as output initialized to 0.
//
// bit 7 : RB7/PGD               : 0 : Discrete In: (don't care)
// bit 6 : RB6/PGC               : 0 : Discrete In: (don't care)
// bit 5 : RB5/KB11/PGM          : 0 : Discrete In: (don't care)
// bit 4 : RB4/KB10/AN11         : 0 : Discrete In: (don't care)
// bit 3 : RB3/AN9/CCP2          : 0 : Discrete Out: Init to 0.
// bit 2 : RB2/INT2/AN8          : 0 : Discrete Out: Init to 0.
// bit 1 : RB1/INT1/AN10         : 0 : Discrete Out: Init to 0.
// bit 0 : RB0/INT0/FLT0/AN12    : 0 : Discrete Out: Init to 0. 
//
#define UAPP_TRISB_VAL  0xf0
//
// Set TRISB RB0-RB3 to outputs for LED's.
// Set TRISB RB4-RB7 to inputs.  PGC and PGD need to be configured as high-impedance inputs.
//
// bit 7 : DDRB7  : 1 : Discrete In
// bit 6 : DDRB6  : 1 : Discrete In
// bit 5 : DDRB5  : 1 : Discrete In
// bit 4 : DDRB4  : 1 : Discrete In
// bit 3 : DDRB3  : 0 : Discrete Out
// bit 2 : DDRB2  : 0 : Discrete Out
// bit 1 : DDRB1  : 0 : Discrete Out
// bit 0 : DDRB0  : 0 : Discrete Out
//
#define UAPP_PORTC_VAL  0x00
//
// PORTC cleared so any bits later programmed as output initialized to 0.
//
// bit 7 : RC7/RX/DT         : 0 : Discrete In: (don't care)
// bit 6 : RC6/TX/CK         : 0 : Discrete In: (don't care)
// bit 5 : RC5/SDO           : 0 : Discrete In: (don't care)
// bit 4 : RC4/SDI/SDA       : 0 : Discrete In: (don't care)
// bit 3 : RC3/SCK/SCL       : 0 : Discrete In: (don't care)
// bit 2 : RC2/CCP1          : 0 : Discrete In: (don't care)
// bit 1 : RC1/T1OSI         : 0 : Discrete In: (don't care)
// bit 0 : RC0/T1OSO/T1CKI   : 0 : Discrete In: (don't care)
//
#define UAPP_TRISC_VAL  0xff
//
// Set TRISC to all inputs.  SDA and SCL must be configured as inputs for I2C.
//
// bit 7 : DDRC7  : 1 : Discrete In, USART RX: RS-232 driver, USART control of this pin requires pin as Discrete In.
// bit 6 : DDRC6  : 1 : Discrete In, USART TX: RS-232 driver, USART control of this pin requires pin as Discrete In.
// bit 5 : DDRC5  : 1 : Discrete In, No Connect (configured as Discrete In) (Not used for SPI.)
// bit 4 : DDRC4  : 1 : Discrete In, I2C SDA: MSSP implementation of I2C requires pin as Discrete In. (Not used for SPI.)
// bit 3 : DDRC3  : 1 : Discrete In, I2C SCL: MSSP implementation of I2C requires pin as Discrete In. (Not used for SPI.)
// bit 2 : DDRC2  : 1 : Discrete In, Discrete Out: Peizo Buzzer (when J9 in place) (TEMPORARILY DISCRETE IN)
// bit 1 : DDRC1  : 1 : Discrete In, No Connect
// bit 0 : DDRC0  : 1 : Discrete In, No Connect
//
//   PORTD and PORTE defines in case we have 40-pin part.
//
#define UAPP_PORTD_VAL  0x00
#define UAPP_TRISD_VAL  0xf8
#define UAPP_PORTE_VAL  0x00
#define UAPP_TRISE_VAL  0x07
//
#define UAPP_T1CON_VAL  0x30
//
// 1:8 pre-scaler// T1 oscillator disabled// T1SYNC* ignored//
// TMR1CS internal clock Fosc/4// Timer1 off.
//
// bit 7 : RD16    : 0 : Read/write Timer1 in two 8-bit operations
// bit 6 : T1RUN   : 0 : Device clock is derived from another source
// bit 5 : T1CKPS1 : 1 : Timer1 Input Clock Prescale Select, 0b11 -> 1:8 Prescale value
// bit 4 : T1CKPS0 : 1 : Timer1 Input Clock Prescale Select, 0b11 -> 1:8 Prescale value
// bit 3 : T1OSCEN : 0 : T1 oscillator disabled
// bit 2 : T1SYNC  : 0 : IT1SYNC* ignored
// bit 1 : TMR1CS  : 0 : Internal clock (Fosc/4)
// bit 0 : TMR1ON  : 0 : Timer1 disabled
//
#define UAPP_TMR1L_VAL  0x2c
#define UAPP_TMR1H_VAL  0xcf
//
// 4 Mhz Fosc/4 is base clock = 1 Mhz = 1.0 us per clock.
// 1:8 prescale = 1.0 * 8 = 8.0 us per clock.
// 12,500 counts * 8.0us/clock = 100,000 us/rollover = 100ms/rollover.
// Timer preload value = 65,536 - 12,500 = 53,036 = 0xcf2c.
//
#endif
//
//*******************************************************************************
//
#if UCFG_BOARD==UCFG_PD2P_2010
//
//   UCFG_PD2P_2010 specified.
//   *************************
//
// Hardware: PICdem 2 Plus 2010 board with no external crystal.
//           TC74 digital temperature meter with I2C bus clocked at 100 kHz.
//
// Functions:
//  1) Read 1 A/D channel, convert A/D signal to engineering units and ASCII.
//  2) Read TC74 temperature value using I2C bus, convert to ASCII.
//  3) If 40-pin part, send ASCII text and commands to LCD display using 4-bit bus.
//  4) Send ASCII text to RS-232 port.  Receive and echo RS-232 bytes.
//
// User APP defines.
//
#define UAPP_OSCCON_VAL  0x66
//
//   4 Mhz clock// use internal oscillator block.
//
// bit 7 : IDLEN : 0 : Device enters Sleep mode on SLEEP instruction
// bit 6 : IRCF2 : 1 : Internal Oscillator Frequency Select, 0b110 -> 4 Mhz
// bit 5 : IRCF1 : 1 : Internal Oscillator Frequency Select, 0b110 -> 4 Mhz
// bit 4 : IRCF0 : 0 : Internal Oscillator Frequency Select, 0b110 -> 4 Mhz
// bit 3 : OSTS  : 0 : Oscillator Start-up Timer time-out is running// primary oscillator is not ready
// bit 2 : IOFS  : 1 : INTOSC frequency is stable
// bit 1 : SCS1  : 1 : System Clock Select, 0b1x -> use internal oscillator block
// bit 0 : SCS0  : 0 : System Clock Select, 0b1x -> use internal oscillator block
//
#define UAPP_PORTA_VAL  0x00
//
// PORTA cleared so any bits later programmed as output initialized to 0.
//
//   NOTE: ADCON1 must have ??? to ensure RA3:RA0 and RA5 not set as analog inputs.
//       On Power-On Reset they are configured as analog inputs.
//
//   NOTE: CMCON must have ??? to ensure the comparators are off and use RA3:RA0 as discrete in.
//
// bit 7 : OSC1/CLKIN/RA7            : 0 : Using OSC1 (don't care)
// bit 6 : OSC2/CLKOUT/RA6           : 0 : Using OSC2 (don't care)
// bit 5 : RA5/AN4/SS*/HLVDIN/C2OUT  : 0 : DiosPro TX TTL: (unused, configured as Discrete In)
// bit 4 : RA4/T0KI/C1OUT            : 0 : DiosPro RX TTL: (unused, configured as Discrete In)
// bit 3 : RA3/AN2/Vref+             : 0 : RA3 input (don't care) 
// bit 2 : RA2/AN2/Vref-/CVref       : 0 : (unused, configured as RA2 input) (don't care)
// bit 1 : RA1/AN1                   : 0 : (unused, configured as RA1 input) (don't care)
// bit 0 : RA0/AN0                   : 0 : (unused, configured as RA0 input) (don't care)
//
#define UAPP_TRISA_VAL  0x3f
//
// Set all PORTA to inputs.
//
// bit 7 : TRISA7 : 0 : Using OSC1, overridden by CONFIG1H (don't care)
// bit 6 : TRISA6 : 0 : Using OSC2, overridden by CONFIG1H (don't care)
// bit 5 : DDRA5  : 1 : Disrete In
// bit 4 : DDRA4  : 1 : Disrete In
// bit 3 : DDRA3  : 1 : Disrete In
// bit 2 : DDRA2  : 1 : Disrete In
// bit 1 : DDRA1  : 1 : Disrete In
// bit 0 : DDRA0  : 1 : Disrete In
//
#define UAPP_PORTB_VAL  0x00
//
// PORTB cleared so any bits later programmed as output initialized to 0.
//
// bit 7 : RB7/PGD               : 0 : Discrete In: (don't care)
// bit 6 : RB6/PGC               : 0 : Discrete In: (don't care)
// bit 5 : RB5/KB11/PGM          : 0 : Discrete In: (don't care)
// bit 4 : RB4/KB10/AN11         : 0 : Discrete In: (don't care)
// bit 3 : RB3/AN9/CCP2          : 0 : Discrete Out: Init to 0.
// bit 2 : RB2/INT2/AN8          : 0 : Discrete Out: Init to 0.
// bit 1 : RB1/INT1/AN10         : 0 : Discrete Out: Init to 0.
// bit 0 : RB0/INT0/FLT0/AN12    : 0 : Discrete Out: Init to 0. 
//
#define UAPP_TRISB_VAL  0xf0
//
// Set TRISB RB0-RB3 to outputs for LED's.
// Set TRISB RB4-RB7 to inputs.  PGC and PGD need to be configured as high-impedance inputs.
//
// bit 7 : DDRB7  : 1 : Discrete In
// bit 6 : DDRB6  : 1 : Discrete In
// bit 5 : DDRB5  : 1 : Discrete In
// bit 4 : DDRB4  : 1 : Discrete In
// bit 3 : DDRB3  : 0 : Discrete Out
// bit 2 : DDRB2  : 0 : Discrete Out
// bit 1 : DDRB1  : 0 : Discrete Out
// bit 0 : DDRB0  : 0 : Discrete Out
//
#define UAPP_PORTC_VAL  0x00
//
// PORTC cleared so any bits later programmed as output initialized to 0.
//
// bit 7 : RC7/RX/DT         : 0 : Discrete In: (don't care)
// bit 6 : RC6/TX/CK         : 0 : Discrete In: (don't care)
// bit 5 : RC5/SDO           : 0 : Discrete In: (don't care)
// bit 4 : RC4/SDI/SDA       : 0 : Discrete In: (don't care)
// bit 3 : RC3/SCK/SCL       : 0 : Discrete In: (don't care)
// bit 2 : RC2/CCP1          : 0 : Discrete In: (don't care)
// bit 1 : RC1/T1OSI         : 0 : Discrete In: (don't care)
// bit 0 : RC0/T1OSO/T1CKI   : 0 : Discrete In: (don't care)
//
#define UAPP_TRISC_VAL  0xff
//
// Set TRISC to all inputs.  SDA and SCL must be configured as inputs for I2C.
//
// bit 7 : DDRC7  : 1 : Discrete In, USART RX: RS-232 driver, USART control of this pin requires pin as Discrete In.
// bit 6 : DDRC6  : 1 : Discrete In, USART TX: RS-232 driver, USART control of this pin requires pin as Discrete In.
// bit 5 : DDRC5  : 1 : Discrete In, No Connect (configured as Discrete In) (Not used for SPI.)
// bit 4 : DDRC4  : 1 : Discrete In, I2C SDA: MSSP implementation of I2C requires pin as Discrete In. (Not used for SPI.)
// bit 3 : DDRC3  : 1 : Discrete In, I2C SCL: MSSP implementation of I2C requires pin as Discrete In. (Not used for SPI.)
// bit 2 : DDRC2  : 1 : Discrete In, Discrete Out: Peizo Buzzer (when J9 in place) (TEMPORARILY DISCRETE IN)
// bit 1 : DDRC1  : 1 : Discrete In, No Connect
// bit 0 : DDRC0  : 1 : Discrete In, No Connect
//
//   PORTD and PORTE defines in case we have 40-pin part.
//
#define UAPP_PORTD_VAL  0x00
#define UAPP_TRISD_VAL  0xf8
#define UAPP_PORTE_VAL  0x00
#define UAPP_TRISE_VAL  0x07
//
#define UAPP_T1CON_VAL  0x30
//
// 1:8 pre-scaler// T1 oscillator disabled// T1SYNC* ignored//
// TMR1CS internal clock Fosc/4// Timer1 off.
//
// bit 7 : RD16    : 0 : Read/write Timer1 in two 8-bit operations
// bit 6 : T1RUN   : 0 : Device clock is derived from another source
// bit 5 : T1CKPS1 : 1 : Timer1 Input Clock Prescale Select, 0b11 -> 1:8 Prescale value
// bit 4 : T1CKPS0 : 1 : Timer1 Input Clock Prescale Select, 0b11 -> 1:8 Prescale value
// bit 3 : T1OSCEN : 0 : T1 oscillator disabled
// bit 2 : T1SYNC  : 0 : IT1SYNC* ignored
// bit 1 : TMR1CS  : 0 : Internal clock (Fosc/4)
// bit 0 : TMR1ON  : 0 : Timer1 disabled
//
#define UAPP_TMR1L_VAL  0x2c
#define UAPP_TMR1H_VAL  0xcf
//
// 4 Mhz Fosc/4 is base clock = 1 Mhz = 1.0 us per clock.
// 1:8 prescale = 1.0 * 8 = 8.0 us per clock.
// 12,500 counts * 8.0us/clock = 100,000 us/rollover = 100ms/rollover.
// Timer preload value = 65,536 - 12,500 = 53,036 = 0xcf2c.
//
#endif
//
//*******************************************************************************
//
#define UAPP_PIE1_VAL  0x00
//
// ADIE, RCIE, TXIE, SSPIE, CCP1IE, TMR2IE, TMR1IE disabled.
//
// bit 7 : PSPIE   : 0 : Disables the PSP read/write interrupt
// bit 6 : ADIE    : 0 : Disables the A/D interrupt
// bit 5 : RCIE    : 0 : Disables the EUSART receive interrupt
// bit 4 : TXIE    : 0 : Disables the EUSART transmit interrupt
// bit 3 : SSPIE   : 0 : Disables the MSSP interrupt
// bit 2 : CCP1IE  : 0 : Disables the CCP1 interrupt
// bit 1 : TMR2IE  : 0 : Disables the TMR2 to PR2 match interrupt
// bit 0 : TMR1IE  : 0 : Disables the TMR1 overflow interrupt
//
#define UAPP_PIR1_VAL  0x00
//
// ADIF, RCIF, TXIF, SSPIF, CCP1IF, TMR2IF, TMR1IF cleared.
//
// bit 7 : PSPIF   : 0 : No read or write has occurred
// bit 6 : ADIF    : 0 : The A/D conversion is not complete
// bit 5 : RCIF    : 0 : The EUSART receive buffer is empty
// bit 4 : TXIF    : 0 : The EUSART transmit buffer is full
// bit 3 : SSPIF   : 0 : Waiting to transmit/receive
// bit 2 : CCP1IF  : 0 : No TMR1 register capture occurred
// bit 1 : TMR2IF  : 0 : No TMR2 to PR2 match occurred
// bit 0 : TMR1IF  : 0 : TMR1 register did not overflow
//
#define UAPP_RCON_VAL  0x1F
//
// IPEN cleared so disable priority levels on interrupts (PIC16 compatiblity mode.)
// RI set; TO set; PD set; POR set; BOR set; subsequent hardware resets will clear these bits.
//
// bit 7 : IPEN    : 0 : Disable priority levels on interrupts
// bit 6 : SBOREN  : 0 : Software BOR Enable
// bit 5 : n/a     : 0 : n/a
// bit 4 : NOT_RI  : 1 : RESET Instruction Flag
// bit 3 : NOT_TO  : 1 : Watchdog Timer Time-out Flag
// bit 2 : NOT_PD  : 1 : Power-Down Detection Flag
// bit 1 : NOT_POR : 1 : Power-on Reset Status
// bit 0 : NOT_BOR : 1 : Brown-out Reset Status
//
#define UAPP_INTCON_VAL  0xC0
//
// INTCON changed: GIE, PEIE enabled; TMR0IE, INT0IE, RBIE disabled; TMR0IF, INT0IF, RBIF cleared.
//
// bit 7 : GIE/GIEH  : 1 : Enables all unmasked interrupts
// bit 6 : PEIE/GIEL : 1 : Enables all unmasked peripheral interrupts
// bit 5 : TMR0IE    : 0 : Disables the TMR0 overflow interrupt
// bit 4 : INT0IE    : 0 : Disables the INT0 external interrupt
// bit 3 : RBIE      : 0 : Disables the RB port change interrupt
// bit 2 : TMR0IF    : 0 : TMR0 register did not overflow
// bit 1 : INT0IF    : 0 : The INT0 external interrupt did not occur
// bit 0 : RBIF      : 0 : None of the RB7:RB4 pins have changed state
//
//*******************************************************************************
//
// User application Power-On Reset initialization.

#pragma code    UAPP_CodeSec

void UAPP_POR_Init_PhaseA( void )
{
    OSCCON = UAPP_OSCCON_VAL;   // Configure Fosc. Note relation to CONFIG1H.
}

//*******************************************************************************
//
// User application Power-On Reset initialization.

void UAPP_POR_Init_PhaseB( void )
{
    PORTA = UAPP_PORTA_VAL;   // Clear initial data values in port.
    TRISA = UAPP_TRISA_VAL;   // Set port bits function and direction.
    PORTB = UAPP_PORTB_VAL;   // Clear initial data values in port.
    TRISB = UAPP_TRISB_VAL;   // Set port bits function and direction.
    PORTC = UAPP_PORTC_VAL;   // Clear initial data values in port.
    TRISC = UAPP_TRISC_VAL;   // Set port bits function and direction.

//   40-pin parts need setup of PORTD and PORTE.

    #if UCFG_PROC==UCFG_18F452
    PORTD = UAPP_PORTD_VAL;   // Clear initial data values in port.
    TRISD = UAPP_TRISD_VAL;   // Set port bits function and direction.
    PORTE = UAPP_PORTE_VAL;   // Clear initial data values in port.
    TRISE = UAPP_TRISE_VAL;   // Set port bits function and direction.
    #endif

// PIE1 changed: ADIE, RCIE, TXIE, SSPIE, CCP1IE, TMR2IE, TMR1IE disabled.

    PIE1 = UAPP_PIE1_VAL;

// PIR1 changed: ADIF, RCIF, TXIF, SSPIF, CCP1IF, TMR2IF, TMR1IF cleared.

    PIR1 = UAPP_PIR1_VAL;

// PIE2 untouched// EEIE, BCLIE disabled.
// PIR2 untouched// EEIR, BCLIF remain cleared.
//
// IPEN cleared so disable priority levels on interrupts (PIC16 compatiblity mode.)
// RI set// TO set// PD set// POR set// BOR set// subsequent hardware resets will clear these bits.

    RCON = UAPP_RCON_VAL;

// INTCON changed: GIE, PEIE enabled// TMR0IE, INT0IE, RBIE disabled// TMR0IF, INT0IF, RBIF cleared.

    INTCON = UAPP_INTCON_VAL;

//   Global interrupts enabled. The following routines
//   may enable additional specific interrupts.

    UADC_Init();        // User ADC hardware init.

    #if UCFG_PROC==UCFG_18F452
        SLCD_Init();    // System LCD init.
    #endif

    UI2C_Init();        // User I2C hardware init.
    ULCD_Init();        // User LCD init.
    USIO_Init();        // User Serial I/O hardware init.
}

//*******************************************************************************
//
// Init Timer1 module to generate timer interrupt every 100ms.

void UAPP_Timer1Init( void )
{
    T1CON = UAPP_T1CON_VAL;   // Initialize Timer1 but don't start it.
    TMR1L = UAPP_TMR1L_VAL;   // Timer1 pre-load value, low byte.
    TMR1H = UAPP_TMR1H_VAL;   // Timer1 pre-load value, high byte.

    PIE1bits.TMR1IE = 1;      // Enable Timer1 interrupts.
    T1CONbits.TMR1ON = 1;     // Turn on Timer1 module.
}

//*******************************************************************************
//
// Task1

void UAPP_Task1( void )
{
    LATBbits.LATB0 = LATBbits.LATB0^1;  // Toggle LED 1.
    UADC_Trigger();                     // Initiate new A/D conversion. (Enables ADC int.)

// UADC_Trigger enabled ADC interrupts.
}

//*******************************************************************************
//
// Task2.

void UAPP_Task2( void )
{
    LATBbits.LATB1 = LATBbits.LATB1^1;  // Toggle LED 2.
    ULCD_RefreshLine1();                // Update LCD data buffer with scrolling message.

    #if UCFG_PROC==UCFG_18F452
        SLCD_RefreshLine1();            // Send LCD data buffer to LCD.
    #endif
}

//*******************************************************************************
//
// Task3.

void UAPP_Task3( void )
{
    LATBbits.LATB2 = LATBbits.LATB2^1;  // Toggle LED 3.
    UI2C_MsgTC74();                     // Use I2C to get raw temperature from TC74.
}

//*******************************************************************************
//
// TaskADC - Convert A/D result and do something with it.

void UAPP_TaskADC( void )
{
    UADC_RawToASCII();              // Convert A/D result to ASCII msg for display.
    ULCD_RefreshLine2();            // Update LCD data buffer with A/D and temperature.

    #if UCFG_PROC==UCFG_18F452
        SLCD_RefreshLine2();        // Send LCD data buffer to LCD.
    #endif

    USIO_TxLCDMsgToSIO();           // Send LCD data buffer to serial I/O (RS-232).
}

//*******************************************************************************
//*******************************************************************************
//
//  These routines are here for backwards compatibility.  They are not used by
//  this application, but since the uapp.h defines them, they are here to
//  satisfy the linker.
//
//*******************************************************************************
//
// Background Task.  UNUSED.

void UAPP_BkgdTask( void )
{
}

//*******************************************************************************
//
// Put Byte in Receive Buffer.  UNUSED.

void UAPP_PutByteRxBuffer( void )
{
}

//*******************************************************************************
//
// Parse Receive Message.  UNUSED.

void UAPP_ParseRxMsg( void )
{
}
