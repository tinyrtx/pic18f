//*******************************************************************************
// tinyRTX Filename: uapp_ka107i.c
//  User APPlication for KA 107I board - Quad Encoder / Video Mux
//
// Copyright 2015 Kairos Autonomi ** www.tinyRTX.com **
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
//  03Sep15 Stephen_Higgins@KairosAutonomi.com
//              Created from uapp_ka280bi.c.
//
//*******************************************************************************
//
//   UCFG_KA107I specified.
//   **********************
//
// Hardware: Kairos Autonomi 107I circuit board.
//           Microchip PIC18F2620 processor with 10 MHz input resonator.
//
//  Functions:
//      8 quadrature encoder inputs to LS7566, which interfaces to 18F2620.
//      6:1 video encoder mux, controlled by 74HC174 hex latch, set by messages only.
//      Init message "[V: KA-107I 18F2620 v3.0.0 20151109]" (or whatever date)
//      Processes input messages: (c is checksum)
//      "[HH]"  responds with Help message.
//      "[VV]"  responds by sending Version message.
//      "[EE]"  responds by toggling Echo of received messages.
//      "[RR]"  responds by toggling Reporting W messages periodically.
//      "[WW]"  responds with "[Waaaaaa,bbbbbb,cccccc,dddddd]" (see UAPP_W_Msg()).
//      "[Mn]"  responds by setting analog video mux CD5041BC to input n.
//      "[BB]"  responds by invoking Bootloader.
//
//  20Hz operation only will be implemented
//  6:1 video mux operation will be implemented
//  Misc messages like query version, go to bootloader, will be implemented
//  2 wheel or 4 wheel or both?
//  No checksums either way
//  Timing disabled
//  No Delta X
//  No Timing Prescalar
//  No LED on/off
//  No Dump 1 wire and no Set 1 wire
//  No Reset (via message)
//
// Complete PIC18F2620 (28-pin device) pin assignments for KA board 107I:
//
//  1) MCLR*/Vpp/RE3             = Reset/Programming connector(1): ATN TTL (active low)
//  2) RA0/AN0                   = Discrete Out: 74HC174 CLK (rising edge clocks latches)
//  3) RA1/AN1                   = Discrete Out: LS7566 RD (active low)
//  4) RA2/AN2/Vref-/CVref       = Discrete Out: LS7566 DB6
//  5) RA3/AN3/Vref+             = Discrete Out: LS7566 WR (rising edge clocks write)
//  6) RA4/T0KI/C1OUT            = Discrete Out: LS7566 DB4, 74HC174 5D -> unused
//  7) RA5/AN4/SS*/HLVDIN/C2OUT  = Discrete Out: LS7566 DB5, 74HC174 6D -> Test LED
//  8) Vss                       = Programming connector(3) (Ground)
//
//   External 10 MHz ceramic oscillator installed in pins 9, 10; KA board 107I.
//
//  9) OSC1/CLKIN/RA7        = 10 MHz clock in (10 MHz * 4(PLL)/4 = 10 MHz = 0.1us instr cycle)
// 10) OSC2/CLKOUT/RA6       = 10 MHz clock out (non-configurable output)
//
// 11) RC0/T1OSO/T13CKI      = Discrete Out: LS7566 RS2
// 12) RC1/T1OSI/CCP2        = Discrete Out: LS7566 RS1
// 13) RC2/CCP1              = Discrete Out: LS7566 RS0
// 14) RC3/SCK/SCL           = Discrete Out: LS7566 CHS1 (Not used for SPI.) (Not used for I2C.)
// 15) RC4/SDI/SDA           = Discrete Out: LS7566 CHS0 (Not used for SPI.) (Not used for I2C.)
// 16) RC5/SDO               = Discrete Out: LS7566 DB7  (Not used for SPI.)
// 17) RC6/TX/CK             = Discrete In: USART TX (RS-232)
//                               USART control of this pin requires pin as Discrete In.
// 18) RC7/RX/DT             = Discrete In: USART RX (RS-232)
//                               USART control of this pin requires pin as Discrete In.
// 19) Vss                   = Programming connector(3) (Ground)
// 20) Vdd                   = Programming connector(2) (+5 VDC)
// 21) RB0/INT0/FLT0/AN12    = Discrete In: Quadrature Q0A (don't care)
// 22) RB1/INT1/AN10         = Discrete In: Quadrature Q1A (Pulled to Ground) (don't care)
// 23) RB2/INT2/AN8          = Discrete In: OneWire1 (unused, don't care)
// 24) RB3/AN9/CCP2          = Discrete In: OneWire2 (unused, don't care)
// 25) RB4/KB10/AN11         = Discrete Out: LS7566 DB3, 74HC174 4D -> CD4051BC INH
// 26) RB5/KB11/PGM          = Discrete Out: LS7566 DB2, 74HC174 3D -> CD4051BC C (Pulled to Ground) (don't care) WHY???
// 27) RB6/KB12/PGC          = Discrete Out: LS7566 DB1, 74HC174 2D -> CD4051BC B, Programming connector(5) (PGC)
//                               ICD2 control of this pin requires pin as Discrete In.
// 28) RB7/KB13/PGD          = Discrete Out: LS7566 DB0, 74HC174 1D -> CD4051BC A, Programming connector(4) (PGD)
//                               ICD2 control of this pin requires pin as Discrete In.
//
//  Jumper settings where: (DP) = DiosPro default, (KA) = KA F2620 SW default
//      (Note: SWRX may be labeled as ProgRX, SWTX may be labeled as ProgTX)
//      (Note: This KA board SW used HWRX/TX and hserin and hserout.)
//      JP11: local SWTX (see JP10)
//          1-2: chip (pin 7) P16D (DP) (KA)
//          2-3: chip (pin 25) P3D
//      JP9: target (pin 6) RS-232 RX
//          1-2: chip (pin 6) SWRX
//          2-3: chip (pin 18) HWRX (DP) (KA)
//      JP10: target (pin 7) RS-232 TX
//          1-2: chip (pin 7) SWTX
//          2-3: chip (pin 17) HWTX (DP) (KA)
//      Note: Currently no way to access whichever pair of RX/TX pins (either HW or SW) are unused
//
//  CD4051BC INPUT STATES    "ON" CHANNELS
//           INHIBIT C B A
//               0   0 0 0   0
//               0   0 0 1   1
//               0   0 1 0   2
//               0   0 1 1   3
//               0   1 0 0   4
//               0   1 0 1   5
//               0   1 1 0   6
//               0   1 1 1   7
//               1   x x x   NONE
//
//*******************************************************************************

