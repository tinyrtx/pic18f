//*******************************************************************************
// tinyRTX Filename: si2c.h (System I2C communication services)
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
//   17Apr15  Stephen_Higgins@KairosAutonomi.com
//               Created from si2c.inc.
//   28May15  Stephen_Higgins@KairosAutonomi.com
//               Create si2c.h from ssio.h to use in sisd.c.
//
//*******************************************************************************

// External type definitions.

// External variables.

// External prototypes.

extern	void	SI2C_Tbl_HwState( void );

// Following routines do not need to be called from C yet.

//extern	void	SI2C_Msg_Write(void);
//extern	void	SI2C_Msg_Wrt_Rd(void);
