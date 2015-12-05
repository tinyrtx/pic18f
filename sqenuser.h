//**********************************************************************************************
// Filename: sqenuser.inc (System Quadrature ENcoder - USER configuration)
//  SLCD configuration is defined by these #defines.
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
//  03Sep15 Stephen_Higgins@KairosAutonomi.com
//              Created from scratch.
//  25Nov15 Stephen_Higgins@KairosAutonomi.com
//              Add data direction and port definitions.
//
//*********************************************************************************************

// SQEN configuration is defined by these #defines.
// User should set them to match hardware configuration.
// NOTE: User is also responsible for proper PIC hardware initialization so
//   PIC registers addressed by these #defines may be used.

#if UCFG_BOARD==UCFG_KA107I

#define     SQEN_7566_WR       LATAbits.LATA3
#define     SQEN_7566_RD       LATAbits.LATA1

#define     SQEN_7566_CHS0     LATCbits.LATC4
#define     SQEN_7566_CHS1     LATCbits.LATC3

#define     SQEN_7566_RS0      LATCbits.LATC2
#define     SQEN_7566_RS1      LATCbits.LATC1
#define     SQEN_7566_RS2      LATCbits.LATC0

#define     SQEN_7566_DB0_OUT  LATBbits.LATB7
#define     SQEN_7566_DB1_OUT  LATBbits.LATB6
#define     SQEN_7566_DB2_OUT  LATBbits.LATB5
#define     SQEN_7566_DB3_OUT  LATBbits.LATB4
#define     SQEN_7566_DB4_OUT  LATAbits.LATA4
#define     SQEN_7566_DB5_OUT  LATAbits.LATA5
#define     SQEN_7566_DB6_OUT  LATAbits.LATA2
#define     SQEN_7566_DB7_OUT  LATCbits.LATC5

#define     SQEN_7566_DB0_IN   PORTBbits.RB7
#define     SQEN_7566_DB1_IN   PORTBbits.RB6
#define     SQEN_7566_DB2_IN   PORTBbits.RB5
#define     SQEN_7566_DB3_IN   PORTBbits.RB4
#define     SQEN_7566_DB4_IN   PORTAbits.RA4
#define     SQEN_7566_DB5_IN   PORTAbits.RA5
#define     SQEN_7566_DB6_IN   PORTAbits.RA2
#define     SQEN_7566_DB7_IN   PORTCbits.RC5

#define     SQEN_7566_DB0_DIR  TRISBbits.TRISB7
#define     SQEN_7566_DB1_DIR  TRISBbits.TRISB6
#define     SQEN_7566_DB2_DIR  TRISBbits.TRISB5
#define     SQEN_7566_DB3_DIR  TRISBbits.TRISB4
#define     SQEN_7566_DB4_DIR  TRISAbits.TRISA4
#define     SQEN_7566_DB5_DIR  TRISAbits.TRISA5
#define     SQEN_7566_DB6_DIR  TRISAbits.TRISA2
#define     SQEN_7566_DB7_DIR  TRISCbits.TRISC5

#endif
