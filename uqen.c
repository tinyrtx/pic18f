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

// Internal prototypes. ???

//*******************************************************************************
//
// User application Power-On Reset initialization.

void UQEN_Init( void )
{
    // Reset all four channels.
    SQEN_7566_Write( SQEN_Chan0, SQEN_CMR, SQEN_MASTER_RESET );
    SQEN_7566_Write( SQEN_Chan1, SQEN_CMR, SQEN_MASTER_RESET );
    SQEN_7566_Write( SQEN_Chan2, SQEN_CMR, SQEN_MASTER_RESET );
    SQEN_7566_Write( SQEN_Chan3, SQEN_CMR, SQEN_MASTER_RESET );

    // Set all four counting modes to one count per quad cycle.
    SQEN_7566_Write( SQEN_Chan0, SQEN_MDR0, SQEN_QDX1 );
    SQEN_7566_Write( SQEN_Chan1, SQEN_MDR0, SQEN_QDX1 );
    SQEN_7566_Write( SQEN_Chan2, SQEN_MDR0, SQEN_QDX1 );
    SQEN_7566_Write( SQEN_Chan3, SQEN_MDR0, SQEN_QDX1 );

//  for i = 0 to 15          ' Age wheel counters
//    owheels(i) = wheels(i)
//    next
}

void UQEN_LoadOL_All( void )
{
    // Cause all four channels to load their OL from their CNTR.
    SQEN_7566_Write( SQEN_Chan0, SQEN_CMR, SQEN_LOAD_OL );
    SQEN_7566_Write( SQEN_Chan1, SQEN_CMR, SQEN_LOAD_OL );
    SQEN_7566_Write( SQEN_Chan2, SQEN_CMR, SQEN_LOAD_OL );
    SQEN_7566_Write( SQEN_Chan3, SQEN_CMR, SQEN_LOAD_OL );
}