#include "ucfg.h"          // includes processor definitions.
#include "uapp_ucfg.h"     // includes #config definitions, only include in uapp*.c.
#include "sqen.h"
#include "ssio.h"
#include "sutl.h"
#include "uadc.h"
#include "usio.h"

//  Internal prototypes.

void UAPP_W_Msg( void );
void UAPP_ClearRcBuffer( void );
void UAPP_WriteVideoMuxSelect( void );

//  Constants.

#define FALSE 0
#define TRUE  0xFF

//  String literals.

#pragma romdata   UAPP_ROMdataSec

const rom char UAPP_MsgVersion[] = "[V: KA-107I 18F2620 v3.0.0 20151109]\n\r";
const rom char UAPP_MsgHelp[] = "[H: H-elp V-ersion E-cho R-eport W-heelspeed M-ux B-ootloader]\n\r";
const rom char UAPP_MsgMuxHelp[] = "[M?: Use format [Mn] where n = 0 through 7]\n\r";
const rom char UAPP_MsgEchoActive[] = "[E: Message echo activated]\n\r";
const rom char UAPP_MsgEchoInactive[] = "[E: Message echo deactivated]\n\r";
const rom char UAPP_ReportActive[] = "[R: Reporting activated]\n\r";
const rom char UAPP_ReportInactive[] = "[R: Reporting deactivated]\n\r";
const rom char UAPP_MsgNotImplemented[] = "[?: Not implemented]\n\r";
const rom char UAPP_MsgNotRecognized[] = "[?: Not recognized]\n\r";
const rom char UAPP_Nibble_ASCII[] = "0123456789ABCDEF";

//  Variables.

#pragma udata   UAPP_UdataSec

SUTL_Byte UAPP_VideoMuxSelectBits;

// Internal variables to hold wheel data.

SUTL_ShortLong UAPP_OL_Q0;
SUTL_ShortLong UAPP_OL_Q1;
SUTL_ShortLong UAPP_OL_Q2;
SUTL_ShortLong UAPP_OL_Q3;

