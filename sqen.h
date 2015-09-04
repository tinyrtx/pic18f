//*******************************************************************************
// tinyRTX Filename: sqen.h (System Quadrature ENcoder services)
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
//              Created based upon LSI Computer Systems, Inc. document LS7566R
//              24-BIT x 4-AXES QUADRATURE COUNTER 7566R-072108.  No licensing
//              specified in that document.
//
//*******************************************************************************

// External type definitions.

// External variables.

// External prototypes.

extern void SQEN_7566_Init( void );

extern SUTL_Byte SQEN_7566_Read( SUTL_Byte SQEN_Channel,
                                 SUTL_Byte SQEN_Register );

extern void SQEN_7566_Write(    SUTL_Byte SQEN_Channel,
                                SUTL_Byte SQEN_Register,
                                SUTL_Byte SQEN_Data );

//*******************************************************************************
//  Channels.
//*******************************************************************************
#define SQEN_Chan0  0x00
#define SQEN_Chan1  0x01
#define SQEN_Chan2  0x02
#define SQEN_Chan3  0x03

//*******************************************************************************
//  Write-only command register.
//*******************************************************************************
#define SQEN_CMR        0x7

//*******************************************************************************
//  Bidirectional control/status registers.
//*******************************************************************************
#define SQEN_ISR_IMR    0x0
#define SQEN_MDR0       0x1
#define SQEN_MDR1       0x2

//*******************************************************************************
//  Read-only count registers.
//*******************************************************************************
#define SQEN_STR        0x3
#define SQEN_OL0        0x4
#define SQEN_OL1        0x5
#define SQEN_OL2        0x6

//*******************************************************************************
//  Write-only count pre-load registers.
//*******************************************************************************
#define SQEN_PR0        0x4
#define SQEN_PR1        0x5
#define SQEN_PR2        0x6

//*******************************************************************************
//  Control register bit definitions.
//*******************************************************************************

// IMR - Interrupt Mask Register.
#define SQEN_INT0               0x01
#define SQEN_INT1               0x02
#define SQEN_INT2               0x04
#define SQEN_INT3               0x08

// CMR - Command Register.
#define SQEN_RESET_CNTR         0x01
#define SQEN_LPAD_CNTR          0x02
#define SQEN_LOAD_OL            0x04
#define SQEN_RESET_STR          0x08    // CEN, U/D, S not affected.
#define SQEN_MASTER_RESET       0x10
#define SQEN_SET_SIGN           0x20
#define SQEN_RESET_SIGN         0x40
#define SQEN_RESET_ISR          0x80

// MDR0 - Counting Modes / INDX functionality.
#define SQEN_NO_QUAD            0x00
#define SQEN_QUAD_X1            0x01
#define SQEN_QUAD_X2            0x02
#define SQEN_QUAD_X4            0x03

#define SQEN_FREE_RUN           0x00
#define SQEN_SINGLE_CYCLE       0x04
#define SQEN_RANGE_LIMIT        0x08
#define SQEN_MODULO_N           0x0C

#define SQEN_DISABLE_IDX        0x00
#define SQEN_INDX_LOADS_CNTR    0x10
#define SQEN_INDX_RESETS_CNTR   0x20
#define SQEN_INDX_LOADS_OL      0x30

#define SQEN_SYNCH_IDX          0x40
#define SQEN_FILTER_DIV_2       0x80

// MDR1 - FLGa, FLGb functionality.
#define SQEN_FLGA_ENABLE_CY     0x01
#define SQEN_FLGA_ENABLE_BW     0x02
#define SQEN_FLGA_ENABLE_CMP    0x04
#define SQEN_FLGA_ENABLE_IDX    0x08

#define SQEN_FLGB_DISABLE       0x00
#define SQEN_FLGB_SIGN          0x10
#define SQEN_FLGB_UPDN          0x20

#define SQEN_DISABLE_CNTR       0x40
#define SQEN_FLGA_INSTANT       0x80