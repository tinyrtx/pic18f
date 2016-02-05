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
//  04Dec15 Stephen_Higgins@KairosAutonomi.com
//              Milestone in development, only [Sy] prescaler msg not done.
//  21Dec15 Stephen_Higgins@KairosAutonomi.com
//              Milestone in testing, only [Sy] prescaler msg not done.
//              Combine UAPP_WriteVideoMuxSelect() and UAPP_WriteTestLED().
//  28Jan16 Stephen_Higgins@KairosAutonomi.com
//              Implement [Sy] pulse width timing prescaler.
//              Implement [U0],[U1] wheel pulse width measurement.
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
//      Init message "[V: KA-107I 18F2620 v3.0.0 20151203]" (or whatever date)
//
//  Processes input messages:
//      "[B]"   Invokes Bootloader.
//      "[Dn]"  Delta timing skipping n periods WHERE n is '@' - 'Z' (0 to 26).
//      "[E]"   Toggles Echo of received messages.
//      "[F1]"  Sets 10Hz reporting W messages.
//      "[F2]"  Sets 20Hz reporting W messages.
//      "[H]"   Sends Help message.
//      "[L0]"  Sets Test LED output inactive.
//      "[L1]"  Sets Test LED output active.
//      "[Mn]"  Sets analog video mux CD5041BC to input n WHERE n is '0' - '7'.
//      "[R]"   Toggles Reporting W messages periodically.
//      "[Sy]"  Pulse width timing prescaler WHERE n is '1' - '8'.
//      "[W2]"  Sets 2 wheel reporting.
//      "[W4]"  Sets 4 wheel reporting.
//      "[U0]"  Disables pulse width timing.
//      "[U1]"  Enables pulse width timing.
//      "[V]"   Sends Version message.
//      "[Z]"   Resets SQEN hardware.
//
//  Legacy messages for compatibility with djLoader and djLocalizer:
//      "[Mn]"  Sets analog video mux CD5041BC to input n WHERE n is 0x00 - 0x07.
//      "[Lx]"  Sets Test LED output inactive WHERE x is 0x1F.
//      "[Ly]"  Sets Test LED output active WHERE x is 0x20.
//      "[M(0xB2)]" Reset processor
//
//  MESSAGES NOT IMPLEMENTED:
//  =========================
//      Dump 1 wire and Set 1 wire.
//
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
// 21) RB0/INT0/FLT0/AN12    = Discrete In: Quadrature Q0A (INT0 rising edge for pulse timing)
//                              Interrupt On Change requires pin as Discrete In
// 22) RB1/INT1/AN10         = Discrete In: Quadrature Q1A (INT1 rising edge for pulse timing, needs Discrete In)
//                              Interrupt On Change requires pin as Discrete In
// 23) RB2/INT2/AN8          = Discrete In: OneWire1 (unused, don't care)
// 24) RB3/AN9/CCP2          = Discrete In: OneWire2 (unused, don't care)
// 25) RB4/KB10/AN11         = Discrete Out: LS7566 DB3, 74HC174 4D -> CD4051BC INH
// 26) RB5/KB11/PGM          = Discrete Out: LS7566 DB2, 74HC174 3D -> CD4051BC C (Pulled to Ground) (PGM not used.)
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

void UAPP_ClearRcBuffer( void );
void UAPP_InitSQEN( void );
void UAPP_WriteVideoMuxTestLED( void );
void UAPP_PWStateMachineDisable( void );
void UAPP_PWStateMachineEnable( void );
void UAPP_PWStateMachineMain( void );

//  Constants.

#define FALSE 0
#define TRUE  0xFF

//  String literals.

#pragma romdata   UAPP_ROMdataSec