SUTL_ShortLong UAPP_OL_Q0_Prev;
SUTL_ShortLong UAPP_OL_Q1_Prev;
SUTL_ShortLong UAPP_OL_Q2_Prev;
SUTL_ShortLong UAPP_OL_Q3_Prev;

signed short long UAPP_SumWheelDeltas;  // Sum of all wheel deltas per timebase.

// Internal variables to manage receive buffer.

#define UAPP_BUFFERLENGTH 80

unsigned char UAPP_BufferRc[UAPP_BUFFERLENGTH];
unsigned char UAPP_IndexRc;

struct
{
    unsigned char UAPP_MsgEchoActive: 1;    //  Echo incoming Rc msg to Tx.
    unsigned char UAPP_ReportActive:  1;    //  Report data in Task 3.
    unsigned char unused            : 6;
} UAPP_Flags;

//*******************************************************************************
//
// User APP defines.

#define UAPP_OSCCON_VAL  0x64

//   40 Mhz clock; use HS PLL with external 10 MHz resonator.
//
//   NOTE: Configuration register CONFIG1H (@0x300001) must be set to 0x06.
//       FOSC3:FOSC0: 0b0110 = HS oscillator, PLL enabled (Clock Frequency = 4 x Fosc1)
//
// bit 7 : IDLEN : 0 : Device enters Sleep mode on SLEEP instruction
// bit 6 : IRCF2 : 1 : Internal Oscillator Frequency Select, 0b110 -> 4 Mhz (not used)
// bit 5 : IRCF1 : 1 : Internal Oscillator Frequency Select, 0b110 -> 4 Mhz (not used)
// bit 4 : IRCF0 : 0 : Internal Oscillator Frequency Select, 0b110 -> 4 Mhz (not used)
// bit 3 : OSTS  : 0 : Oscillator Start-up Timer time-out is running// primary oscillator is not ready
// bit 2 : IOFS  : 1 : INTOSC frequency is stable
// bit 1 : SCS1  : 0 : System Clock Select, 0b00 -> use primary oscillator
// bit 0 : SCS0  : 0 : System Clock Select, 0b00 -> use primary oscillator

#define UAPP_PORTA_VAL  0x00

// PORTA cleared so any bits later programmed as output initialized to 0.
//
//   NOTE: ADCON1 must have ??? to ensure RA3:RA0 and RA5 not set as analog inputs.
//       On Power-On Reset they are configured as analog inputs.
//
//   NOTE: CMCON must have ??? to ensure the comparators are off and use RA3:RA0 as discrete in.
//
// bit 7 : OSC1/CLKIN/RA7            : 0 : Using OSC1 (don't care)
// bit 6 : OSC2/CLKOUT/RA6           : 0 : Using OSC2 (don't care)
// bit 5 : RA5/AN4/SS*/HLVDIN/C2OUT  : 0 : Discrete Out: LS7566 DB5
// bit 4 : RA4/T0KI/C1OUT            : 0 : Discrete Out: LS7566 DB4
// bit 3 : RA3/AN3/Vref+             : 0 : Discrete Out: LS7566 WR (rising edge clocks write)
// bit 2 : RA2/AN2/Vref-/CVref       : 0 : Discrete Out: LS7566 DB6
// bit 1 : RA1/AN1                   : 0 : Discrete Out: LS7566 RD (active low)
// bit 0 : RA0/AN0                   : 0 : Discrete Out: 74HC174 CLK (rising edge clocks latches)

#define UAPP_TRISA_VAL  0x00

// Set all PORTA to don't care, outputs, and inputs.
//
// bit 7 : TRISA7 : 0 : Using OSC1, overridden by CONFIG1H (don't care)
// bit 6 : TRISA6 : 0 : Using OSC2, overridden by CONFIG1H (don't care)
// bit 5 : DDRA5  : 0 : Discrete Out
// bit 4 : DDRA4  : 0 : Discrete Out
// bit 3 : DDRA3  : 0 : Discrete Out
// bit 2 : DDRA2  : 0 : Discrete Out
// bit 1 : DDRA1  : 0 : Discrete Out
// bit 0 : DDRA0  : 0 : Discrete Out

#define UAPP_PORTB_VAL  0x00

