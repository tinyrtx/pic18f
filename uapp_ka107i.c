//*******************************************************************************
// tinyRTX Filename: uapp_ka107i.c
//  User APPlication for KA 107I board - Quad and Video Mux
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
//      Init message "[Wheel Sensor v3.0.0 107I 20150903] (or whatever date)"
//      Processes input messages: (c is checksum)
//      "[QQ]"  responds by sending Init message
//      "[BB]"  responds by invoking Bootloader.

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

//      "[RR]"  responds with "[Dii00000000c]" (see UAPP_D_Msg())
//      "[T0c]"  responds by setting all discrete outputs active (high)
//      "[T1c]"  responds by setting all discrete outputs inactive (low)
//      "[Lnc]"  responds by setting discrete output n inactive (low)
//      "[Hnc]"  responds by setting discrete output n active (high)
//
// Complete PIC18F2620 (28-pin device) pin assignments for KA board 107I:
//
//  1) MCLR*/Vpp/RE3             = Reset/Programming connector(1): ATN TTL (active low)
//  2) RA0/AN0                   = Discrete Out: LS7566 CLK (rising edge clocks latches)
//  3) RA1/AN1                   = Discrete Out: LS7566 RD (active low)
//  4) RA2/AN2/Vref-/CVref       = Discrete Out: LS7566 DB6
//  5) RA3/AN3/Vref+             = Discrete Out: LS7566 WR (rising edge clocks write)
//  6) RA4/T0KI/C1OUT            = Discrete Out: LS7566 DB4
//  7) RA5/AN4/SS*/HLVDIN/C2OUT  = Discrete Out: LS7566 DB5
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
//      JP11: local SWTX (see JP10)
//          1-2: chip (pin 7) P16D (DP) (KA)
//          2-3: chip (pin 25) P3D 
//      JP9: target (pin 6) RS-232 RX
//          1-2: chip (pin 6) SWRX (DP)
//          2-3: chip (pin 18) HWRX (KA)
//      JP10: target (pin 7) RS-232 TX
//          1-2: chip (pin 7) SWTX (DP)
//          2-3: chip (pin 17) HWTX (KA)
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
#include "si2c.h"
#include "ssio.h"
#include "sutl.h"
#include "uadc.h"
#include "usio.h"

//  Internal prototypes.

void UAPP_D_Msg( void );
void UAPP_ReadDiscreteInputs( void );
void UAPP_WriteDiscreteOutputs( void );
void UAPP_ClearRxBuffer( void );

//  Constants.

#define FALSE 0
#define TRUE  0xFF

#define UAPP_ADC_CHANNEL_INIT   0
#define UAPP_ADC_CHANNEL_MAX    1
#define UAPP_ADC_ARRAY_INIT     0
#define UAPP_ADC_ARRAY_MAX      7

//  String literals.

const char UAPP_MsgInit[] = "[Wheel Sensor v3.0.0 107I 20150903]\n\r";
const char UAPP_MsgEnd[] = "c]\n\r";
const unsigned char UAPP_Nibble_ASCII[] = "0123456789ABCDEF";

//  Variables.

#pragma udata   UAPP_UdataSec

SUTL_Byte UAPP_InputBits;
SUTL_Byte UAPP_OutputBits;

// Internal variables to compute save and average analog inputs.

unsigned char UAPP_ADCActiveChannel;                    // ADC active channel.
unsigned char UAPP_ADC0Index;                           // Index into UAPP_ADC0Values.
unsigned int  UAPP_ADC0Values[UAPP_ADC_ARRAY_MAX+1];    // Saved values to average.
SUTL_Word     UAPP_ADC0Average;               // Average of saved value.
SUTL_Word     UAPP_ADC1Instantaneous;         // ADC1 only has instantaneous read.

// Internal variables to manage receive buffer.