const rom char UAPP_MsgVersion[] = "[V: KA-107I 18F2620 v3.0.0 20160205]\n\r";
const rom char UAPP_MsgDeltaActive[] = "[D: Delta timing active]\n\r";
const rom char UAPP_MsgDeltaInactive[] = "[D: Delta timing inactive]\n\r";
const rom char UAPP_MsgDeltaHelp[] = "[D?: Use format [Dn] where n = @ through Z]\n\r";
const rom char UAPP_MsgEchoActive[] = "[E: Message echo active]\n\r";
const rom char UAPP_MsgEchoInactive[] = "[E: Message echo inactive]\n\r";
const rom char UAPP_MsgFrequency10Hz[] = "[F1: Frequency 10 Hz]\n\r";
const rom char UAPP_MsgFrequency20Hz[] = "[F2: Frequency 20 Hz]\n\r";
const rom char UAPP_MsgFreqHelp[] = "[F?: F1 = 10 Hz, F2 = 20 Hz]\n\r";
const rom char UAPP_MsgHelp[] = "[H: D-elta E-cho F-requency H-elp L-ED M-ux R-eport V-ersion W-heelmode Z-Reset B-ootloader]\n\r";
const rom char UAPP_MsgTestLEDInactive[] = "[L0: Test LED inactive]\n\r";
const rom char UAPP_MsgTestLEDActive[] = "[L1: Test LED active]\n\r";
const rom char UAPP_MsgTestLEDHelp[] = "[L?: L0 = LED inactive, L1 = LED active]\n\r";
const rom char UAPP_MsgMux0[] = "[M0: Video Mux input 0]\n\r";
const rom char UAPP_MsgMux1[] = "[M1: Video Mux input 1]\n\r";
const rom char UAPP_MsgMux2[] = "[M2: Video Mux input 2]\n\r";
const rom char UAPP_MsgMux3[] = "[M3: Video Mux input 3]\n\r";
const rom char UAPP_MsgMux4[] = "[M4: Video Mux input 4]\n\r";
const rom char UAPP_MsgMux5[] = "[M5: Video Mux input 5]\n\r";
const rom char UAPP_MsgMux6[] = "[M6: Video Mux input 6]\n\r";
const rom char UAPP_MsgMux7[] = "[M7: Video Mux input 7]\n\r";
const rom char UAPP_MsgMuxHelp[] = "[M?: Use format [Mn] where n = 0 through 7]\n\r";
const rom char UAPP_MsgReportActive[] = "[R: Reporting active]\n\r";
const rom char UAPP_MsgReportInactive[] = "[R: Reporting inactive]\n\r";
const rom char UAPP_MsgPrescaler[] = "[S: Pulse width timing prescaler set]\n\r";
const rom char UAPP_MsgPrescalerHelp[] = "[S?: Use format [Sn] where n = 1 through 8]\n\r";
const rom char UAPP_Msg2Wheels[] = "[W2: 2 Wheel Mode]\n\r";
const rom char UAPP_Msg4Wheels[] = "[W4: 4 Wheel Mode]\n\r";
const rom char UAPP_MsgWheelsHelp[] = "[W?: W2 = 2 Wheel Mode, W4 = 4 Wheel Mode]\n\r";
const rom char UAPP_MsgDisableTiming[] = "[U0: Pulse width timing disabled]\n\r";
const rom char UAPP_MsgEnableTiming[] = "[U1: Pulse width timing enabled]\n\r";
const rom char UAPP_MsgTimingHelp[] = "[U?: U0 = Disable timing, U1 = Enable timing]\n\r";
const rom char UAPP_MsgReset[] = "[Z: SQEN reset]\n\r";
const rom char UAPP_MsgNotImplemented[] = "[?: Not implemented]\n\r";
const rom char UAPP_MsgNotRecognized[] = "[?: Not recognized]\n\r";
const rom char UAPP_Nibble_ASCII[] = "0123456789ABCDEF";

//  Variables.

#pragma udata   UAPP_UdataSec

SUTL_Byte UAPP_VideoMuxSelectBits;

unsigned char UAPP_DeltaReload;
unsigned char UAPP_DeltaCurrent;
unsigned char UAPP_PWState;
unsigned char UAPP_PWPassesWithoutInt0;
unsigned char UAPP_PWPassesWithoutInt1;
unsigned char UAPP_PWPrescale;

SUTL_Word UAPP_PWPulse0;
SUTL_Word UAPP_PWPulse1;

// Internal variables to hold wheel data.

SUTL_ShortLong UAPP_OL_Q0;
SUTL_ShortLong UAPP_OL_Q1;
SUTL_ShortLong UAPP_OL_Q2;
SUTL_ShortLong UAPP_OL_Q3;

SUTL_ShortLong UAPP_OL_Q0_Prev;
SUTL_ShortLong UAPP_OL_Q1_Prev;
SUTL_ShortLong UAPP_OL_Q2_Prev;
SUTL_ShortLong UAPP_OL_Q3_Prev;

SUTL_ShortLong UAPP_WheelDelta_Q0;
SUTL_ShortLong UAPP_WheelDelta_Q1;
SUTL_ShortLong UAPP_WheelDelta_Q2;
SUTL_ShortLong UAPP_WheelDelta_Q3;

// Internal variables to manage receive buffer.

#define UAPP_BUFFERLENGTH 80

unsigned char UAPP_BufferRc[UAPP_BUFFERLENGTH];
unsigned char UAPP_IndexRc;

