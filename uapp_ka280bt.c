//*******************************************************************************
// tinyRTX Filename: uapp_ka280bt.c (User APPlication for KA 280B board - Transmission)
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
//              Converted from PIC18F452 to PIC18F2620.
//  29Apr15  Stephen_Higgins@KairosAutonomi.com
//              Added support for 2010 PICDEM2+ demo board (no 4 Mhz crystal).
//  05May15  Stephen_Higgins@KairosAutonomi.com
//              Added support for Kairos Autonomi 280B board.
//  14May15  Stephen_Higgins@KairosAutonomi.com  
//              Substitute #include <ucfg.inc> for <p18f452.inc>.
//  20May15  Stephen_Higgins@KairosAutonomi.com  
//              Fix UAPP_Timer1Init by adding terminating return.
//  28May15 Stephen_Higgins@KairosAutonomi
//              Move most logic from SUSR.asm to here in preparation
//              of turning this into UAPP.C.
//  29May15 Stephen_Higgins@KairosAutonomi
//              Create uapp_pd2p.asm from uapp.asm to support PICdem2+ boards.
//              Internal names are all still UAPP_xxx.
//              Create uapp_ka280b.c from uapp_ka280b.asm to allow user apps in C.
//  01Jun15 Stephen_Higgins@KairosAutonomi
//              Implement 280B board app specifics.
//  09Jun15 Stephen_Higgins@KairosAutonomi
//              Add background task for measuring PWM on pin 5 RA3.
//  22Jun15 Stephen_Higgins@KairosAutonomi
//              Add UAPP_ClearRxBuffer(), UAPP_PutByteRxBuffer(), UAPP_ParseRxMsg().
//  09Jul15 Stephen_Higgins@KairosAutonomi
//              Process input messages, split UCFG_DJPCB_280B into
//              UCFG_KA280BI (I/O) and UCFG_KA280BT (Transmission).
//  10Jul15 Stephen_Higgins@KairosAutonomi
//              Convert from uapp_ka280b.c to uapp_ka280bt.c (Transmission).
//  22Jul15 Stephen_Higgins@KairosAutonomi
//              Add handlers for input messages "Q" and "B".
//              Moved all #config to ucfg.h as CONFIG needed in ucfg.inc by SBTL.
//  30Jul15 Stephen_Higgins@KairosAutonomi.com
//              Combine UCFG_KA280BI and UCFG_KA280BT into UCFG_DJPCB_280B.
//  18Aug15 Stephen_Higgins@KairosAutonomi.com
//              Remove extern prototypes, already in uapp.h, not needed herein.
//  27Aug15 Stephen_Higgins@KairosAutonomi.com
//              Change SRTX timebase from 50ms (20 Hz) to 10ms (100 Hz).
//  04Feb17 Stephen_Higgins@KairosAutonomi.com
//              Change all ..Rx.. names to ..Rc.. for consistency with USIO_MsgReceived.
//  07Feb17 Stephen_Higgins@KairosAutonomi.com
//              Fix background task for measuring PWM to measure time high.
//              Invert UAPP_InputBits to account for active low.
//              Change Q msg to V msg for consistency with other KA apps.
//              Remove [BB] bootloader invocation msg.
//  22Feb17 Stephen_Higgins@KairosAutonomi.com
//              Disable all interrupts in UAPP_POR_Init_PhaseA().
//              Call FindDesiredGear() in Task2().
//              If PWM signal removed set measured PWM width = 0.
//
//*******************************************************************************
//
//   UCFG_KA280BT specified.
//   ***********************
//
// Hardware: Kairos Autonomi 280B circuit board WITH Transmission option.
//           Microchip PIC18F2620 processor with 10 MHz input resonator.
//
//  Functions:
//      4 discrete inputs, if exactly one is active that sets the output gear.
//      4 discrete outputs, set by PWM width or calculated from discrete inputs.
//      No analog inputs.
//      PWM input, denotes PRNDL position (see UAPP_Task1()).
//      Init (Version) message "[Digital Trans v2.0.0 280BT 20150723]" (or whatever date).
//      Processes input messages: (c is checksum)
//      "[VV]"  responds by sending Version message
//      Creates status message when gear changes due to PWM or discrete inputs.
//
// Complete PIC18F2620 (28-pin device) pin assignments for KA board 280B (Transmission option):
//
//  1) MCLR*/Vpp/RE3             = Reset/Programming connector(1): ATN TTL (active low)
//  2) RA0/AN0                   = Analog In: AIN0 (not used)
//  3) RA1/AN1                   = Analog In: AIN1 (not used)
//  4) RA2/AN2/Vref-/CVref       = Analog In: AIN2 (not used)
//  5) RA3/AN3/Vref+             = Discrete In: RA3 (PWM input)
//  6) RA4/T0KI/C1OUT            = Discrete Out: DOUTI0 "Park" output pin (RX TTL on native board)
//  7) RA5/AN4/SS*/HLVDIN/C2OUT  = Discrete Out: DOUTI1 "Reverse" output pin (TX TTL on native board)
//  8) Vss                       = Programming connector(3) (Ground)
//
//   External 10 Mhz ceramic oscillator installed in pins 9, 10; KA board 280B.
//
//  9) OSC1/CLKIN/RA7        = 10 MHz clock in (10 MHz * 4(PLL)/4 = 10 MHz = 0.1us instr cycle)
// 10) OSC2/CLKOUT/RA6       = 10 MHz clock out (non-configurable output)
//
// 11) RC0/T1OSO/T13CKI      = Discrete Out: DOUTI7 (not used)
// 12) RC1/T1OSI/CCP2        = Discrete Out: DOUTI6 (not used)
// 13) RC2/CCP1              = Discrete Out: DOUTI5 (not used)
// 14) RC3/SCK/SCL           = Discrete Out: DOUTI4 (not used) (Not used for SPI or I2C.)
// 15) RC4/SDI/SDA           = Discrete Out: DOUTI3 "Drive" output pin (Not used for SPI or I2C.)
// 16) RC5/SDO               = Discrete Out: DOUTI2 "Neutral" output pin (Not used for SPI.)
// 17) RC6/TX/CK             = Discrete Out, USART TX (RS-232): (DOUTI1 on native board)
//                               USART control of this pin requires pin as Discrete In.
// 18) RC7/RX/DT             = Discrete Out, USART RX (RS-232): (DOUTI0 on native board)
//                               USART control of this pin requires pin as Discrete In.
// 19) Vss                   = Programming connector(3) (Ground)
// 20) Vdd                   = Programming connector(2) (+5 VDC)
// 21) RB0/INT0/FLT0/AN12    = Discrete In: DIN7 (not used)
// 22) RB1/INT1/AN10         = Discrete In: DIN6 (not used)
// 23) RB2/INT2/AN8          = Discrete In: DIN5 (not used)
// 24) RB3/AN9/CCP2          = Discrete In: DIN4 (not used)
// 25) RB4/KB10/AN11         = Discrete In: DIN3 "Drive" input pin
// 26) RB5/KB11/PGM          = Discrete In: DIN2 "Neutral" input pin
// 27) RB6/KB12/PGC          = Discrete In: DIN1 "Reverse" input pin also Programming connector(5) (PGC)
//                               ICD2 control of this pin requires pin as Discrete In.
// 28) RB7/KB13/PGD          = Discrete In: DIN0 "Park" input pin also Programming connector(4) (PGD)
//                               ICD2 control of this pin requires pin as Discrete In.
//
// 	Jumper settings where: (DP) = DiosPro default, (KA) = KA F2620 SW default
//      (Note: SWRX may be labeled as ProgRX, SWTX may be labeled as ProgTX)
//      JP1: local SWTX (see JP5)
//          1-2: chip (pin 7) P16DI (DP) (KA)
//          2-3: chip (pin 25) P3DI
//      JP2: target (pin 6) RS-232 RX
//          1-2: chip (pin 6) SWRX (DP)
//          2-3: chip (pin 18) HWRX (KA)
//      JP3: target (pin 7) RS-232 TX
//          1-2: chip (pin 7) SWTX (DP)
//          2-3: chip (pin 17) HWTX (KA)
//      JP4: target (pin 18) DOUT0
//          1-2: chip (pin 18) HWRX (DP)
//          2-3: chip (pin 6) SWRX (KA)
//      JP5: target (pin 17) DOUT1
//          1-2: chip (pin 17) HWTX (DP)
//          2-3: local SWTX (from JP1) (KA)
//      JP6: chip PGC/PGD (pins 27/28)
//          1-2: ICD3 PGC/PGD (CON8 pins 5/4) (enable ICSP) (KA)
//          n/c: target P0/P1 (pins 27/28) (native operation) (DP)
//
//*******************************************************************************