unsigned char UAPP_BufferRx[40];
unsigned char UAPP_IndexRx;

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
// bit 3 : RA3/AN3/Vref+             : 0 : AN3 input (unused, configured as AN3 input)
// bit 2 : RA2/AN2/Vref-/CVref       : 0 : AN2 input (unused, configured as AN2 input)
// bit 1 : RA1/AN1                   : 0 : AN1 input: Instantaneous analog input
// bit 0 : RA0/AN0                   : 0 : AN0 input: Averaged analog input
//
#define UAPP_TRISA_VAL  0x0F
//
// Set all PORTA to don't care, outputs, and inputs.
//
// bit 7 : TRISA7 : 0 : Using OSC1, overridden by CONFIG1H (don't care)
// bit 6 : TRISA6 : 0 : Using OSC2, overridden by CONFIG1H (don't care)
// bit 5 : DDRA5  : 0 : Discrete Out
// bit 4 : DDRA4  : 0 : Discrete Out
// bit 3 : DDRA3  : 1 : Analog In
// bit 2 : DDRA2  : 1 : Analog In
// bit 1 : DDRA1  : 1 : Analog In
// bit 0 : DDRA0  : 1 : Analog In
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
//
#pragma code    UAPP_CodeSec
//
void UAPP_POR_Init_PhaseA( void )
{
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

// INTCON changed: GIE, PEIE enabled// TMR0IE, INT0IE, RBIE disabled// TMR0IF, INT0IF, RBIF cleared.

    INTCON = UAPP_INTCON_VAL;

//  Global interrupts enabled. The following routines
//      may enable additional specific interrupts.

    UAPP_ADC0Index = UAPP_ADC_ARRAY_INIT;               // Init index into ADC0 array.
    UAPP_ADCActiveChannel = UAPP_ADC_CHANNEL_INIT;      // Init local active channel.
    UADC_SetActiveChannelC( UAPP_ADCActiveChannel );    // Select ADC active channel.
    UADC_Init();            // User ADC init (config pins even if not using ADC.)

    UAPP_ClearRxBuffer();   // Clear Rx buffer before messages can arrive.
    USIO_Init();            // User Serial I/O hardware init.

    UAPP_OutputBits.byte = 0x00;    // Init output bits to low.
    UAPP_WriteDiscreteOutputs();    // Write output bits to discrete output pins.

    SSIO_PutStringTxBuffer( (char*) UAPP_MsgInit );     // Version message.
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
// Background Task. UNUSED.
// Not used for I/O board functions, only 280BT Transmission.
//
void UAPP_BkgdTask( void )
{
}

//*******************************************************************************
//
// Task1 - 100 ms.
//
void UAPP_Task1( void )
{
    // Toggle RA2 for timing measurement, this pin only used if using 3 or 4 analogs in.
    //  Method 1: (RA2)
    //    ADCON1 = 0x0D;          // Override ADCON1 to change RA2:RA3 from AIN to discrete I/O.
    //    TRISAbits.TRISA2 = 0b0; // Set RA2 to discrete output.
    //    LATAbits.LATA2 = 0b0;   // Drive RA2 low to show ADC starting.
    //  Method 2: (RA4)
    //    LATAbits.LATA4 = 0b0;

    // Wrap or increment ADC active channel for this entry.
    if( UAPP_ADC_CHANNEL_MAX <= UAPP_ADCActiveChannel )
        UAPP_ADCActiveChannel = UAPP_ADC_CHANNEL_INIT;  // Init local active channel.
    else
        UAPP_ADCActiveChannel++;

    UADC_SetActiveChannelC(UAPP_ADCActiveChannel);      // Select ADC active channel.
    UADC_Trigger();     // Initiate new A/D conversion. (Enables ADC interrupt.)
}

//*******************************************************************************
//
// Task2 - 1.0 sec. UNUSED.
//
void UAPP_Task2( void )
{
//  LATAbits.LATA4 = LATAbits.LATA4 ^ 1;    // Toggle RA4 for testing
}

//*******************************************************************************
//
// Task3 - 2.5 sec. UNUSED.
//
void UAPP_Task3( void )
{
//  LATAbits.LATA4 = LATAbits.LATA4 ^ 1;    // Toggle RA4 for testing
//  UAPP_D_Msg();                           // Show analog values for testing.
}

//*******************************************************************************
//
// TaskADC - Convert A/D result and do something with it.
//
void UAPP_TaskADC( void )
{
unsigned char i;

    switch( UAPP_ADCActiveChannel )
    {
    case 0:                         // ADC channel 0 gets 8 values averaged.
        UAPP_ADC0Values[UAPP_ADC0Index] = UADC_Raw10BitC();

        // Wrap or increment ADC0 array index.
        if( UAPP_ADC_ARRAY_MAX <= UAPP_ADC0Index )
            UAPP_ADC0Index = UAPP_ADC_ARRAY_INIT;
        else
            UAPP_ADC0Index++;

        // Average ADC0 values.
        // (First 7 times through UAPP_ADC0Average too low, that's OK.)
        UAPP_ADC0Average.word = 0;
        for( i=0; i<=UAPP_ADC_ARRAY_MAX; i++ )
            UAPP_ADC0Average.word += UAPP_ADC0Values[i];

        UAPP_ADC0Average.word >>= 3;    // Shift right by 3 same as divide by 8.
        break;

    case 1:                             // ADC channel 1 gets latest value.
        UAPP_ADC1Instantaneous.word = UADC_Raw10BitC();
        break;
    }   // switch( UAPP_ADCActiveChannel )

    // Toggle RA2 for timing measurement, this pin only used if using 3 or 4 analogs in.
    //  Method 1: (RA2)
    //    LATAbits.LATA2 = 0b1;   // Drive RA2 high to show ADC ended.
    //  Method 2: (RA4)
    //    LATAbits.LATA4 = 0b1;
}

//*******************************************************************************
//
// UAPP_D_Msg - Create a D message formatted for djAuxFunctions.bas .
//
// D msg = "[Diixxxxyyyyc]" where
//  D = UPDATE
//  ii = bits 7,6,5,4,3,2,1,0
//  xxxx = ADC0 value (16 bits)
//  yyyy = ADC1 value (16 bits)
//  c = bogus checksum
//
void UAPP_D_Msg( void )
{
    UAPP_ReadDiscreteInputs();      // Read current state of DIN's.

    SSIO_PutByteTxBufferC( '[' );
    SSIO_PutByteTxBufferC( 'D' );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_InputBits.nibble1] );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_InputBits.nibble0] );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_ADC0Average.nibble3] );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_ADC0Average.nibble2] );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_ADC0Average.nibble1] );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_ADC0Average.nibble0] );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_ADC1Instantaneous.nibble3] );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_ADC1Instantaneous.nibble2] );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_ADC1Instantaneous.nibble1] );
    SSIO_PutByteTxBufferC( UAPP_Nibble_ASCII[UAPP_ADC1Instantaneous.nibble0] );
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