struct
{
    unsigned char UAPP_MsgEchoActive:   1;  //  Echo incoming Rc msg to Tx.
    unsigned char UAPP_ReportActive:    1;  //  Report data at desired frequency.
    unsigned char UAPP_Frequency20Hz:   1;  //  Reporting frequency (Task2) 20 Hz.
    unsigned char UAPP_SkipTask2:       1;  //  Force Task2 to 10 Hz by skipping.
    unsigned char UAPP_TestLEDActive:   1;  //  Test LED is active.
    unsigned char UAPP_4WheelsActive:   1;  //  4 wheels mode active.
    unsigned char UAPP_DeltaActive:     1;  //  Delta timing mode active.
    unsigned char UAPP_PWTimingActive:  1;  //  Pulse width timing active.
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
//                                     TESTING: ICD control of this pin requires pin as Discrete In.
//                                     PRODUCTION: Driving LS7566 DB0, 74HC174 1D requires pin as Discrete Out.
// bit 6 : RB6/PGC              : 0 : Discrete Out: LS7566 DB1, 74HC174 2D -> CD4051BC B, Programming connector(5) (PGC)
//                                     TESTING: ICD control of this pin requires pin as Discrete In.
//                                     PRODUCTION: Driving LS7566 DB1, 74HC174 2D requires pin as Discrete Out.
// bit 5 : RB5/KB11/PGM         : 0 : Discrete Out: LS7566 DB2, 74HC174 3D -> CD4051BC C (Pulled to Ground)
// bit 4 : RB4/KB10/AN11        : 0 : Discrete Out: LS7566 DB3, 74HC174 4D -> CD4051BC INH
// bit 3 : RB3/AN9/CCP2         : 0 : Discrete In: OneWire2 (unused, don't care)
// bit 2 : RB2/INT2/AN8         : 0 : Discrete In: OneWire1 (unused, don't care)
// bit 1 : RB1/INT1/AN10        : 0 : Discrete In: Quadrature Q1A (Interrupt On Edge)
// bit 0 : RB0/INT0/FLT0/AN12   : 0 : Discrete In: Quadrature Q0A (Interrupt On Edge)

//#define UAPP_TRISB_VAL  0xCF    //  TESTING.
#define UAPP_TRISB_VAL  0x0F    //  PRODUCTION.

// Set TRISB RB0-RB3 to inputs, RB4-RB5 to outputs.
// If debugging, RB6-RB7 (ICD PGC and PGD) need to be configured as inputs.
// If production, RB6-RB7 need to be configured as outputs to drive LS7566.

// bit 7 : DDRB7  : 1 : Discrete In     TESTING: ICD control of this PGD pin requires pin as Discrete In.
// bit 6 : DDRB6  : 1 : Discrete In     TESTING: ICD control of this PGC pin requires pin as Discrete In.
//
// bit 7 : DDRB7  : 0 : Discrete Out    PRODUCTION: Driving LS7566 DB0, 74HC174 1D requires pin as Discrete Out.
// bit 6 : DDRB6  : 0 : Discrete Out    PRODUCTION: Driving LS7566 DB1, 74HC174 2D requires pin as Discrete Out.
//
// bit 5 : DDRB5  : 0 : Discrete Out
// bit 4 : DDRB4  : 0 : Discrete Out
// bit 3 : DDRB3  : 1 : Discrete In
// bit 2 : DDRB2  : 1 : Discrete In
// bit 1 : DDRB1  : 1 : Discrete In     (Interrupt On Edge)
// bit 0 : DDRB0  : 1 : Discrete In     (Interrupt On Edge)

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

#define UAPP_T0CON_VAL  0x00

// Timer0 stopped; 16-bit timer;
// use Internal instruction cycle clock Fosc/4 (10 MHz -> 0.1us);
// use pre-scaler; pre-scaler initialized to 1:2.
//
// bit 7 : TMR0ON : 0 : Timer0 is stopped
// bit 6 : T08BIT : 0 : Timer0 is configured as a 16-bit timer/counter
// bit 5 : T0CS   : 0 : Internal instruction cycle clock (CLKO)
// bit 4 : T0SE   : 0 : Increment on low-to-high transition on clock (don't care)
// bit 3 : PSA    : 0 : Timer0 prescaler is assigned. Timer0 clock input comes from prescaler output.
// bit 2 : T0PS2  : 0 : Prescaler value, 0b000 -> 1:2
// bit 1 : T0PS1  : 0 : Prescaler value, 0b000 -> 1:2
// bit 0 : T0PS0  : 0 : Prescaler value, 0b000 -> 1:2
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

// INTCON changed: GIE, PEIE enabled; TMR0IE, INT0IE, RBIE disabled; TMR0IF, INT0IF, RBIF cleared.

    INTCON = UAPP_INTCON_VAL;

//  Global interrupts enabled. The following routines
//      may enable additional specific interrupts.

    UADC_Init();            // User ADC init (config pins even if not using ADC.)
    UAPP_ClearRcBuffer();   // Clear Rc buffer before messages can arrive.
    USIO_Init();            // User Serial I/O hardware init.

    UAPP_Flags.UAPP_MsgEchoActive = 0;  // Don't echo received msgs.
    UAPP_Flags.UAPP_ReportActive = 1;   // Report data.
    UAPP_Flags.UAPP_Frequency20Hz = 1;  // Report frequency 20 Hz.
    UAPP_Flags.UAPP_SkipTask2 = 0;      // Don't skip task 2.
    UAPP_Flags.UAPP_TestLEDActive = 0;  // Test LED is inactive.
    UAPP_Flags.UAPP_4WheelsActive = 1;  // 4 wheels mode is active.
    UAPP_Flags.UAPP_DeltaActive = 0;    // Delta timing mode is inactive.
    UAPP_Flags.UAPP_PWTimingActive = 0; // Pulse width timing inactive.