#include "ucfg.h"          // includes processor definitions.
#include "uapp_ucfg.h"     // includes #config definitions, only include in uapp*.c.
#include "si2c.h"
#include "ssio.h"
#include "sutl.h"
#include "uadc.h"
#include "usio.h"

//  Internal prototypes.

void UAPP_U_Msg( void );
void UAPP_ReadDiscreteInputs( void );
void UAPP_WriteDiscreteOutputs( void );
void UAPP_ClearRcBuffer( void );
void UAPP_FindDesiredGear( void );

//  Constants.

#define FALSE 0
#define TRUE  0xFF

//  Count of .1us periods within which input PWM signifies PRNDL position.

const int UAPP_PulseLength_PrkHi = 0x4E5D +0x100;   //  20061 + 256 = 2031.7 us
const int UAPP_PulseLength_PrkLo = 0x4E5D -0x100;   //  20061 - 256 = 1980.5 us
const int UAPP_PulseLength_RevHi = 0x4075 +0x100;   //  16501 + 256 = 1675.7 us
const int UAPP_PulseLength_RevLo = 0x4075 -0x100;   //  16501 - 256 = 1624.5 us
const int UAPP_PulseLength_NeuHi = 0x35FD +0x100;   //  13821 + 256 = 1407.7 us
const int UAPP_PulseLength_NeuLo = 0x35FD -0x100;   //  13821 - 256 = 1356.5 us
const int UAPP_PulseLength_DrvHi = 0x2B85 +0x100;   //  11141 + 256 = 1139.7 us
const int UAPP_PulseLength_DrvLo = 0x2B85 -0x100;   //  11141 - 256 = 1088.5 us

