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
//  02Sep15 Stephen_Higgins@KairosAutonomi.com
//              Created from scratch.
//
//*******************************************************************************

#include "sqen.h"
#include "sutl.h"

//*******************************************************************************
//
// User application Power-On Reset initialization.

void UQEN_Init( void )
{
SUTL_Byte UQEN_Channel;
SUTL_Byte UQEN_Register;
SUTL_Byte UQEN_Command;

    // Reset all four channels.

    // Oh my heck this is so painful, but can't place literals directly in arg
    // list as C18 doesn't support compound literals.  This run time penalty
    // incurred as tradeoff for elegance of SQEN_7566_Write() implementation
    // when receiving SUTL_Byte parameters.
    // Preferred form: SQEN_7566_Write( SQEN_CHAN0, SQEN_CMR, SQEN_MASTER_RESET );

    UQEN_Channel.byte  = SQEN_CHAN0;
    UQEN_Register.byte = SQEN_CMR;
    UQEN_Command.byte  = SQEN_MASTER_RESET;
    SQEN_7566_Write( UQEN_Channel, UQEN_Register, UQEN_Command );

    UQEN_Channel.byte  = SQEN_CHAN1;
    SQEN_7566_Write( UQEN_Channel, UQEN_Register, UQEN_Command );

    UQEN_Channel.byte  = SQEN_CHAN2;
    SQEN_7566_Write( UQEN_Channel, UQEN_Register, UQEN_Command );

    UQEN_Channel.byte  = SQEN_CHAN3;
    SQEN_7566_Write( UQEN_Channel, UQEN_Register, UQEN_Command );

    // Set all four counting modes to one count per quad cycle.

    UQEN_Channel.byte  = SQEN_CHAN0;
    UQEN_Register.byte = SQEN_MDR0;
    UQEN_Command.byte  = SQEN_QUAD_X1;
    SQEN_7566_Write( UQEN_Channel, UQEN_Register, UQEN_Command );

    UQEN_Channel.byte  = SQEN_CHAN1;
    SQEN_7566_Write( UQEN_Channel, UQEN_Register, UQEN_Command );

    UQEN_Channel.byte  = SQEN_CHAN2;
    SQEN_7566_Write( UQEN_Channel, UQEN_Register, UQEN_Command );

    UQEN_Channel.byte  = SQEN_CHAN3;
    SQEN_7566_Write( UQEN_Channel, UQEN_Register, UQEN_Command );
}

void UQEN_LoadOL_All( void )
{
SUTL_Byte UQEN_Channel;
SUTL_Byte UQEN_Register;
SUTL_Byte UQEN_Command;

    // Cause all four channels to load their OL from their CNTR.

    UQEN_Channel.byte  = SQEN_CHAN0;
    UQEN_Register.byte = SQEN_CMR;
    UQEN_Command.byte  = SQEN_LOAD_OL;
    SQEN_7566_Write( UQEN_Channel, UQEN_Register, UQEN_Command );

    UQEN_Channel.byte  = SQEN_CHAN1;
    SQEN_7566_Write( UQEN_Channel, UQEN_Register, UQEN_Command );

    UQEN_Channel.byte  = SQEN_CHAN2;
    SQEN_7566_Write( UQEN_Channel, UQEN_Register, UQEN_Command );

    UQEN_Channel.byte  = SQEN_CHAN3;
    SQEN_7566_Write( UQEN_Channel, UQEN_Register, UQEN_Command );
}
