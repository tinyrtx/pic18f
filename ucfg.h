//*******************************************************************************
// tinyRTX Filename: ucfg.h (User ConFiGuration)
//
// Copyright 2015 Sycamore Software, Inc.  ** www.tinyRTX.com **
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
// NOTE: This file must be (manually) kept in synch with ucfg.inc !!!
// ******************************************************************
//
// Revision history:
//  20May15 Stephen_Higgins@KairosAutonomi.com  Created from ucfg.inc.
//  28May15 Stephen_Higgins@KairosAutonomi.com
//              Move selection of board to assembler macro to keep w project.
//  09Jul15 Stephen_Higgins@KairosAutonomi.com
//              Split UCFG_DJPCB_280B into UCFG_KA280BI and UCFG_KA280BT.
//  13Jul15 Stephen_Higgins@KairosAutonomi.com
//              Replace hardcoded <CR> with UCFG_SSIO_EOMC (End Of Msg Char) so
//              incoming msg string may be terminated by any character.
//  22Jul15 Stephen_Higgins@KairosAutonomi.com
//              Moved all #config here as CONFIG needed in ucfg.inc by SBTL.
//
//*******************************************************************************
//
//   Define all supported board configurations.
//
#define UCFG_PD2P_2002      0x01    // Microchip PICDEM2PLUS 2002 (has ext 4MHz)
#define UCFG_PD2P_2010      0x02    // Microchip PICDEM2PLUS 2010 (no ext 4MHz)
#define UCFG_KA280BI        0x10    // Kairos Autonomi 280B: Utility Module I/O
#define UCFG_KA280BT        0x11    // Kairos Autonomi 280B: Transmission & I/O
//
//   Define all supported processors.
//
#define UCFG_18F452     0x01        // 40 pins.
#define UCFG_18F2620    0x02        // 28 pins.
//
//*******************************************************************************
//
//  Include appropriate processor definition file, define UCFG_PROC.
//  Define UCFG_BOARD, set UCFG_SSIO_EOMC.
//  Output message of configuration of board and procesor.
//
    #ifdef __18F452
        #include "p18f452.h"
        #define UCFG_PROC   UCFG_18F452     // Allows logical expressions.
        #warning "Processor defined: 18F452"
    #endif       
    #ifdef __18F2620
        #include "p18f2620.h"
        #define UCFG_PROC   UCFG_18F2620    // Allows logical expressions.
        #warning "Processor defined: 18F2620"
    #endif       

    #ifdef _UCFG_PD2P02
        #define UCFG_BOARD  UCFG_PD2P_2002  // Allows logical expressions.
        #define UCFG_SSIO_EOMC  0x0d        // End Of Msg Char = <CR>
        #warning "Board defined: Microchip PICDEM2PLUS 2002"
    #endif       
    #ifdef _UCFG_PD2P10
        #define UCFG_BOARD  UCFG_PD2P_2010  // Allows logical expressions.
        #define UCFG_SSIO_EOMC  0x0d        // End Of Msg Char = <CR>
        #warning "Board defined: Microchip PICDEM2PLUS 2010"
    #endif       
    #ifdef _UCFG_KA280BI
        #define UCFG_BOARD  UCFG_KA280BI // Allows logical expressions.
        #define UCFG_SSIO_EOMC  0x5d        // End Of Msg Char = "]"
        #warning "Board defined: KA280BI"
    #endif       
    #ifdef _UCFG_KA280BT
        #define UCFG_BOARD  UCFG_KA280BT // Allows logical expressions.
        #define UCFG_SSIO_EOMC  0x5d        // End Of Msg Char = "]"
        #warning "Board defined: KA280BT"
    #endif       

//*******************************************************************************
//
//   Set CONFIG bits. Valid values found in <"processor".h>, e.g. <p18f2620.h>.
//
//*******************************************************************************
//
//  UCFG_18F452 specified. Board doesn't matter (currently) so common to all boards.
//  ********************************************************************************

    #if UCFG_PROC==UCFG_18F452
        #pragma config  OSC = RCIO          // RC oscillator w/ OSC2 configured as RA6
        #pragma config  OSCS = OFF          // Oscillator system clock switch option is disabled (main oscillator is source)
        #pragma config  BOR = ON            // Brown-out Reset enabled
        #pragma config  BORV = 20           // VBOR set to 2.0V
        #pragma config  CCP2MUX = ON        // CCP2 input/output is multiplexed with RC1
        #pragma config  STVR = ON           // Stack full/underflow will cause Reset
    #endif

//  UCFG_18F2620 specified.
//  ***********************

    #if UCFG_PROC==UCFG_18F2620

//      UCFG_PD2P_2002 specified.
//      *************************

        #if UCFG_BOARD==UCFG_PD2P_2002
        #pragma config  OSC = RCIO6         // External RC oscillator, port function on RA6
        #endif

//      UCFG_PD2P_2010 specified.
//      *************************

        #if UCFG_BOARD==UCFG_PD2P_2010
        #pragma config  OSC = INTIO67       // Internal oscillator block, port function on RA6 and RA7
        #endif

//      UCFG_KA280BI or UCFG_KA280BT specified.
//      ***************************************

        #if UCFG_BOARD==UCFG_KA280BI || UCFG_BOARD==UCFG_KA280BT
        #pragma config  OSC = HSPLL         // HS oscillator, PLL enabled (Clock Frequency = 4 x Fosc1)
        #endif

//      UCFG_18F2620 common to all boards.
//      **********************************

        #pragma config  FCMEN = OFF         // Fail-Safe Clock Monitor disabled
        #pragma config  IESO = OFF          // Oscillator Switchover mode disabled
        #pragma config  BOREN = SBORDIS     // Brown-out Reset enabled in hardware only (SBOREN is disabled)
        #pragma config  BORV = 3            // Minimum setting
        #pragma config  CCP2MX = PORTC      // CCP2 input/output is multiplexed with RC1
        #pragma config  PBADEN = ON         // PORTB<4:0> pins are configured as analog input channels on Reset
        #pragma config  LPT1OSC = OFF       // Timer1 configured for higher power operation
        #pragma config  MCLRE = ON          // MCLR pin enabled// RE3 input pin disabled
        #pragma config  STVREN = ON         // Stack full/underflow will cause Reset
        #pragma config  XINST = OFF         // Instruction set extension and Indexed Addressing mode disabled (Legacy mode)
    #endif

//*******************************************************************************
//
//  CONFIG common to all processors and all boards.
//  ***********************************************

        #pragma config  PWRT = OFF          // PWRT disabled
        #pragma config  WDT = OFF           // WDT disabled (control is placed on the SWDTEN bit)
        #pragma config  WDTPS = 128         // 1:128
        #pragma config  LVP = OFF           // Single-Supply ICSP disabled
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

//*******************************************************************************