// PORTB cleared so any bits later programmed as output initialized to 0.
//
// bit 7 : RB7/PGD              : 0 : Discrete Out: LS7566 DB0, 74HC174 1D -> CD4051BC A, Programming connector(4) (PGD)
//                                     TESTING: ICD2 control of this pin requires pin as Discrete In.
//                                     PRODUCTION: Driving LS7566 DB0, 74HC174 1D requires pin as Discrete Out.
// bit 6 : RB6/PGC              : 0 : Discrete Out: LS7566 DB1, 74HC174 2D -> CD4051BC B, Programming connector(5) (PGC)
//                                     TESTING: ICD2 control of this pin requires pin as Discrete In.
//                                     PRODUCTION: Driving LS7566 DB1, 74HC174 2D requires pin as Discrete Out.
// bit 5 : RB5/KB11/PGM         : 0 : Discrete Out: LS7566 DB2, 74HC174 3D -> CD4051BC C (Pulled to Ground)
// bit 4 : RB4/KB10/AN11        : 0 : Discrete Out: LS7566 DB3, 74HC174 4D -> CD4051BC INH
// bit 3 : RB3/AN9/CCP2         : 0 : Discrete In: OneWire2 (unused, don't care)
// bit 2 : RB2/INT2/AN8         : 0 : Discrete In: OneWire1 (unused, don't care)
// bit 1 : RB1/INT1/AN10        : 0 : Discrete In: Quadrature Q1A (Pulled to Ground) (don't care)
// bit 0 : RB0/INT0/FLT0/AN12   : 0 : Discrete In: Quadrature Q0A (don't care)
//
#define UAPP_TRISB_VAL  0xCF    //  TESTING.
//#define UAPP_TRISB_VAL  0x0F    //  PRODUCTION.
//
// Set TRISB RB0-RB7 to inputs.  PGC and PGD need to be configured as high-impedance inputs.
//
// bit 7 : DDRB7  : 1 : Discrete In     TESTING: ICD2 control of this pin requires pin as Discrete In.
// bit 6 : DDRB6  : 1 : Discrete In     TESTING: ICD2 control of this pin requires pin as Discrete In.
//
// bit 7 : DDRB7  : 0 : Discrete In     PRODUCTION: Driving LS7566 DB0, 74HC174 1D requires pin as Discrete Out.
// bit 6 : DDRB6  : 0 : Discrete In     PRODUCTION: Driving LS7566 DB1, 74HC174 2D requires pin as Discrete Out.
//
// bit 5 : DDRB5  : 0 : Discrete Out
// bit 4 : DDRB4  : 0 : Discrete Out
// bit 3 : DDRB3  : 1 : Discrete In
// bit 2 : DDRB2  : 1 : Discrete In
// bit 1 : DDRB1  : 1 : Discrete In
// bit 0 : DDRB0  : 1 : Discrete In

#define UAPP_PORTC_VAL  0x00

// PORTC cleared so any bits later programmed as output initialized to 0.
//
// bit 7 : RC7/RX/DT         : 0 : Discrete In: (don't care)
// bit 6 : RC6/TX/CK         : 0 : Discrete In: (don't care)
// bit 5 : RC5/SDO           : 0 : Discrete Out
// bit 4 : RC4/SDI/SDA       : 0 : Discrete Out
// bit 3 : RC3/SCK/SCL       : 0 : Discrete Out
// bit 2 : RC2/CCP1          : 0 : Discrete Out
// bit 1 : RC1/T1OSI         : 0 : Discrete Out
// bit 0 : RC0/T1OSO/T1CKI   : 0 : Discrete Out

#define UAPP_TRISC_VAL  0xC0

// Set TRISC to inputs for only USART RX/TX.  All others are outputs.
//
// bit 7 : DDRC7  : 1 : Discrete In, USART RX: RS-232 driver, USART control of this pin requires pin as Discrete In.
// bit 6 : DDRC6  : 1 : Discrete In, USART TX: RS-232 driver, USART control of this pin requires pin as Discrete In.
// bit 5 : DDRC5  : 0 : Discrete Out: LS7566 DB7  (Not used for SPI.)
// bit 4 : DDRC4  : 0 : Discrete Out: LS7566 CHS0 (Not used for SPI.) (Not used for I2C.)
// bit 3 : DDRC3  : 0 : Discrete Out: LS7566 CHS1 (Not used for SPI.) (Not used for I2C.)
// bit 2 : DDRC2  : 0 : Discrete Out: LS7566 RS0
// bit 1 : DDRC1  : 0 : Discrete Out: LS7566 RS1
// bit 0 : DDRC0  : 0 : Discrete Out: LS7566 RS2

