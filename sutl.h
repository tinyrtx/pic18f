//**********************************************************************************************
// tinyRTX Filename: sutl.h (System UTiLities)
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
//  25Aug14 SHiggins@tinyRTX.com  Create sutl.h from sutl.inc to use in uapp.c.
//              Note that SUTL_ComputedBraRCall and SUTL_ComputedGotoCall
//              cannot be called from C programs, therefore not included here.
//  03Sep15 Stephen_Higgins@KairosAutonomi.com
//              Move generic SUTL_Byte, SUTL_Word here.  Add SUTL_ShortLong.
//
//*******************************************************************************

// External type definitions.

typedef union
{
    struct
    {
        unsigned char bit0 : 1;
        unsigned char bit1 : 1;
        unsigned char bit2 : 1;
        unsigned char bit3 : 1;
        unsigned char bit4 : 1;
        unsigned char bit5 : 1;
        unsigned char bit6 : 1;
        unsigned char bit7 : 1;
    };
    struct
    {
        unsigned char nibble0 : 4;
        unsigned char nibble1 : 4;
    };
    unsigned char byte;
} SUTL_Byte;

typedef union
{
    struct
    {
        unsigned char nibble0 : 4;
        unsigned char nibble1 : 4;
        unsigned char nibble2 : 4;
        unsigned char nibble3 : 4;
    };
    struct
    {
        unsigned char byte0;
        unsigned char byte1;
    };
    unsigned int word;
} SUTL_Word;

typedef union
{
    struct
    {
        unsigned char nibble0 : 4;
        unsigned char nibble1 : 4;
        unsigned char nibble2 : 4;
        unsigned char nibble3 : 4;
        unsigned char nibble4 : 4;
        unsigned char nibble5 : 4;
    };
    struct
    {
        unsigned char byte0;
        unsigned char byte1;
        unsigned char byte2;
    };
    unsigned short long shortLong;
} SUTL_ShortLong;

// External variables.

// External prototypes.

extern	void	SUTL_InvokeBootloader(void);
extern	void	SUTL_DisableBootloader(void);
