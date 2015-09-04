//**********************************************************************************************
// Filename: slcduser.inc (System Liquid Crystal Display - USER configuration)
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
//
//*********************************************************************************************

// SQEN configuration is defined by these #defines.
// User should set them to match hardware configuration.
// NOTE: User is also responsible for proper hardware initialization so
//   hardware addressed by these #defines may be used.

#if UCFG_BOARD==UCFG_KA107I

#define     SQEN_7566_WR       LATAbits.LATA3
#define     SQEN_7566_RD       LATAbits.LATA1

#define     SQEN_7566_CHS0     LATCbits.LATC4
#define     SQEN_7566_CHS1     LATCbits.LATC3

#define     SQEN_7566_RS0      LATCbits.LATC2
#define     SQEN_7566_RS1      LATCbits.LATC1
#define     SQEN_7566_RS2      LATCbits.LATC0

#define     SQEN_7566_DB0      LATBbits.LATB7
#define     SQEN_7566_DB1      LATBbits.LATB6
#define     SQEN_7566_DB2      LATBbits.LATB5
#define     SQEN_7566_DB3      LATBbits.LATB4
#define     SQEN_7566_DB4      LATAbits.LATA4
#define     SQEN_7566_DB5      LATAbits.LATA5
#define     SQEN_7566_DB6      LATAbits.LATA2
#define     SQEN_7566_DB7      LATCbits.LATC5

#endif
        LIST