#define UAPP_T1CON_VAL  0x30

// 1:8 pre-scaler; T1 oscillator disabled; T1SYNC* ignored;
// TMR1CS internal clock Fosc/4; Timer1 off.
//
// bit 7 : RD16    : 0 : Read/write Timer1 in two 8-bit operations
// bit 6 : T1RUN   : 0 : Device clock is derived from another source
// bit 5 : T1CKPS1 : 1 : Timer1 Input Clock Prescale Select, 0b11 -> 1:8 Prescale value
// bit 4 : T1CKPS0 : 1 : Timer1 Input Clock Prescale Select, 0b11 -> 1:8 Prescale value
// bit 3 : T1OSCEN : 0 : T1 oscillator disabled
// bit 2 : T1SYNC  : 0 : IT1SYNC* ignored
// bit 1 : TMR1CS  : 0 : Internal clock (Fosc/4)
// bit 0 : TMR1ON  : 0 : Timer1 disabled

#define UAPP_TMR1L_VAL  0x2C
#define UAPP_TMR1H_VAL  0xCF

// 40 Mhz Fosc/4 is base clock = 10 Mhz = 0.1 us per clock.
// 1:8 prescale = 0.1 * 8 = 0.8 us per clock.
// 12,500 counts * 0.8us/clock = 10,000 us/rollover = 10ms/rollover.
// Timer preload value = 65,536 - 12,500 = 53,036 = 0xCF2C.

#define UAPP_T0CON_VAL  0x08

// Timer0 stopped; no pre-scaler; 16-bit timer;
// use Internal instruction cycle clock Fosc/4 (10 MHz -> 0.1us);
//
// bit 7 : TMR0ON : 0 : Timer0 is stopped
// bit 6 : T08BIT : 0 : Timer0 is configured as a 16-bit timer/counter
// bit 5 : T0CS   : 0 : Internal instruction cycle clock (CLKO)
// bit 4 : T0SE   : 0 : Increment on low-to-high transition on T0CKI pin (don't care)
// bit 3 : PSA    : 1 : Timer0 prescaler is not assigned. Timer0 clock input bypasses prescaler.
// bit 2 : T0PS2  : 0 : (don't care)
// bit 1 : T0PS1  : 0 : (don't care)
// bit 0 : T0PS0  : 0 : (don't care)
//
//*******************************************************************************

#define UAPP_PIE1_VAL  0x00

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

#define UAPP_PIR1_VAL  0x00

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

#define UAPP_RCON_VAL  0x1F

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

#define UAPP_INTCON_VAL  0xC0

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

// PIE1 changed: ADIE, RCIE, TXIE, SSPIE, CCP1IE, TMR2IE, TMR1IE disabled.

    PIE1 = UAPP_PIE1_VAL;

// PIR1 changed: ADIF, RCIF, TXIF, SSPIF, CCP1IF, TMR2IF, TMR1IF cleared.

    PIR1 = UAPP_PIR1_VAL;

// PIE2 untouched; EEIE, BCLIE disabled.
// PIR2 untouched; EEIR, BCLIF remain cleared.

// IPEN cleared so disable priority levels on interrupts (PIC16 compatiblity mode.)
// RI set; TO set; PD set; POR set; BOR set; subsequent hardware resets will clear these bits.

    RCON = UAPP_RCON_VAL;

// INTCON changed: GIE, PEIE enabled// TMR0IE, INT0IE, RBIE disabled// TMR0IF, INT0IF, RBIF cleared.

    INTCON = UAPP_INTCON_VAL;

