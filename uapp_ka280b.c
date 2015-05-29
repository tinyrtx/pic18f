//*******************************************************************************
// tinyRTX Filename: uapp_ka280b.c (User APPlication for KA 280B board)
//
// Copyright 2014 Sycamore Software, Inc.  ** www.tinyRTX.com **
// Distributed under the terms of the GNU Lesser General Purpose License v3
//
// This file is part of tinyRTX. tinyRTX is free software: you can redistribute
// it and/or modify it under the terms of the GNU Lesser General Public License
// version 3 as published by the Free Software Foundation.
//
// tinyRTX is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY// without even the implied warranty of MERCHANTABILITY or FITNESS FOR
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
//               Create uapp_ka280b.c from uapp_ka280b.asm to allow user apps in C.
//
//*******************************************************************************
//
#include "ucfg.h"          // includes processor definitions.
#include "si2c.h"
#include "ssio.h"
#include "usio.h"
//
extern void UAPP_POR_Init_PhaseB( void );
extern void UAPP_POR_Init_PhaseA( void );
extern void UAPP_Timer1Init( void );
extern void UAPP_Task1( void );
extern void UAPP_Task2( void );
extern void UAPP_Task3( void );
extern void UAPP_TaskADC( void );
//
//*******************************************************************************
//
//   UCFG_DJPCB_280B specified.
//   **************************
//
// Hardware: Kairos Autonomi 280B circuit board.
//           Microchip PIC18F2620 processor with 10 MHz input resonator.
//
// Functions:
//  1) Read length of low pulse on pin 18 (AIN3), determine transmission mode.
//  2) Read contents of 8 digital inputs and send to 8 digital outputs.
//  3) Send ASCII text to RS-232 port.  Receive and echo RS-232 bytes.
//
// Complete PIC18F2620 (28-pin device) pin assignments for KA board 280B:
//
//  1) MCLR*/Vpp/RE3             = Reset/Programming connector(1): ATN TTL (active low)
//  2) RA0/AN0                   = Analog In: AIN0
//  3) RA1/AN1                   = Analog In: AIN1
//  4) RA2/AN2/Vref-/CVref       = Analog In: AIN2
//  5) RA3/AN3/Vref+             = Analog In: AIN3
//  6) RA4/T0KI/C1OUT            = RX TTL: (configured as Discrete In) (Not used for I2C.)
//  7) RA5/AN4/SS*/HLVDIN/C2OUT  = TX TTL: (configured as Discrete In) (Not used for I2C.)
//  8) Vss                       = Programming connector(3) (Ground)
//
//   External 10 Mhz ceramic oscillator installed in pins 10, 11// KA board 280B.
//
//  9) OSC1/CLKIN/RA7        = 10 MHz clock in (10 MHz * 4(PLL)/4 = 10 MHz = 0.1us instr cycle)
// 10) OSC2/CLKOUT/RA6       = (non-configurable output)
//
// 11) RC0/T1OSO/T13CKI      = Discrete Out: DOUT17
// 12) RC1/T1OSI/CCP2        = Discrete Out: DOUT16
// 13) RC2/CCP1              = Discrete Out: DOUT15
// 14) RC3/SCK/SCL           = Discrete Out: DOUT14 (Not used for SPI.) (Not used for I2C.)
// 15) RC4/SDI/SDA           = Discrete Out: DOUT13 (Not used for SPI.) (Not used for I2C.)
// 16) RC5/SDO               = Discrete Out: DOUT12 (Not used for SPI.)
// 17) RC6/TX/CK             = Discrete Out, USART TX (RS-232): DOUT11
//                               USART control of this pin requires pin as Discrete In.
// 18) RC7/RX/DT             = Discrete Out, USART TX (RS-232): DOUT10
//                               USART control of this pin requires pin as Discrete In.
// 19) Vss                   = Programming connector(3) (Ground)
// 20) Vdd                   = Programming connector(2) (+5 VDC)
// 21) RB0/INT0/FLT0/AN12    = Discrete In: DIN7
// 22) RB1/INT1/AN10         = Discrete In: DIN6
// 23) RB2/INT2/AN8          = Discrete In: DIN5
// 24) RB3/AN9/CCP2          = Discrete In: DIN4
// 25) RB4/KB10/AN11         = Discrete In: DIN3
// 26) RB5/KB11/PGM          = Discrete In: DIN2
// 27) RB6/KB12/PGC          = Discrete In, Programming connector(5) (PGC): DIN1
//                               ICD2 control of this pin requires pin as Discrete In.
// 28) RB7/KB13/PGD          = Discrete In, Programming connector(4) (PGD): DIN0
//                               ICD2 control of this pin requires pin as Discrete In.
//
//   User CONFIG. Valid values are found in <"processor".inc>, e.g. <p18f2620.inc>.
//
#pragma config  OSC = HSPLL         // HS oscillator, PLL enabled (Clock Frequency = 4 x Fosc1)
#pragma config  FCMEN = OFF         // Fail-Safe Clock Monitor disabled
#pragma config  IESO = OFF          // Oscillator Switchover mode disabled
#pragma config  PWRT = OFF          // PWRT disabled
#pragma config  BOREN = SBORDIS     // Brown-out Reset enabled in hardware only (SBOREN is disabled)
#pragma config  BORV = 3            // Minimum setting
#pragma config  WDT = OFF           // WDT disabled (control is placed on the SWDTEN bit)
#pragma config  WDTPS = 32768       // 1:32768
#pragma config  CCP2MX = PORTC      // CCP2 input/output is multiplexed with RC1
#pragma config  PBADEN = ON         // PORTB<4:0> pins are configured as analog input channels on Reset
#pragma config  LPT1OSC = OFF       // Timer1 configured for higher power operation
#pragma config  MCLRE = ON          // MCLR pin enabled// RE3 input pin disabled
#pragma config  STVREN = ON         // Stack full/underflow will cause Reset
#pragma config  LVP = OFF           // Single-Supply ICSP disabled
#pragma config  XINST = OFF         // Instruction set extension and Indexed Addressing mode disabled (Legacy mode)
#pragma config  CP0 = OFF           // Block 0 (000800-003FFFh) not code-protected
#pragma config  CP1 = OFF           // Block 1 (004000-007FFFh) not code-protected
#pragma config  CP2 = OFF           // Block 2 (008000-00BFFFh) not code-protected
#pragma config  CP3 = OFF           // Block 3 (00C000-00FFFFh) not code-protected
#pragma config  CPB = OFF           // Boot block (000000-0007FFh) not code-protected
#pragma config  CPD = OFF           // Data EEPROM not code-protected
#pragma config  WRT0 = OFF          // Block 0 (000800-003FFFh) not write-protected
#pragma config  WRT1 = OFF          // Block 1 (004000-007FFFh) not write-protected
#pragma config  WRT2 = OFF          // Block 2 (008000-00BFFFh) not write-protected
#pragma config  WRT3 = OFF          // Block 3 (00C000-00FFFFh) not write-protected
#pragma config  WRTC = OFF          // Configuration registers (300000-3000FFh) not write-protected
#pragma config  WRTB = OFF          // Boot Block (000000-0007FFh) not write-protected
#pragma config  WRTD = OFF          // Data EEPROM not write-protected
#pragma config  EBTR0 = OFF         // Block 0 (000800-003FFFh) not protected from table reads executed in other blocks
#pragma config  EBTR1 = OFF         // Block 1 (004000-007FFFh) not protected from table reads executed in other blocks
#pragma config  EBTR2 = OFF         // Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks
#pragma config  EBTR3 = OFF         // Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks
#pragma config  EBTRB = OFF         // Boot Block (000000-0007FFh) not protected from table reads executed in other blocks
//
// User APP defines.
//
#define UAPP_OSCCON_VAL  0x64
//
//   40 Mhz clock// use HS PLL with external 10 MHz resonator.
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
// bit 3 : RB3/AN9/CCP2          : 0 : Discrete In: (don't care)
// bit 2 : RB2/INT2/AN8          : 0 : Discrete In: (don't care)
// bit 1 : RB1/INT1/AN10         : 0 : Discrete In: (don't care)
// bit 0 : RB0/INT0/FLT0/AN12    : 0 : Discrete In: (don't care)
//
#define UAPP_TRISB_VAL  0xff
//
// Set TRISB RB0-RB7 to inputs.  PGC and PGD need to be configured as high-impedance inputs.
//
// bit 7 : DDRB7  : 1 : Disrete In
// bit 6 : DDRB6  : 1 : Disrete In
// bit 5 : DDRB5  : 1 : Disrete In
// bit 4 : DDRB4  : 1 : Disrete In
// bit 3 : DDRB3  : 1 : Disrete Out
// bit 2 : DDRB2  : 1 : Disrete Out
// bit 1 : DDRB1  : 1 : Disrete Out
// bit 0 : DDRB0  : 1 : Disrete Out
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
#define UAPP_TRISC_VAL  0xc0
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
#define UAPP_TMR1L_VAL  0xdc
#define UAPP_TMR1H_VAL  0x0b
//
// 40 Mhz Fosc/4 is base clock = 10 Mhz = 0.1 us per clock.
// 1:8 prescale = 1.0 * 8 = 0.8 us per clock.
// 62,500 counts * 0.8us/clock = 50,000 us/rollover = 50ms/rollover.
// Timer preload value = 65,536 - 62,500 = 3,036 = 0x0bdc.
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
//
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
//
// PIE1 changed: ADIE, RCIE, TXIE, SSPIE, CCP1IE, TMR2IE, TMR1IE disabled.
//
    PIE1 = UAPP_PIE1_VAL;