//  String literals.

const char UAPP_MsgVersion[] = "[Digital Trans v2.1.1 280BT 20170224]\n\r";
const char UAPP_MsgEnd[] = "]\n\r";
const unsigned char UAPP_Nibble_ASCII[] = "0123456789ABCDEF";

//  Variables.

#pragma udata   UAPP_UdataSec

SUTL_Byte UAPP_InputBits;
SUTL_Byte UAPP_OutputBits;

// Internal variables to compute PRNDL from PWM.

SUTL_Word UAPP_PWM_Timer0;
SUTL_Word UAPP_PWM_DetectedCount;
unsigned char UAPP_PWM_Gear;

typedef enum {  UAPP_PWM_Init,
                UAPP_PWM_Ready,
                UAPP_PWM_Acquire
             } UAPP_PWM_State_type;

UAPP_PWM_State_type UAPP_PWM_State;

// Internal variables to manage receive buffer.

#define UAPP_BUFFERLENGTH 80

unsigned char UAPP_BufferRc[UAPP_BUFFERLENGTH];
unsigned char UAPP_IndexRc;

//*******************************************************************************
//
// User APP defines.
//
#define UAPP_OSCCON_VAL  0x64
//
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
// bit 5 : RA5/AN4/SS*/HLVDIN/C2OUT  : 0 : DiosPro TX TTL: (Discrete Out to replace DOUTI0)
// bit 4 : RA4/T0KI/C1OUT            : 0 : DiosPro RX TTL: (Discrete Out to replace DOUTI1)
// bit 3 : RA3/AN3/Vref+             : 0 : RA3 input (PWM input) 
// bit 2 : RA2/AN2/Vref-/CVref       : 0 : (unused, configured as RA2 input) (don't care)
// bit 1 : RA1/AN1                   : 0 : (unused, configured as RA1 input) (don't care)
// bit 0 : RA0/AN0                   : 0 : (unused, configured as RA0 input) (don't care)
//
#define UAPP_TRISA_VAL  0x0F
//
// Set all PORTA to don't care, outputs, and inputs.
//
// bit 7 : TRISA7 : 0 : Using OSC1, overridden by CONFIG1H (don't care)
// bit 6 : TRISA6 : 0 : Using OSC2, overridden by CONFIG1H (don't care)
// bit 5 : DDRA5  : 0 : Discrete Out
// bit 4 : DDRA4  : 0 : Discrete Out
// bit 3 : DDRA3  : 1 : Discrete In
// bit 2 : DDRA2  : 1 : Discrete In
// bit 1 : DDRA1  : 1 : Discrete In
// bit 0 : DDRA0  : 1 : Discrete In
//
#define UAPP_PORTB_VAL  0x00
//
// PORTB cleared so any bits later programmed as output initialized to 0.
//
// bit 7 : RB7/PGD               : 0 : Discrete In: (don't care)
// bit 6 : RB6/PGC               : 0 : Discrete In: (don't care)
// bit 5 : RB5/KB11/PGM          : 0 : Discrete In: (don't care)
// bit 4 : RB4/KB10/AN11         : 0 : Discrete In: (don't care)
// bit 3 : RB3/AN9/CCP2          : 0 : Discrete In: (don't care)
// bit 2 : RB2/INT2/AN8          : 0 : Discrete In: (don't care)
// bit 1 : RB1/INT1/AN10         : 0 : Discrete In: (don't care)
// bit 0 : RB0/INT0/FLT0/AN12    : 0 : Discrete In: (don't care)
//
#define UAPP_TRISB_VAL  0xFF
//
// Set TRISB RB0-RB7 to inputs.  PGC and PGD need to be configured as high-impedance inputs.
//
// bit 7 : DDRB7  : 1 : Discrete In
// bit 6 : DDRB6  : 1 : Discrete In
// bit 5 : DDRB5  : 1 : Discrete In
// bit 4 : DDRB4  : 1 : Discrete In
// bit 3 : DDRB3  : 1 : Discrete In
// bit 2 : DDRB2  : 1 : Discrete In
// bit 1 : DDRB1  : 1 : Discrete In
// bit 0 : DDRB0  : 1 : Discrete In
//
#define UAPP_PORTC_VAL  0x00
//
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
//
#define UAPP_TRISC_VAL  0xC0
//
// Set TRISC to inputs for only USART RX/TX.  All others are outputs.
//
// bit 7 : DDRC7  : 1 : Discrete In, USART RX: RS-232 driver, USART control of this pin requires pin as Discrete In.
// bit 6 : DDRC6  : 1 : Discrete In, USART TX: RS-232 driver, USART control of this pin requires pin as Discrete In.
// bit 5 : DDRC5  : 0 : Discrete Out: DOUT12 (Not used for SPI.)
// bit 4 : DDRC4  : 0 : Discrete Out: DOUT13 (Not used for SPI.) (Not used for I2C.)
// bit 3 : DDRC3  : 0 : Discrete Out: DOUT14 (Not used for SPI.) (Not used for I2C.)
// bit 2 : DDRC2  : 0 : Discrete Out: DOUT15
// bit 1 : DDRC1  : 0 : Discrete Out: DOUT16
// bit 0 : DDRC0  : 0 : Discrete Out: DOUT17
//
#define UAPP_T1CON_VAL  0x30
//
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
//
#define UAPP_TMR1L_VAL  0x2C
#define UAPP_TMR1H_VAL  0xCF
//
// 40 Mhz Fosc/4 is base clock = 10 Mhz = 0.1 us per clock.
// 1:8 prescale = 0.1 * 8 = 0.8 us per clock.
// 12,500 counts * 0.8us/clock = 10,000 us/rollover = 10ms/rollover.
// Timer preload value = 65,536 - 12,500 = 53,036 = 0xCF2C.
//
#define UAPP_T0CON_VAL  0x08
//
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
#define UAPP_INTCON_OFF  0x00
//
// INTCON changed: GIE, PEIE, TMR0IE, INT0IE, RBIE disabled; TMR0IF, INT0IF, RBIF cleared.
//
// bit 7 : GIE/GIEH  : 0 : Disables all unmasked interrupts
// bit 6 : PEIE/GIEL : 0 : Disables all unmasked peripheral interrupts
// bit 5 : TMR0IE    : 0 : Disables the TMR0 overflow interrupt
// bit 4 : INT0IE    : 0 : Disables the INT0 external interrupt
// bit 3 : RBIE      : 0 : Disables the RB port change interrupt
// bit 2 : TMR0IF    : 0 : TMR0 register did not overflow
// bit 1 : INT0IF    : 0 : The INT0 external interrupt did not occur
// bit 0 : RBIF      : 0 : None of the RB7:RB4 pins have changed state
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