//  Global interrupts enabled. The following routines
//      may enable additional specific interrupts.

    UADC_Init();            // User ADC init (config pins even if not using ADC.)
    UAPP_ClearRcBuffer();   // Clear Rc buffer before messages can arrive.
    USIO_Init();            // User Serial I/O hardware init.

    UAPP_Flags.UAPP_MsgEchoActive = 0;  // Do not echo received msgs.
    UAPP_Flags.UAPP_ReportActive = 0;   // Do not report data (until I implement it.)

    // Reset all four SQEN channels.
    SQEN_7566_Write( SQEN_CHAN0, SQEN_CMR, SQEN_MASTER_RESET );
    SQEN_7566_Write( SQEN_CHAN1, SQEN_CMR, SQEN_MASTER_RESET );
    SQEN_7566_Write( SQEN_CHAN2, SQEN_CMR, SQEN_MASTER_RESET );
    SQEN_7566_Write( SQEN_CHAN3, SQEN_CMR, SQEN_MASTER_RESET );

    // Set all four SQEN counting modes to one count per quad cycle.
    SQEN_7566_Write( SQEN_CHAN0, SQEN_MDR0, SQEN_QUAD_X1 );
    SQEN_7566_Write( SQEN_CHAN1, SQEN_MDR0, SQEN_QUAD_X1 );
    SQEN_7566_Write( SQEN_CHAN2, SQEN_MDR0, SQEN_QUAD_X1 );
    SQEN_7566_Write( SQEN_CHAN3, SQEN_MDR0, SQEN_QUAD_X1 );

    SSIO_PutStringTxBuffer( (char*) UAPP_MsgVersion );     // Version message.
}

//*******************************************************************************
//
// Init Timer1 module to generate timer interrupt every 50ms.

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
// Background Task. UNUSED.
// Not used for I/O board functions, only 280BT Transmission.

void UAPP_BkgdTask( void )
{
}

//*******************************************************************************
//
// Task1 - 10 ms.  UNUSED.

void UAPP_Task1( void )
{
}

//*******************************************************************************
//
// Task2 - 100 ms.

void UAPP_Task2( void )
{
    // Save existing values to create deltas.
    UAPP_OL_Q0_Prev = UAPP_OL_Q0;
    UAPP_OL_Q1_Prev = UAPP_OL_Q1;
    UAPP_OL_Q2_Prev = UAPP_OL_Q2;
    UAPP_OL_Q3_Prev = UAPP_OL_Q3;

    // Cause 7566 to transfer all CNTRs to OL's.
    SQEN_7566_Write( SQEN_CHAN0, SQEN_CMR, SQEN_LOAD_OL );
    SQEN_7566_Write( SQEN_CHAN1, SQEN_CMR, SQEN_LOAD_OL );
    SQEN_7566_Write( SQEN_CHAN2, SQEN_CMR, SQEN_LOAD_OL );
    SQEN_7566_Write( SQEN_CHAN3, SQEN_CMR, SQEN_LOAD_OL );

    // Load in all the OL values from the 7566.
    UAPP_OL_Q0.byte2 = SQEN_7566_Read( SQEN_CHAN0, SQEN_OL2 );
    UAPP_OL_Q0.byte1 = SQEN_7566_Read( SQEN_CHAN0, SQEN_OL1 );
    UAPP_OL_Q0.byte0 = SQEN_7566_Read( SQEN_CHAN0, SQEN_OL0 );

    UAPP_OL_Q1.byte2 = SQEN_7566_Read( SQEN_CHAN1, SQEN_OL2 );
    UAPP_OL_Q1.byte1 = SQEN_7566_Read( SQEN_CHAN1, SQEN_OL1 );
    UAPP_OL_Q1.byte0 = SQEN_7566_Read( SQEN_CHAN1, SQEN_OL0 );

    UAPP_OL_Q2.byte2 = SQEN_7566_Read( SQEN_CHAN2, SQEN_OL2 );
    UAPP_OL_Q2.byte1 = SQEN_7566_Read( SQEN_CHAN2, SQEN_OL1 );
    UAPP_OL_Q2.byte0 = SQEN_7566_Read( SQEN_CHAN2, SQEN_OL0 );

    UAPP_OL_Q3.byte2 = SQEN_7566_Read( SQEN_CHAN3, SQEN_OL2 );
    UAPP_OL_Q3.byte1 = SQEN_7566_Read( SQEN_CHAN3, SQEN_OL1 );
    UAPP_OL_Q3.byte0 = SQEN_7566_Read( SQEN_CHAN3, SQEN_OL0 );

    // Sum the deltas of all the 24-bit OL values to report in W msg.
    UAPP_SumWheelDeltas  = UAPP_OL_Q0.shortLong - UAPP_OL_Q0_Prev.shortLong;
    UAPP_SumWheelDeltas += UAPP_OL_Q1.shortLong - UAPP_OL_Q1_Prev.shortLong;
    UAPP_SumWheelDeltas += UAPP_OL_Q2.shortLong - UAPP_OL_Q2_Prev.shortLong;
    UAPP_SumWheelDeltas += UAPP_OL_Q3.shortLong - UAPP_OL_Q3_Prev.shortLong;
}