    UAPP_InitSQEN();                    // Reset all four SQEN channels.
    UAPP_OL_Q0_Prev.shortLong = 0;      // Reset saved OL values.
    UAPP_OL_Q1_Prev.shortLong = 0;      // Reset saved OL values.
    UAPP_OL_Q2_Prev.shortLong = 0;      // Reset saved OL values.
    UAPP_OL_Q3_Prev.shortLong = 0;      // Reset saved OL values.

    T0CON = UAPP_T0CON_VAL;             // Initialize Timer0 but don't start it.
    UAPP_PWPrescale = 0x00;             // Clear pulse width prescale value.

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
// Task2 - 50ms (20Hz base frequency) OR 100ms (10Hz effective frequency).
//
//  Creats a W msg with optional sections
// W msg = "[Waaaaaa,bbbbbb(,cccccc,dddddd)(,eeee,ffff(,gggg,hhhh))]" where
//  aaaaaa = Q0 CNTR value (24 bits) = LR wheel sampled at 10 or 20 Hz
//  bbbbbb = Q1 CNTR value (24 bits) = RR wheel sampled at 10 or 20 Hz
//  cccccc = Q2 CNTR value (24 bits) = LF wheel sampled at 10 or 20 Hz (4 wheel mode)
//  dddddd = Q3 CNTR value (24 bits) = RF wheel sampled at 10 or 20 Hz (4 wheel mode)
//  eeee   = Q0 CNTR delta (16 bits) = LR wheel delta ([Dn] msg freq)
//  ffff   = Q1 CNTR delta (16 bits) = RR wheel delta ([Dn] msg freq)
//  gggg   = Q2 CNTR delta (16 bits) = LF wheel delta ([Dn] msg freq) (4 wheel mode)
//  hhhh   = Q3 CNTR delta (16 bits) = RF wheel delta ([Dn] msg freq) (4 wheel mode)

void UAPP_Task2( void )
{
SUTL_ShortLong UAPP_ShortLongTemp;
    //  To force Task2 to run at 10 Hz, we skip every other invocation.
    //  Unless we change SRTX, we have a hardcoded SRTX_CNT_RELOAD_TASK2 we have
    //  to live with.  Another option is to dynamically change the base task
    //  frequency from 10ms to 20ms, but that affects Task1 and Task3 as well, and
    //  doesn't really seem to be any cleaner.

    if( !UAPP_Flags.UAPP_Frequency20Hz )    // If frequency 20 Hz not active..
        {
        UAPP_Flags.UAPP_SkipTask2 ^= 1;     // ..then toggle whether to skip task.
        if( UAPP_Flags.UAPP_SkipTask2 )     // If toggle active then skip task.
            return;
        }
    // Return to processing common to 20Hz and 10Hz.

    // Update pulse width state machine.
    UAPP_PWStateMachineMain();

    // Temp for measuring timing.
    //UAPP_Flags.UAPP_TestLEDActive ^= 1;     // Toggle Test LED state.
    //UAPP_WriteVideoMuxTestLED();

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

    //  Report message to output buffer if reporting active.
    if( UAPP_Flags.UAPP_ReportActive )
        {
        // Send LR and RR wheels if in 2 wheel or 4 wheel mode.
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

        if( UAPP_Flags.UAPP_4WheelsActive )
            {
            // Send LF and RF wheels only if in 4 wheel mode.
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
            }

        if( UAPP_Flags.UAPP_PWTimingActive )
            {
            // Send pulse widths only if pulse width timing active.
            SSIO_PutByteTxBufferC( ',' );
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_PWPulse0.nibble3 ]);
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_PWPulse0.nibble2 ]);
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_PWPulse0.nibble1 ]);
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_PWPulse0.nibble0 ]);

            SSIO_PutByteTxBufferC( ',' );
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_PWPulse1.nibble3 ]);
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_PWPulse1.nibble2 ]);
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_PWPulse1.nibble1 ]);
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_PWPulse1.nibble0 ]);
            }
        }

    //  To implement Delta Timing, we execute every nth invocation,
    //  where n is received by a [Dn] message.

    if( UAPP_Flags.UAPP_DeltaActive )       // If Delta Timing is active..
        {
        if( 0 < UAPP_DeltaCurrent )         // ..then if current delta not expired..
            {
            UAPP_DeltaCurrent--;            // ..then decrement current delta, finish msg, and leave.
            if( UAPP_Flags.UAPP_ReportActive )
                {
                SSIO_PutByteTxBufferC( ']' );
                SSIO_PutByteTxBufferC( '\n' );
                SSIO_PutByteTxBufferC( '\r' );
                }
            return;
            }
        else
            UAPP_DeltaCurrent = UAPP_DeltaReload;   // ..else reload delta and execute.
        }

    // Return to processing common to active or inactive Delay Timing.

    // Form the deltas of all the 24-bit OL values to report in W msg.
    UAPP_WheelDelta_Q0.shortLong = UAPP_OL_Q0.shortLong - UAPP_OL_Q0_Prev.shortLong;
    UAPP_WheelDelta_Q1.shortLong = UAPP_OL_Q1.shortLong - UAPP_OL_Q1_Prev.shortLong;
    UAPP_WheelDelta_Q2.shortLong = UAPP_OL_Q2.shortLong - UAPP_OL_Q2_Prev.shortLong;
    UAPP_WheelDelta_Q3.shortLong = UAPP_OL_Q3.shortLong - UAPP_OL_Q3_Prev.shortLong;

    //  Report deltas to output buffer if reporting active.
    if( UAPP_Flags.UAPP_ReportActive )
        {
        // Send LR and RR deltas if in 2 wheel or 4 wheel mode.
        SSIO_PutByteTxBufferC( ',' );
        SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q0.nibble3 ]);
        SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q0.nibble2 ]);
        SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q0.nibble1 ]);
        SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q0.nibble0 ]);

        SSIO_PutByteTxBufferC( ',' );
        SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q1.nibble3 ]);
        SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q1.nibble2 ]);
        SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q1.nibble1 ]);
        SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q1.nibble0 ]);

        if( UAPP_Flags.UAPP_4WheelsActive )
            {
            // Send LF and RF deltas only if in 4 wheel mode.
            SSIO_PutByteTxBufferC( ',' );
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q2.nibble3 ]);
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q2.nibble2 ]);
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q2.nibble1 ]);
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q2.nibble0 ]);

            SSIO_PutByteTxBufferC( ',' );
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q3.nibble3 ]);
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q3.nibble2 ]);
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q3.nibble1 ]);
            SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[ UAPP_WheelDelta_Q3.nibble0 ]);
            }
        SSIO_PutByteTxBufferC( ']' );
        SSIO_PutByteTxBufferC( '\n' );
        SSIO_PutByteTxBufferC( '\r' );
        }

    // Save existing values to create deltas next pass.
    UAPP_OL_Q0_Prev = UAPP_OL_Q0;
    UAPP_OL_Q1_Prev = UAPP_OL_Q1;
    UAPP_OL_Q2_Prev = UAPP_OL_Q2;
    UAPP_OL_Q3_Prev = UAPP_OL_Q3;
}