#pragma code    UAPP_CodeSec

//*******************************************************************************
//
// User application Power-On Reset initialization.
//
void UAPP_POR_Init_PhaseA( void )
{
    // GIE, PEIE, TMR0IE, INT0IE, RBIE disabled; TMR0IF, INT0IF, RBIF cleared.
    INTCON = UAPP_INTCON_OFF;
    OSCCON = UAPP_OSCCON_VAL;   // Configure Fosc. Note relation to CONFIG1H.
}

//*******************************************************************************
//
// User application Power-On Reset initialization.
//
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

    UAPP_ClearRcBuffer();   // Clear UAPP_BufferRc before messages can arrive.
    USIO_Init();            // User Serial I/O hardware init.

    SSIO_PutStringTxBuffer( (char*) UAPP_MsgVersion );  // Version message.

    // Init for measuring PWM.

    T0CON = UAPP_T0CON_VAL;         // Initialize Timer0 but don't start it.
    UAPP_PWM_State = UAPP_PWM_Init; // Init PWM measurement state.
    UAPP_PWM_Gear = 'U';            // Init computed gear to Unknown.
    UAPP_PWM_Timer0.word = 0;       // Avoid bad data if no PWM signal.
}

//*******************************************************************************
//
// Init Timer1 module to generate timer interrupt every 50ms.
//
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
//  Background Task: Use Timer0 to measure high PWM signal in the background.
//
//  In a perfect world the PWM signal would be connected to a CCP pin.
//  Slightly less perfect it would be connected to an Int on Level Change pin.
//  In our real world it comes in on just a digital input pin, so the only thing
//  we can do is poll it and approximately measure the time it is in the high state.
//
void UAPP_BkgdTask( void )
{
unsigned char PWM_Low;

    // Capture state so we don't care if it changes during execution.
    if( PORTAbits.RA3 == 0 )
        PWM_Low = TRUE;
    else
        PWM_Low = FALSE;

    switch( UAPP_PWM_State )
    {
        // If we are still in init, wait until current high pulse ends.
        case UAPP_PWM_Init:
            if( PWM_Low )
            {
                UAPP_PWM_State = UAPP_PWM_Ready;
            }
            break;

        //  Last high pulse ended, now look for start of next high pulse.
        case UAPP_PWM_Ready:
            if( !PWM_Low )
            {
                TMR0H = 0;              // Must be done before write of TMR0L to latch high byte.
                TMR0L = 0;              // Set Timer0 = 0;
                T0CONbits.TMR0ON = 1;   // Turn on Timer0;
                UAPP_PWM_State = UAPP_PWM_Acquire;
            }
            break;

        // Wait until current high pulse ends, then grab elapsed time.
        case UAPP_PWM_Acquire:
            if( PWM_Low )
            {
                T0CONbits.TMR0ON = 0;           // Turn off Timer0;
                UAPP_PWM_Timer0.byte0 = TMR0L;  // Must be done before read of TMR0H to latch high byte.
                UAPP_PWM_Timer0.byte1 = TMR0H;  // Read timer0 value.
                UAPP_PWM_State = UAPP_PWM_Ready;
                UAPP_PWM_DetectedCount.word++;  // Count PWM pulse so we know they exist.
            }
            break;
    }   // switch( UAPP_PWM_State )
}