//*******************************************************************************
//
// Task3 - 1.0 sec.

void UAPP_Task3( void )
{
//  LATAbits.LATA4 = LATAbits.LATA4 ^ 1;    // Toggle RA4 for testing

    //  Report message to output buffer if reporting active.
    if( UAPP_Flags.UAPP_ReportActive )
        UAPP_W_Msg();
}

//*******************************************************************************
//
// TaskADC - Convert A/D result and do something with it. UNUSED.

void UAPP_TaskADC( void )
{
}

//*******************************************************************************
//
// UAPP_W_Msg - Create a W message formatted for djLocalization.bas .
//
// W msg = "[Waaaaaa,bbbbbb,cccccc,dddddd]" where
//  W = Wheel quad counts (raw)
//  aaaaaa = Q0 CNTR value (24 bits)
//  bbbbbb = Q1 CNTR value (24 bits)
//  cccccc = Q2 CNTR value (24 bits)
//  dddddd = Q3 CNTR value (24 bits)

void UAPP_W_Msg( void )
{
SUTL_ShortLong UAPP_ShortLongTemp;

    SSIO_PutByteTxBufferC( '[' );
    SSIO_PutByteTxBufferC( 'W' );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q0.nibble5 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q0.nibble4 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q0.nibble3 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q0.nibble2 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q0.nibble1 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q0.nibble0 ]);

    SSIO_PutByteTxBufferC( ',' );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q1.nibble5 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q1.nibble4 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q1.nibble3 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q1.nibble2 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q1.nibble1 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q1.nibble0 ]);

    SSIO_PutByteTxBufferC( ',' );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q2.nibble5 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q2.nibble4 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q2.nibble3 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q2.nibble2 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q2.nibble1 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q2.nibble0 ]);

    SSIO_PutByteTxBufferC( ',' );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q3.nibble5 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q3.nibble4 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q3.nibble3 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q3.nibble2 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q3.nibble1 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_OL_Q3.nibble0 ]);

    // Change type to allow nibble access.
    UAPP_ShortLongTemp.shortLong = UAPP_SumWheelDeltas;

    SSIO_PutByteTxBufferC( ',' );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_ShortLongTemp.nibble5 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_ShortLongTemp.nibble4 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_ShortLongTemp.nibble3 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_ShortLongTemp.nibble2 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_ShortLongTemp.nibble1 ]);
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_ShortLongTemp.nibble0 ]);
    SSIO_PutByteTxBufferC( '\n' );
    SSIO_PutByteTxBufferC( '\r' );
}

//*******************************************************************************
//
//  Clear UAPP Rc buffer.

void UAPP_ClearRcBuffer( void )
{
    UAPP_IndexRc = 0;
}

//*******************************************************************************
//
//  Copy input byte to UAPP Rc buffer.

void UAPP_PutByteRcBuffer( unsigned char RcChar )
{
    UAPP_BufferRc[UAPP_IndexRc++] = RcChar;
}

