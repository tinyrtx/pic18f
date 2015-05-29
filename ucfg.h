//*******************************************************************************
// tinyRTX Filename: ucfg.inc (User ConFiGuration)
//
// Copyright 2015 Sycamore Software, Inc.  ** www.tinyRTX.com **
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
//  20May15 Stephen_Higgins@KairosAutonomi.com  Created from ucfg.inc.
//  28May15 Stephen_Higgins@KairosAutonomi.com
//              Move selection of board to assembler macro to keep w project.
//
//*******************************************************************************
//
//   Define all supported board configurations.
//
#define UCFG_PD2P_2002      0x01    // Microchip PICDEM2PLUS 2002 (has ext 4MHz)
#define UCFG_PD2P_2010      0x02    // Microchip PICDEM2PLUS 2010 (no ext 4MHz)
#define UCFG_DJPCB_280B     0x10    // Kairos Autonomi 280B: Utility Module I/O
//
//   Define all supported processors.
//
#define UCFG_18F452     0x01        // 40 pins.
#define UCFG_18F2620    0x02        // 28 pins.
//
//   Setting processor being used done below.
//
//*******************************************************************************
//
//    Output message of configuration of board and procesor.
//
    #ifdef __18F452
        #include "p18f452.h"
        #define UCFG_PROC   UCFG_18F452     // Allows logical expressions.
        #warning "Processor defined: 18F452"
    #endif       
    #ifdef __18F2620
        #include "p18f2620.h"
        #define UCFG_PROC   UCFG_18F2620    // Allows logical expressions.
        #warning "Processor defined: 18F2620"
    #endif       
//           
    #ifdef _UCFG_PD2P02
        #define UCFG_BOARD  UCFG_PD2P_2002  // Allows logical expressions.
        #warning "Board defined: Microchip PICDEM2PLUS 2002"
    #endif       
    #ifdef _UCFG_PD2P10
        #define UCFG_BOARD  UCFG_PD2P_2010  // Allows logical expressions.
        #warning "Board defined: Microchip PICDEM2PLUS 2010"
    #endif       
    #ifdef _UCFG_KA280B
        #define UCFG_BOARD  UCFG_DJPCB_280B // Allows logical expressions.
        #warning "Board defined: KA DJPCB_280B"
    #endif       
