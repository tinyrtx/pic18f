//*******************************************************************************
// tinyRTX Filename: uapp.h (User APPlication)
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
//  29May15 Stephen_Higgins@KairosAutonomi
//              Created from srtx.h.
//  09Jun15 Stephen_Higgins@KairosAutonomi.com
//              Added UAPP_BkgdTask().
//  22Jun15 Stephen_Higgins@KairosAutonomi.com
//              Added UAPP_PutByteRxBuffer(), UAPP_ParseRxMsg().
//
//*******************************************************************************

//  External prototypes.

extern	void	UAPP_POR_Init_PhaseA( void );
extern	void	UAPP_POR_Init_PhaseB( void );
extern	void	UAPP_BkgdTask( void );
extern	void	UAPP_Timer1Init( void );
extern	void	UAPP_TaskADC( void );
extern	void	UAPP_Task1( void );
extern	void	UAPP_Task2( void );
extern	void	UAPP_Task3( void );
extern	void	UAPP_PutByteRxBuffer( void );
extern	void	UAPP_ParseRxMsg( void );