//*******************************************************************************
//
//  Parse command string in UAPP Rc buffer.
//
void UAPP_ParseRcMsg( void )
{
unsigned char i;
char c;
rom char* UAPP_RomMsgPtr;

UAPP_RomMsgPtr = 0;    //  Nonzero will mean there is a message to output.

//    for( i=0; i<UAPP_IndexRc; i++ )
//       if( UAPP_BufferRc[i] != 0x0a && UAPP_BufferRc[i] != 0x0d )
//            SSIO_PutByteTxBufferC( UAPP_BufferRc[i] );

    if( UAPP_Flags.UAPP_MsgEchoActive )             // Echo msg if flag active.
        {
        for( i=0; i<UAPP_IndexRc; i++ )
            if( UAPP_BufferRc[i] != 0x0a && UAPP_BufferRc[i] != 0x0d )
                SSIO_PutByteTxBufferC( UAPP_BufferRc[i] );
  
        SSIO_PutByteTxBufferC( '\n' );
        SSIO_PutByteTxBufferC( '\r' );
        }

    // "[H: H-elp V-ersion E-cho R-eport W-heelspeed M-ux B-ootloader]\n\r";

    if( UAPP_BufferRc[0] == '[' )
    {
        switch( UAPP_BufferRc[1] ) {
        case 'H':
            UAPP_RomMsgPtr = UAPP_MsgHelp;          // Help message.
            break;  // case 'H'
        case 'V':
            UAPP_RomMsgPtr = UAPP_MsgVersion;       // Version message.
            break;  // case 'V'
        case 'E':
            UAPP_Flags.UAPP_MsgEchoActive ^= 1;     // Toggle whether to echo msgs.
            if( UAPP_Flags.UAPP_MsgEchoActive )     // Echo msg if flag active.
                UAPP_RomMsgPtr = UAPP_MsgEchoActive;
            else                                    // Don't echo msg if flag inactive.
                UAPP_RomMsgPtr = UAPP_MsgEchoInactive;
            break;  // case 'E'
        case 'R':
            UAPP_Flags.UAPP_ReportActive ^= 1;      // Toggle whether to report data.
            if( UAPP_Flags.UAPP_ReportActive )      // Report data if flag active.
                UAPP_RomMsgPtr = UAPP_ReportActive;
            else                                    // Don't report data if flag inactive.
                UAPP_RomMsgPtr = UAPP_ReportInactive;
            break;  // case 'R'
        case 'W':
            UAPP_W_Msg();
            break;  // case 'W'
        case 'M':
            switch( UAPP_BufferRc[2] ) {
            case '0':
                UAPP_VideoMuxSelectBits.nibble0 = 0x0;
                break;
            case '1':
                UAPP_VideoMuxSelectBits.nibble0 = 0x1;
                break;
            case '2':
                UAPP_VideoMuxSelectBits.nibble0 = 0x2;
                break;
            case '3':
                UAPP_VideoMuxSelectBits.nibble0 = 0x3;
                break;
            case '4':
                UAPP_VideoMuxSelectBits.nibble0 = 0x4;
                break;
            case '5':
                UAPP_VideoMuxSelectBits.nibble0 = 0x5;
                break;
            case '6':
                UAPP_VideoMuxSelectBits.nibble0 = 0x6;
                break;
            case '7':
                UAPP_VideoMuxSelectBits.nibble0 = 0x7;
                break;
            default:
                UAPP_RomMsgPtr = UAPP_MsgMuxHelp;   // Mux help message.
                break;
            };  // switch( UAPP_BufferRc[2] )
            UAPP_WriteVideoMuxSelect();             // Write video mux select.
            break;  // case 'M'
        case 'B':
            SUTL_InvokeBootloader();                // Bootloader.
            break;  // case 'B'
        default:
            UAPP_RomMsgPtr = UAPP_MsgNotRecognized; // Msg not recognized.
            break;
        };  // switch( UAPP_BufferRc[1] )
    }

    UAPP_ClearRcBuffer();

    //  If UAPP_RomMsgPtr was assigned then copy the message to TX buffer.
    //  Note we have to do this by character because we don't have a SSIO_PutRomStringTxBuffer.
    if( UAPP_RomMsgPtr )
        while (c = *UAPP_RomMsgPtr++)
            SSIO_PutByteTxBufferC( c );
}

//*******************************************************************************
//
// Select input to Video Mux by clocking select bits to 74H174 hex latch.

void UAPP_WriteVideoMuxSelect()
{
    // Setup latch data lines.
    LATBbits.LATB7 = UAPP_VideoMuxSelectBits.bit0;  // Valid mux select bit.
    LATBbits.LATB6 = UAPP_VideoMuxSelectBits.bit1;  // Valid mux select bit.
    LATBbits.LATB5 = UAPP_VideoMuxSelectBits.bit2;  // Valid mux select bit.
    LATBbits.LATB4 = 0;                             // Always write INH non-active.
    LATAbits.LATA4 = UAPP_VideoMuxSelectBits.bit5;  // Currently unused, good for timing?
    LATAbits.LATA5 = UAPP_VideoMuxSelectBits.bit6;  // Test LED.

_asm
    NOP                                     // Delay 100ns to ensure setup time for data.
_endasm

    // Clock data into latches.
    LATAbits.LATA0  = 0b0;                  // Drive WR low to allow write.
    LATAbits.LATA0  = 0b1;                  // Drive WR high to clock data in.
}