//
// PIR1 changed: ADIF, RCIF, TXIF, SSPIF, CCP1IF, TMR2IF, TMR1IF cleared.
//
    PIR1 = UAPP_PIR1_VAL;
//
// PIE2 untouched// EEIE, BCLIE disabled.
// PIR2 untouched// EEIR, BCLIF remain cleared.
//
// IPEN cleared so disable priority levels on interrupts (PIC16 compatiblity mode.)
// RI set// TO set// PD set// POR set// BOR set// subsequent hardware resets will clear these bits.
//
    RCON = UAPP_RCON_VAL;
//
// INTCON changed: GIE, PEIE enabled// TMR0IE, INT0IE, RBIE disabled// TMR0IF, INT0IF, RBIF cleared.
//
    INTCON = UAPP_INTCON_VAL;
//
//   Global interrupts enabled. The following routines
//   may enable additional specific interrupts.
//
    USIO_Init();        // User Serial I/O hardware init.
}
//
//*******************************************************************************
//
// Init Timer1 module to generate timer interrupt every 100ms.
//
void UAPP_Timer1Init( void )
{
    T1CON = UAPP_T1CON_VAL;   // Initialize Timer1 but don't start it.
    TMR1L = UAPP_TMR1L_VAL;   // Timer1 pre-load value, low byte.
    TMR1H = UAPP_TMR1H_VAL;   // Timer1 pre-load value, high byte.
//
    PIE1bits.TMR1IE = 1;      // Enable Timer1 interrupts.
    T1CONbits.TMR1ON = 1;     // Turn on Timer1 module.
}
//
//*******************************************************************************
//
// Task1
//
void UAPP_Task1( void )
{
//
//  Wait for SDIO to be implemented, then copy port A to port B.
//
//        movf    PORTA, W                ; Get contents of PORTA.
//        movwf   PORTB                   ; Store contents at PORTB.
//
//  State of health message at RS-232 port.
//
    _asm
    movlw   0x31
    _endasm
    SSIO_PutByteTxBuffer();
 //
    _asm
    movlw   0x0d
    _endasm
    SSIO_PutByteTxBuffer();
 //
    _asm
    movlw   0x0a
    _endasm
    SSIO_PutByteTxBuffer();
}
//
//*******************************************************************************
//
// Task2.
//
void UAPP_Task2( void )
{
//
//  State of health message at RS-232 port.
//
    _asm
    movlw   0x32
    _endasm
    SSIO_PutByteTxBuffer();
 //
    _asm
    movlw   0x32
    _endasm
    SSIO_PutByteTxBuffer();
 //
    _asm
    movlw   0x0d
    _endasm
    SSIO_PutByteTxBuffer();
 //
    _asm
    movlw   0x0a
    _endasm
    SSIO_PutByteTxBuffer();
}
//
//*******************************************************************************
//
// Task3.
//
void UAPP_Task3( void )
{
//
//  State of health message at RS-232 port.
//
    _asm
    movlw   0x33
    _endasm
    SSIO_PutByteTxBuffer();
 //
    _asm
    movlw   0x33
    _endasm
    SSIO_PutByteTxBuffer();
 //
    _asm
    movlw   0x33
    _endasm
    SSIO_PutByteTxBuffer();
 //
    _asm
    movlw   0x0d
    _endasm
    SSIO_PutByteTxBuffer();
 //
    _asm
    movlw   0x0a
    _endasm
    SSIO_PutByteTxBuffer();
}
//
//*******************************************************************************
//
// TaskADC - Convert A/D result and do something with it.
//
void UAPP_TaskADC( void )
{
}
