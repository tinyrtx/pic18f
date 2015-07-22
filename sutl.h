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
//
//*******************************************************************************
extern	void	SUTL_InvokeBootloader(void);
extern	void	SUTL_DisableBootloader(void);