//*******************************************************************************
//
// Task1 - 10 ms.
//
void UAPP_Task1( void )
{
}

//*******************************************************************************
//
// Task2 - 100 ms. UNUSED.
//
void UAPP_Task2( void )
{
    if( 0 == UAPP_PWM_DetectedCount.word )
        UAPP_PWM_Timer0.word = 0;           // Delete old data if no PWM signal.
    else
        UAPP_PWM_DetectedCount.word = 0;    // Reset count of PWM pulses.

    UAPP_FindDesiredGear();     // Find desired gear from discrete inputs or PWM.
}

//*******************************************************************************
//
// Task3 - 1.0 sec. UNUSED.
//
void UAPP_Task3( void )
{
}

//*******************************************************************************
//
// UAPP_FindDesiredGear - Find desired gear from discrete inputs or PWM.
//
void UAPP_FindDesiredGear( void )
{
unsigned char UAPP_PWM_GearTemp;

    UAPP_PWM_GearTemp = UAPP_PWM_Gear;  // Save old gear to check for change.

    UAPP_OutputBits.byte = 0;   // Set all internal copies of outputs to low.
    UAPP_ReadDiscreteInputs();  // Read discrete inputs into UAPP_InputBits.

    // Find PRNDL.

    switch( UAPP_InputBits.nibble0 )    // Switch based on which DIN is active.
    {
        // Attempt to find PRNDL based on single active DIN.

        case 0x1:                       // DIN3 low , DIN2 low , DIN1 low , DIN0 high. 
            UAPP_OutputBits.bit0 = 1;   // Set DOUTI0 "Park" output pin active.
            UAPP_PWM_Gear = 'P';        // Set msg char.
            break;

        case 0x2:                       // DIN3 low , DIN2 low , DIN1 high, DIN0 low. 
            UAPP_OutputBits.bit1 = 1;   // Set DOUTI1 "Reverse" output pin active.
            UAPP_PWM_Gear = 'R';        // Set msg char.
            break;

        case 0x4:                       // DIN3 low , DIN2 high, DIN1 low , DIN0 low. 
            UAPP_OutputBits.bit2 = 1;   // Set DOUTI2 "Neutral" output pin active.
            UAPP_PWM_Gear = 'N';        // Set msg char.
           break;

        case 0x8:                       // DIN3 high, DIN2 low , DIN1 low , DIN0 low. 
            UAPP_OutputBits.bit3 = 1;   // Set DOUTI3 "Drive" output pin active.
            UAPP_PWM_Gear = 'D';        // Set msg char.
           break;

        // No single DIN active, instead we must use measured PWM to find PRNDL.

        default:
            if( UAPP_PWM_Timer0.word >= UAPP_PulseLength_PrkLo &&
                UAPP_PWM_Timer0.word <= UAPP_PulseLength_PrkHi )
                {
                UAPP_PWM_Gear = 'P';        // Set msg char.
                UAPP_OutputBits.bit0 = 1;   // Set DOUTI0 "Park" output pin active.
                }
            else if( UAPP_PWM_Timer0.word >= UAPP_PulseLength_RevLo &&
                     UAPP_PWM_Timer0.word <= UAPP_PulseLength_RevHi )
                {
                UAPP_PWM_Gear = 'R';        // Set msg char.
                UAPP_OutputBits.bit1 = 1;   // Set DOUTI1 "Reverse" output pin active.
                }
            else if( UAPP_PWM_Timer0.word >= UAPP_PulseLength_NeuLo &&
                     UAPP_PWM_Timer0.word <= UAPP_PulseLength_NeuHi )
                {
                UAPP_PWM_Gear = 'N';        // Set msg char.
                UAPP_OutputBits.bit2 = 1;   // Set DOUTI2 "Neutral" output pin active.
                }
            else if( UAPP_PWM_Timer0.word >= UAPP_PulseLength_DrvLo &&
                     UAPP_PWM_Timer0.word <= UAPP_PulseLength_DrvHi )
                {
                UAPP_PWM_Gear = 'D';        // Set msg char.
                UAPP_OutputBits.bit3 = 1;   // Set DOUTI3 "Drive" output pin active.
                }
            else
                UAPP_PWM_Gear = '?';    // PWM not in a valid range.
    }   // switch( UAPP_InputBits.nibble0 )

    // If msg char changed then send an update message, and update DOUTs.

    if( UAPP_PWM_GearTemp != UAPP_PWM_Gear )
        {                                       
        UAPP_U_Msg();                   // Write status msg on change.
        UAPP_WriteDiscreteOutputs();    // Write discrete outs on change.
        }
}

