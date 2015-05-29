//*******************************************************************************
// tinyRTX Filename: usio.h (User Serial I/O communication services)
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
//  17Apr15 Stephen_Higgins@KairosAutonomi.com
//              Created from ui2c.inc.
//  27Apr15 Stephen_Higgins@KairosAutonomi.com
//              Added USIO_TxLCDMsgToSIO.
//  29May15 Stephen_Higgins@KairosAutonomi.com
//              Converted from usio.inc.
//
//*******************************************************************************
//
extern	void	USIO_Init( void );
extern	void	USIO_TxLCDMsgToSIO( void );
extern	void	USIO_MsgReceived( void );