//*******************************************************************************
//
// Task3 - 1.0 sec.

void UAPP_Task3( void )
{
//  LATAbits.LATA4 = LATAbits.LATA4 ^ 1;    // Toggle RA4 for testing
}

//*******************************************************************************
//
// TaskADC - Convert A/D result and do something with it. UNUSED.

void UAPP_TaskADC( void )
{
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

    if( UAPP_Flags.UAPP_MsgEchoActive )             // Echo msg if flag active.
        {
        for( i=0; i<UAPP_IndexRc; i++ )
            if( UAPP_BufferRc[i] != 0x0a && UAPP_BufferRc[i] != 0x0d )
                SSIO_PutByteTxBufferC( UAPP_BufferRc[i] );
  
        SSIO_PutByteTxBufferC( '\n' );
        SSIO_PutByteTxBufferC( '\r' );
        }

    if( UAPP_BufferRc[0] == '[' )
    {
        switch( UAPP_BufferRc[1] ) {

        case 'V':
            UAPP_RomMsgPtr = UAPP_MsgVersion;       // Version message.
            break;  // case 'V'

        case 'B':
            SUTL_InvokeBootloader();                // Bootloader.
            break;  // case 'B'

        case 'D':
            c = UAPP_BufferRc[2];
            if( 'A' <= c && 'Z' >= c )
                {
                UAPP_DeltaCurrent = UAPP_DeltaReload = c & 0x1F;
                UAPP_Flags.UAPP_DeltaActive = 1;        // Delta timing active.
                UAPP_RomMsgPtr = UAPP_MsgDeltaActive;
                }
            else if( '@' == c )
                {
                UAPP_DeltaCurrent = UAPP_DeltaReload = 0;
                UAPP_Flags.UAPP_DeltaActive = 0;        // Delta timing inactive.
                UAPP_RomMsgPtr = UAPP_MsgDeltaInactive;
                }
            else
                UAPP_RomMsgPtr = UAPP_MsgDeltaHelp;     // Delta timing help message.
            break;  // case 'D'

        case 'E':
            UAPP_Flags.UAPP_MsgEchoActive ^= 1;     // Toggle whether to echo msgs.
            if( UAPP_Flags.UAPP_MsgEchoActive )     // Echo msg if flag active.
                UAPP_RomMsgPtr = UAPP_MsgEchoActive;
            else                                    // Don't echo msg if flag inactive.
                UAPP_RomMsgPtr = UAPP_MsgEchoInactive;
            break;  // case 'E'

        case 'F':
            switch( UAPP_BufferRc[2] ) {
            case '1':
                UAPP_Flags.UAPP_Frequency20Hz = 0;      // Report data at 10 Hz.
                UAPP_RomMsgPtr = UAPP_MsgFrequency10Hz;
                break;  // case '1'
            case '2':
                UAPP_Flags.UAPP_Frequency20Hz = 1;      // Report data at 20 Hz.
                UAPP_RomMsgPtr = UAPP_MsgFrequency20Hz;
                break;  // case '2'
            default:
                UAPP_RomMsgPtr = UAPP_MsgFreqHelp;      // Frequency help message.
                break;
            };  // switch( UAPP_BufferRc[2] )
            break;  // case 'F'

        case 'H':
            UAPP_RomMsgPtr = UAPP_MsgHelp;              // Help message.
            break;  // case 'H'

        case 'L':
            switch( UAPP_BufferRc[2] ) {
            case '0':
                UAPP_Flags.UAPP_TestLEDActive = 0;      // Test LED off.
                UAPP_RomMsgPtr = UAPP_MsgTestLEDInactive;
                UAPP_WriteVideoMuxTestLED();
                break;  // case '0'
            case '1':
                UAPP_Flags.UAPP_TestLEDActive = 1;      // Test LED on.
                UAPP_RomMsgPtr = UAPP_MsgTestLEDActive;
                UAPP_WriteVideoMuxTestLED();
                break;  // case '1'
            default:
                UAPP_RomMsgPtr = UAPP_MsgTestLEDHelp;   // Test LED help message.
                break;
            };  // switch( UAPP_BufferRc[2] )
            break;  // case 'L'

        case 'M':
            switch( UAPP_BufferRc[2] ) {
            case '0':                                   // ASCII mode.
            case 0x00:                                  // Legacy mode.
                UAPP_VideoMuxSelectBits.nibble0 = 0x0;
                UAPP_RomMsgPtr = UAPP_MsgMux0;
                UAPP_WriteVideoMuxTestLED();            // Write video mux select.
                break;
            case '1':                                   // ASCII mode.
            case 0x01:                                  // Legacy mode.
                UAPP_VideoMuxSelectBits.nibble0 = 0x1;
                UAPP_RomMsgPtr = UAPP_MsgMux1;
                UAPP_WriteVideoMuxTestLED();            // Write video mux select.
                break;
            case '2':                                   // ASCII mode.
            case 0x02:                                  // Legacy mode.
                UAPP_VideoMuxSelectBits.nibble0 = 0x2;
                UAPP_RomMsgPtr = UAPP_MsgMux2;
                UAPP_WriteVideoMuxTestLED();            // Write video mux select.
                break;
            case '3':                                   // ASCII mode.
            case 0x03:                                  // Legacy mode.
                UAPP_VideoMuxSelectBits.nibble0 = 0x3;
                UAPP_RomMsgPtr = UAPP_MsgMux3;
                UAPP_WriteVideoMuxTestLED();            // Write video mux select.
                break;
            case '4':                                   // ASCII mode.
            case 0x04:                                  // Legacy mode.
                UAPP_VideoMuxSelectBits.nibble0 = 0x4;
                UAPP_RomMsgPtr = UAPP_MsgMux4;
                UAPP_WriteVideoMuxTestLED();            // Write video mux select.
                break;
            case '5':                                   // ASCII mode.
            case 0x05:                                  // Legacy mode.
                UAPP_VideoMuxSelectBits.nibble0 = 0x5;
                UAPP_RomMsgPtr = UAPP_MsgMux5;
                UAPP_WriteVideoMuxTestLED();            // Write video mux select.
                break;
            case '6':                                   // ASCII mode.
            case 0x06:                                  // Legacy mode.
                UAPP_VideoMuxSelectBits.nibble0 = 0x6;
                UAPP_RomMsgPtr = UAPP_MsgMux6;
                UAPP_WriteVideoMuxTestLED();            // Write video mux select.
                break;
            case '7':                                   // ASCII mode.
            case 0x07:                                  // Legacy mode.
                UAPP_VideoMuxSelectBits.nibble0 = 0x7;
                UAPP_RomMsgPtr = UAPP_MsgMux7;
                UAPP_WriteVideoMuxTestLED();            // Write video mux select.
                break;
            case 0x1F:
                UAPP_Flags.UAPP_TestLEDActive = 0;      // Test LED off, legacy mode.
                UAPP_RomMsgPtr = UAPP_MsgTestLEDActive;
                UAPP_WriteVideoMuxTestLED();
                break;  // case 0x1F
            case 0x20:
                UAPP_Flags.UAPP_TestLEDActive = 1;      // Test LED on, legacy mode.
                UAPP_RomMsgPtr = UAPP_MsgTestLEDInactive;
                UAPP_WriteVideoMuxTestLED();
                break;  // case 0x20
            case 0xB2:
                Reset();                                // Force processor reset, legacy mode.
                break;  // case 0xB2
            default:
                UAPP_RomMsgPtr = UAPP_MsgMuxHelp;       // Mux help message.
                break;
            };  // switch( UAPP_BufferRc[2] )
            break;  // case 'M'

        case 'R':
            UAPP_Flags.UAPP_ReportActive ^= 1;      // Toggle whether to report data.
            if( UAPP_Flags.UAPP_ReportActive )      // Report data if flag active.
                UAPP_RomMsgPtr = UAPP_MsgReportActive;
            else                                    // Don't report data if flag inactive.
                UAPP_RomMsgPtr = UAPP_MsgReportInactive;
            break;  // case 'R'

        case 'S':
            c = UAPP_BufferRc[2];
            if( '1' <= c && '8' >= c )
                {
                UAPP_PWPrescale = (c & 0x0F) - 1;       // Set Prescaler value.
                UAPP_RomMsgPtr = UAPP_MsgPrescaler;
                }
            else
                UAPP_RomMsgPtr = UAPP_MsgPrescalerHelp; // Prescaler help message.
            break;  // case 'S'

        case 'U':
            switch( UAPP_BufferRc[2] ) {
            case '0':
                UAPP_PWStateMachineDisable();            // Disable pulse width timing.
                UAPP_RomMsgPtr = UAPP_MsgDisableTiming;
                break;  // case '0'
            case '1':
                UAPP_PWStateMachineEnable();             // Enable pulse width timing.
                UAPP_RomMsgPtr = UAPP_MsgEnableTiming;
                break;  // case '1'
            default:
                UAPP_RomMsgPtr = UAPP_MsgTimingHelp;     // Pulse width timing help message.
                break;
            };  // switch( UAPP_BufferRc[2] )
            break;  // case 'U'

        case 'W':
            switch( UAPP_BufferRc[2] ) {
            case '2':
                UAPP_Flags.UAPP_4WheelsActive = 0;      // 2 wheels active.
                UAPP_RomMsgPtr = UAPP_Msg2Wheels;
                break;  // case '2'
            case '4':
                UAPP_Flags.UAPP_4WheelsActive = 1;      // 4 wheels active.
                UAPP_RomMsgPtr = UAPP_Msg4Wheels;
                break;  // case '4'
            default:
                UAPP_RomMsgPtr = UAPP_MsgWheelsHelp;    // Wheels help message.
                break;
            };  // switch( UAPP_BufferRc[2] )
            break;  // case 'W'

        case 'Z':
            UAPP_InitSQEN();
            UAPP_OL_Q0_Prev.shortLong = 0;      // Reset saved OL values.
            UAPP_OL_Q1_Prev.shortLong = 0;      // Reset saved OL values.
            UAPP_OL_Q2_Prev.shortLong = 0;      // Reset saved OL values.
            UAPP_OL_Q3_Prev.shortLong = 0;      // Reset saved OL values.
            UAPP_RomMsgPtr = UAPP_MsgReset;     // Msg: Reset SQEN.
            break;  // case 'Z'

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
// Init SQEN by resetting channels and setting counting modes.

void UAPP_InitSQEN()
{
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
}

//*******************************************************************************
//
// Select input to Video Mux by clocking select bits to 74H174 hex latch.
// Select input to Test LED bit and unused testing bit
// by clocking select bits to 74H174 hex latch.

void UAPP_WriteVideoMuxTestLED()
{
    TRISBbits.TRISB7 = 0;                           //  Set up port bits for writing.
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB5 = 0;
    TRISBbits.TRISB4 = 0;
    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA5 = 0;

    // Setup latch data lines.
    LATBbits.LATB7 = UAPP_VideoMuxSelectBits.bit0;  // Valid mux select bit.
    LATBbits.LATB6 = UAPP_VideoMuxSelectBits.bit1;  // Valid mux select bit.
    LATBbits.LATB5 = UAPP_VideoMuxSelectBits.bit2;  // Valid mux select bit.
    LATBbits.LATB4 = 0;                             // Always write INH non-active.
    LATAbits.LATA4 = 0;                             // Otherwise unused, could show timing.
    LATAbits.LATA5 = UAPP_Flags.UAPP_TestLEDActive; // Test LED.

    LATAbits.LATA0  = 0b0;                  // Drive WR low to allow write to 74H174 hex latch.

    _asm
    NOP                                     // Delay 200ns to ensure setup time for data.
    NOP
    _endasm

    LATAbits.LATA0  = 0b1;                  // Drive WR high to clock data into 74H174 hex latch.
}

//*******************************************************************************
//
// Set up variables and INT0 and INT1 to enable pulse width timing.

void UAPP_PWStateMachineEnable()
{
// Initialize state machine variables.
    UAPP_PWState = 0x00;                // Init state machine.
    UAPP_PWPulse0.word = 0;             // Clear pulse width 0.
    UAPP_PWPulse1.word = 0;             // Clear pulse width 1.
    UAPP_PWPassesWithoutInt0 = 0;       // Clear passes counter 0;
    UAPP_PWPassesWithoutInt1 = 0;       // Clear passes counter 1;

// Note T0CONbits.TMR0ON = 1; (Start timer) happens in
//  UAPP_ISR_PWStateMachineInt0 and/or UAPP_ISR_PWStateMachineInt1.

    UAPP_Flags.UAPP_PWTimingActive = 1; // Pulse width timing enabled.

// Initialize HW registers to support INT0 and INT1 (leave OR'ed RBIF disabled).
    INTCON2bits.INTEDG0 = 1;    // INT0 on rising edge.
    INTCON2bits.INTEDG1 = 1;    // INT1 on rising edge.

    INTCONbits.INT0IF = 0;      // Clear any existing INT0 interrupts.
    INTCON3bits.INT1IF = 0;     // Clear any existing INT1 interrupts.

    INTCONbits.INT0IE = 1;      // Enable INT0 interrupts.
    INTCON3bits.INT1IE = 1;     // Enable INT1 interrupts.
}

//*******************************************************************************
//
// Disable pulse width timing.

void UAPP_PWStateMachineDisable()
{
    INTCONbits.INT0IE = 0;      // Disable INT0 interrupts.
    INTCON3bits.INT1IE = 0;     // Disable INT1 interrupts.

    T0CONbits.TMR0ON = 1;       // Stop timer.

    UAPP_Flags.UAPP_PWTimingActive = 0;     // Pulse width timing disabled.
}

//*******************************************************************************
//
// Select next UAPP_WheelModeState using current UAPP_WheelModeState.
// Three different routines to handle mainline and two interrupt calls.

void UAPP_PWStateMachineMain()
{
    switch( UAPP_PWState ) {
    case 0x00:
    case 0x01:
        if( 40 < UAPP_PWPassesWithoutInt0 && 40 > UAPP_PWPassesWithoutInt1 )
            UAPP_PWState = 0x02;            // Force state change.
        break;  // cases 0x00 or 0x01
    case 0x02:
    case 0x03:
        if( 40 < UAPP_PWPassesWithoutInt1 && 40 > UAPP_PWPassesWithoutInt0 )
            UAPP_PWState = 0x00;            // Force state change.
        break;  // cases 0x02 or 0x03
    default:
        break;
    };  // switch( UAPP_PWState )

    if( 40 < UAPP_PWPassesWithoutInt0 )
        UAPP_PWPulse0.word = 0xFFFF;
    else
        UAPP_PWPassesWithoutInt0++;

    if( 40 < UAPP_PWPassesWithoutInt1 )
        UAPP_PWPulse1.word = 0xFFFF;
    else
        UAPP_PWPassesWithoutInt1++;
}

void UAPP_ISR_PWStateMachineInt0()
{
    switch( UAPP_PWState ) {
    case 0x00:
        // POR defaults: 16-bit timer, internal instruction clock, use pre-scaler.
        T0CONbits.T0PS = UAPP_PWPrescale;   // Set Timer 0 prescaler.
        TMR0H = 0;                          // Reset timer high byte FIRST.
        TMR0L = 0;                          // Reset timer low byte.
        T0CONbits.TMR0ON = 1;               // Start timer.
        UAPP_PWState = 0x01;                // Update state machine.
        break;  // case 0x00
    case 0x01:
        if( 1 == INTCONbits.TMR0IF )        // If Timer 0 overflowed..
            {
            INTCONbits.TMR0IF = 0;          // ..then reset overflow.
            UAPP_PWPulse0.word = 0xFFFF;    // Clip timer value if maxed out.
            }
        else
            {                               // ..else no Timer 0 overflow.
            UAPP_PWPulse0.byte0 = TMR0L;    // Read timer register low byte FIRST.
            UAPP_PWPulse0.byte1 = TMR0H;    // Read timer register high byte.
            }
        TMR0H = 0;                          // Reset timer high byte FIRST.
        TMR0L = 0;                          // Reset timer low byte.
        UAPP_PWState = 0x02;                // Update state machine.
        break;  // case 0x01
    default:
        break;
    };  // switch( UAPP_PWState )

    UAPP_PWPassesWithoutInt0 = 0;
}

void UAPP_ISR_PWStateMachineInt1()
{
    switch( UAPP_PWState ) {
    case 0x02:
        // POR defaults: 16-bit timer, internal instruction clock, use pre-scaler.
        T0CONbits.T0PS = UAPP_PWPrescale;   // Set prescaler.
        TMR0H = 0;                          // Reset timer high byte FIRST.
        TMR0L = 0;                          // Reset timer low byte.
        T0CONbits.TMR0ON = 1;               // Start timer.
        UAPP_PWState = 0x03;                // Update state machine.
        break;  // case 0x02
    case 0x03:
        if( 1 == INTCONbits.TMR0IF )        // If Timer 0 overflowed..
            {
            INTCONbits.TMR0IF = 0;          // ..then reset overflow.
            UAPP_PWPulse1.word = 0xFFFF;    // Clip timer value if maxed out.
            }
        else
            {                               // ..else no Timer 0 overflow.
            UAPP_PWPulse1.byte0 = TMR0L;    // Read timer register low byte FIRST.
            UAPP_PWPulse1.byte1 = TMR0H;    // Read timer register high byte.
            }
        TMR0H = 0;                          // Reset timer high byte FIRST.
        TMR0L = 0;                          // Reset timer low byte.
        UAPP_PWState = 0x00;                // Update state machine.
        break;  // case 0x03
    default:
        break;
    };  // switch( UAPP_PWState )

    UAPP_PWPassesWithoutInt1 = 0;
}