//*******************************************************************************
//
// TaskADC - Convert A/D result and do something with it. UNUSED.
//
void UAPP_TaskADC( void )
{
}

//*******************************************************************************
//
// UAPP_U_Msg - Create an UPDATE message showing the state of I/O bits.
//
// UPDATE msg = "[Uxxxx yyyy abcd g]" where
//  U = UPDATE
//  xxxx = input bits 3,2,1,0
//  yyyy = output bits 3,2,1,0
//  abcd = Timer0 16-bit value (PWM duration in .1us units)
//  g = gear (P,R,N,D,?)
//
void UAPP_U_Msg( void )
{
    SSIO_PutByteTxBufferC( '[' );
    SSIO_PutByteTxBufferC( 'U' );
    SSIO_PutByteTxBufferC( UAPP_InputBits.bit3 == 0 ? (char)'0' : (char)'1');
    SSIO_PutByteTxBufferC( UAPP_InputBits.bit2 == 0 ? (char)'0' : (char)'1');
    SSIO_PutByteTxBufferC( UAPP_InputBits.bit1 == 0 ? (char)'0' : (char)'1');
    SSIO_PutByteTxBufferC( UAPP_InputBits.bit0 == 0 ? (char)'0' : (char)'1');

    SSIO_PutByteTxBufferC( ' ' );
    SSIO_PutByteTxBufferC( UAPP_OutputBits.bit3 == 0 ? (char)'0' : (char)'1');
    SSIO_PutByteTxBufferC( UAPP_OutputBits.bit2 == 0 ? (char)'0' : (char)'1');
    SSIO_PutByteTxBufferC( UAPP_OutputBits.bit1 == 0 ? (char)'0' : (char)'1');
    SSIO_PutByteTxBufferC( UAPP_OutputBits.bit0 == 0 ? (char)'0' : (char)'1');

    SSIO_PutByteTxBufferC( ' ' );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_PWM_Timer0.nibble3] );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_PWM_Timer0.nibble2] );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_PWM_Timer0.nibble1] );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_PWM_Timer0.nibble0] );

    SSIO_PutByteTxBufferC( ' ' );
    SSIO_PutByteTxBufferC( UAPP_PWM_Gear );

    SSIO_PutStringTxBuffer( (char*) UAPP_MsgEnd );
}