// Temp map for testing (0,1 used by ICSP, 3 follows 2)

//    UAPP_InputBits.bit0 = PORTBbits.RB3;    // DIN4
//    UAPP_InputBits.bit1 = PORTBbits.RB2;    // DIN5
//    UAPP_InputBits.bit3 = PORTBbits.RB1;    // DIN6
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
//  Clear UAPP Rx buffer.
//
void UAPP_ClearRxBuffer( void )
{
    UAPP_IndexRx = 0;
}

//*******************************************************************************
//
//  Copy input byte to UAPP Rx buffer.
//
void UAPP_PutByteRxBuffer( unsigned char RxChar )
{
    UAPP_BufferRx[UAPP_IndexRx++] = RxChar;
}

//*******************************************************************************
//
//  Parse command string in UAPP Rx buffer.
//
void UAPP_ParseRxMsg( void )
{
unsigned char i;

    for( i=0; i<UAPP_IndexRx; i++ )
       if( UAPP_BufferRx[i] != 0x0a && UAPP_BufferRx[i] != 0x0d )
            SSIO_PutByteTxBufferC( UAPP_BufferRx[i] );

    if( UAPP_BufferRx[0] == '[' )
    {
        switch( UAPP_BufferRx[1] ) {
        case 'Q':
            SSIO_PutStringTxBuffer( (char*) UAPP_MsgInit );     // Version message.
            break;  // case 'Q'
        case 'B':
            SUTL_InvokeBootloader();
            break;  // case 'R'
        case 'R':
            UAPP_D_Msg();
            break;  // case 'R'
        case 'T':
            switch( UAPP_BufferRx[2] ) {
            case '0':
                UAPP_OutputBits.byte = 0xff;    // '0' means write all active (on).
                UAPP_WriteDiscreteOutputs();    // Write discrete outs on change.
                break;
            case '1':
                UAPP_OutputBits.byte = 0x00;    // '1' means write all inactive (off).
                UAPP_WriteDiscreteOutputs();    // Write discrete outs on change.
                break;
            };
            break;  // case 'T'
        case 'H':                               // Set specified output high.
            switch( UAPP_BufferRx[2] ) {
            case '0':
                UAPP_OutputBits.bit0 = 1;
                break;
            case '1':
                UAPP_OutputBits.bit1 = 1;
                break;
            case '2':
                UAPP_OutputBits.bit2 = 1;
                break;
            case '3':
                UAPP_OutputBits.bit3 = 1;
                break;
            case '4':
                UAPP_OutputBits.bit4 = 1;
                break;
            case '5':
                UAPP_OutputBits.bit5 = 1;
                break;
            case '6':
                UAPP_OutputBits.bit6 = 1;
                break;
            case '7':
                UAPP_OutputBits.bit7 = 1;
                break;
            default:
                break;
            };
            UAPP_WriteDiscreteOutputs();        // Write discrete outputs.
            break;  // case 'H'
        case 'L':                               // Set specified output low.
            switch( UAPP_BufferRx[2] ) {
            case '0':
                UAPP_OutputBits.bit0 = 0;
                break;
            case '1':
                UAPP_OutputBits.bit1 = 0;
                break;
            case '2':
                UAPP_OutputBits.bit2 = 0;
                break;
            case '3':
                UAPP_OutputBits.bit3 = 0;
                break;
            case '4':
                UAPP_OutputBits.bit4 = 0;
                break;
            case '5':
                UAPP_OutputBits.bit5 = 0;
                break;
            case '6':
                UAPP_OutputBits.bit6 = 0;
                break;
            case '7':
                UAPP_OutputBits.bit7 = 0;
                break;
            default:
                break;
            };
            UAPP_WriteDiscreteOutputs();        // Write discrete outputs.
            break;  // case 'L'
        default:
            break;
        };  // switch( UAPP_BufferRx[1] )
    }

    UAPP_ClearRxBuffer();
}
