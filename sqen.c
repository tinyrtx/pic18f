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
//
//*******************************************************************************

// Internal prototypes.

//*******************************************************************************
//
// Write byte to 7566 register.

void SQEN_7566_Write(   SUTL_Byte SQEN_Channel,
                        SUTL_Byte SQEN_Register,
                        SUTL_Byte SQEN_Data )
{
    SQEN_7566_CH0 = SQEN_Channel.bit0;      // Set up channel address.
    SQEN_7566_CH1 = SQEN_Channel.bit1;

    SQEN_7566_RS0 = SQEN_Register.bit0;     // Set up register address.
    SQEN_7566_RS1 = SQEN_Register.bit1;
    SQEN_7566_RS2 = SQEN_Register.bit2;

    SQEN_7566_DB0 = SQEN_Data.bit0;         // Place write byte on data bus.
    SQEN_7566_DB1 = SQEN_Data.bit1;         // (Bitwise access due to different ports.)
    SQEN_7566_DB2 = SQEN_Data.bit2;
    SQEN_7566_DB3 = SQEN_Data.bit3;
    SQEN_7566_DB4 = SQEN_Data.bit4;
    SQEN_7566_DB5 = SQEN_Data.bit5;
    SQEN_7566_DB6 = SQEN_Data.bit6;
    SQEN_7566_DB7 = SQEN_Data.bit7;

    SQEN_7566_RD  = 0b1;                    // Ensure RD high to enable write.
    SQEN_7566_WR  = 0b0;                    // Drive WR low to allow write.

                                            // Delay 100ns to ensure setup time for data.
_asm
    NOP
_endasm

    SQEN_7566_WR  = 0b1;                    // Drive WR high to clock data in.
}

//*******************************************************************************
//
// Read byte from 7566 register.

SUTL_Byte SQEN_7566_Read( SUTL_Byte SQEN_Channel,
                          SUTL_Byte SQEN_Register )
{
SUTL_Byte SQEN_Data;

    SQEN_7566_CH0 = SQEN_Channel.bit0;      // Set up channel address.
    SQEN_7566_CH1 = SQEN_Channel.bit1;

    SQEN_7566_RS0 = SQEN_Register.bit0;     // Set up register address.
    SQEN_7566_RS1 = SQEN_Register.bit1;
    SQEN_7566_RS2 = SQEN_Register.bit2;

    SQEN_7566_WR  = 0b1;                    // Ensure WR high to enable read.
    SQEN_7566_RD  = 0b0;                    // Drive RD low to start read.

                                            // Delay 100ns to ensure setup time for data.
_asm
    NOP
_endasm

    SQEN_Data.bit0 = SQEN_7566_DB0;         // Get read byte from data bus.
    SQEN_Data.bit1 = SQEN_7566_DB1;         // (Bitwise access due to different ports.)
    SQEN_Data.bit2 = SQEN_7566_DB2;
    SQEN_Data.bit3 = SQEN_7566_DB3;
    SQEN_Data.bit4 = SQEN_7566_DB4;
    SQEN_Data.bit5 = SQEN_7566_DB5;
    SQEN_Data.bit6 = SQEN_7566_DB6;
    SQEN_Data.bit7 = SQEN_7566_DB7;

    SQEN_7566_RD  = 0b1;                    // Allow RD high to finish read.

    return SQEN_Data;                       // Return data byte.
}