//*******************************************************************************
//
//  Read input values from pins.
//
void UAPP_ReadDiscreteInputs( void )
{
    UAPP_InputBits.bit0 = PORTBbits.RB7;
    UAPP_InputBits.bit1 = PORTBbits.RB6;
    UAPP_InputBits.bit2 = PORTBbits.RB5;
    UAPP_InputBits.bit3 = PORTBbits.RB4;
    UAPP_InputBits.bit4 = PORTBbits.RB3;
    UAPP_InputBits.bit5 = PORTBbits.RB2;
    UAPP_InputBits.bit6 = PORTBbits.RB1;
    UAPP_InputBits.bit7 = PORTBbits.RB0;

    UAPP_InputBits.byte = ~UAPP_InputBits.byte;   //  Complement for active low.
}

//*******************************************************************************
//
//  Write output values to pins.
//
void UAPP_WriteDiscreteOutputs( void )
{
    LATAbits.LATA4 = UAPP_OutputBits.bit0;
    LATAbits.LATA5 = UAPP_OutputBits.bit1;
    LATCbits.LATC5 = UAPP_OutputBits.bit2;
    LATCbits.LATC4 = UAPP_OutputBits.bit3;
    LATCbits.LATC3 = UAPP_OutputBits.bit4;
    LATCbits.LATC2 = UAPP_OutputBits.bit5;
    LATCbits.LATC1 = UAPP_OutputBits.bit6;
    LATCbits.LATC0 = UAPP_OutputBits.bit7;
}

//*******************************************************************************
//
//  Clear UAPP_BufferRc buffer.
//
void UAPP_ClearRcBuffer( void )
{
    UAPP_IndexRc = 0;
}

//*******************************************************************************
//
//  Copy input byte to UAPP_BufferRc.
//
void UAPP_PutByteRcBuffer( unsigned char RcChar )
{
    UAPP_BufferRc[UAPP_IndexRc++] = RcChar;
}

//*******************************************************************************
//
//  Parse command string in UAPP_BufferRc.
//
void UAPP_ParseRcMsg( void )
{
unsigned char i;

    for( i=0; i<UAPP_IndexRc; i++ )
       if( UAPP_BufferRc[i] != 0x0a && UAPP_BufferRc[i] != 0x0d )
            SSIO_PutByteTxBufferC( UAPP_BufferRc[i] );

    if( UAPP_BufferRc[0] == '[' )
    {
        switch( UAPP_BufferRc[1] ) {
        case 'V':
            SSIO_PutStringTxBuffer( (char*) UAPP_MsgVersion );  // Version message.
            break;  // case 'V'
        default:
            break;
        };  // switch( UAPP_BufferRc[1] )
    }

    UAPP_ClearRcBuffer();
}
