//*******************************************************************************
// tinyRTX Filename: sqen.c (System Quadrature ENcoder services)
//
// Copyright 2015 Kairos Autonomi.  ** www.tinyRTX.com **
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
//  01Sep15 Stephen_Higgins@KairosAutonomi.com
//              Created from scratch.
//  10Nov15 Stephen_Higgins@KairosAutonomi.com
//              Modified input and return parameters for cleaner invocation.
//  25Nov15 Stephen_Higgins@KairosAutonomi.com
//              Set direction bits before each input or output access.
//              Write to latch, read from port.
//
//*******************************************************************************

#include "ucfg.h"           // Configure board and proc, #include <proc.h>
#include "sqen.h"
#include "sqenuser.h"
#include "sutl.h"

// Internal prototypes.

//*******************************************************************************
//
// Read byte from 7566 register.  Bitwise access due to pin mapping.

unsigned char SQEN_7566_Read(   unsigned char SQEN_Channel,
                                unsigned char SQEN_Register )
{
SUTL_Byte SQEN_Channel_Struct;
SUTL_Byte SQEN_Register_Struct;
SUTL_Byte SQEN_Data;

SQEN_Channel_Struct.byte  = SQEN_Channel;
SQEN_Register_Struct.byte = SQEN_Register;

    SQEN_7566_CHS0 = SQEN_Channel_Struct.bit0;      // Set up channel address.
    SQEN_7566_CHS1 = SQEN_Channel_Struct.bit1;

    SQEN_7566_RS0  = SQEN_Register_Struct.bit0;     // Set up register address.
    SQEN_7566_RS1  = SQEN_Register_Struct.bit1;
    SQEN_7566_RS2  = SQEN_Register_Struct.bit2;

    SQEN_7566_DB0_DIR = 1;                  //  Set up port bits for reading.
    SQEN_7566_DB1_DIR = 1;
    SQEN_7566_DB2_DIR = 1;
    SQEN_7566_DB3_DIR = 1;
    SQEN_7566_DB4_DIR = 1;
    SQEN_7566_DB5_DIR = 1;
    SQEN_7566_DB6_DIR = 1;
    SQEN_7566_DB7_DIR = 1;

    SQEN_7566_WR   = 0b1;                   // Ensure WR high to enable read.
    SQEN_7566_RD   = 0b0;                   // Drive RD low to start read.

    _asm
    NOP                                     // Delay 200ns to ensure setup time for data.
    NOP
    _endasm

    SQEN_Data.bit0 = SQEN_7566_DB0_IN;      // Get read byte from data bus.
    SQEN_Data.bit1 = SQEN_7566_DB1_IN;
    SQEN_Data.bit2 = SQEN_7566_DB2_IN;
    SQEN_Data.bit3 = SQEN_7566_DB3_IN;
    SQEN_Data.bit4 = SQEN_7566_DB4_IN;
    SQEN_Data.bit5 = SQEN_7566_DB5_IN;
    SQEN_Data.bit6 = SQEN_7566_DB6_IN;
    SQEN_Data.bit7 = SQEN_7566_DB7_IN;

    SQEN_7566_RD  = 0b1;                    // Allow RD high to finish read.

    return SQEN_Data.byte;                  // Return data byte.
}

//*******************************************************************************
//
// Write byte to 7566 register.  Bitwise access due to pin mapping.

void SQEN_7566_Write(   unsigned char SQEN_Channel,
                        unsigned char SQEN_Register,
                        unsigned char SQEN_Data )
{
SUTL_Byte SQEN_Channel_Struct;
SUTL_Byte SQEN_Register_Struct;
SUTL_Byte SQEN_Data_Struct;

SQEN_Channel_Struct.byte  = SQEN_Channel;
SQEN_Register_Struct.byte = SQEN_Register;
SQEN_Data_Struct.byte     = SQEN_Data;

    SQEN_7566_CHS0 = SQEN_Channel_Struct.bit0;      // Set up channel address.
    SQEN_7566_CHS1 = SQEN_Channel_Struct.bit1;

    SQEN_7566_RS0  = SQEN_Register_Struct.bit0;     // Set up register address.
    SQEN_7566_RS1  = SQEN_Register_Struct.bit1;
    SQEN_7566_RS2  = SQEN_Register_Struct.bit2;

    SQEN_7566_DB0_DIR = 0;                          //  Set up port bits for writing.
    SQEN_7566_DB1_DIR = 0;
    SQEN_7566_DB2_DIR = 0;
    SQEN_7566_DB3_DIR = 0;
    SQEN_7566_DB4_DIR = 0;
    SQEN_7566_DB5_DIR = 0;
    SQEN_7566_DB6_DIR = 0;
    SQEN_7566_DB7_DIR = 0;

    SQEN_7566_DB0_OUT = SQEN_Data_Struct.bit0;      // Place write byte on data bus.
    SQEN_7566_DB1_OUT = SQEN_Data_Struct.bit1;
    SQEN_7566_DB2_OUT = SQEN_Data_Struct.bit2;
    SQEN_7566_DB3_OUT = SQEN_Data_Struct.bit3;
    SQEN_7566_DB4_OUT = SQEN_Data_Struct.bit4;
    SQEN_7566_DB5_OUT = SQEN_Data_Struct.bit5;
    SQEN_7566_DB6_OUT = SQEN_Data_Struct.bit6;
    SQEN_7566_DB7_OUT = SQEN_Data_Struct.bit7;

    SQEN_7566_RD   = 0b1;                   // Ensure RD high to enable write.
    SQEN_7566_WR   = 0b0;                   // Drive WR low to allow write.

    _asm
    NOP                                     // Delay 200ns to ensure setup time for data.
    NOP
    _endasm

    SQEN_7566_WR  = 0b1;                    // Drive WR high to clock data in.
}
