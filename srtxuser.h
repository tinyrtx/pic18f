//*******************************************************************************
// tinyRTX Filename: srtxuser.h (System Real Time eXecutive to USER interface)
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
//  17Oct03 SHiggins@tinyRTX.com  Created from scratch.
//  30Jul14 SHiggins@tinyRTX.com  Reduce from 4 tasks to 3 to reduce stack needs.
//  25Aug14 SHiggins@tinyRTX.com  Create srtxuser.h from srtxuser.inc to use in srtxuser.c.
//  09Jun15 Stephen_Higgins@KairosAutonomi.com
//              Properly define SRTX_CNT_RELOAD_TASK1/2/3 if using Timer1 at 50ms.
//              I'm thinking these defines should move to UAPP or UCFG.
//  30Jul15 Stephen_Higgins@KairosAutonomi.com
//              Combine UCFG_KA280BI and UCFG_KA280BT into UCFG_DJPCB_280B.
//  27Aug15 Stephen_Higgins@KairosAutonomi.com
//              Split UCFG_DJPCB_280B into UCFG_KA280BI and UCFG_KA280BT. (again)
//              SRTX timebase from 50ms(20Hz) to 10ms(100Hz) for UCFG_KA280BI/T.
//
//*******************************************************************************
#include    "ucfg.h"

// Counts to load task timer at initialization (must be non-zero.)
// Each task will be first scheduled at the SRTX-timer event which
//   occurs as equated below.  However, please note that immediately
//   after initialization is complete, a single "faux" SRTX-timer
//   event occurs, which allows all tasks equated to "1" below to run.
// Allowed range is (1 - 255).

#define	SRTX_CNT_INIT_TASK1 1
#define SRTX_CNT_INIT_TASK2 1
#define SRTX_CNT_INIT_TASK3 1

// Counts to reload task timer each expiration.  This is the number of
//   SRTX timer events which must occur before the task is again scheduled.
// SRTX timer events occur every 100.0 ms or 10.0 ms, depending on board.
// Allowed range is (0-255).

#if (UCFG_BOARD == UCFG_PD2P_2002) || (UCFG_BOARD == UCFG_PD2P_2010)
    #define	SRTX_CNT_RELOAD_TASK1	0x01    //  1 = 100 ms
    #define	SRTX_CNT_RELOAD_TASK2	0x0a    // 10 = 1.000 sec
    #define	SRTX_CNT_RELOAD_TASK3	0x32    // 50 = 5.000 sec
#elif (UCFG_BOARD==UCFG_KA280BI) || (UCFG_BOARD==UCFG_KA280BT)
    #define	SRTX_CNT_RELOAD_TASK1	0x01    //   1 =    10 ms
    #define	SRTX_CNT_RELOAD_TASK2	0x0A    //  10 =   100 ms
    #define	SRTX_CNT_RELOAD_TASK3	0x64    // 100 = 1.000 sec
#endif
#warning "SRTX CNT RELOAD TASK1: SRTX_CNT_RELOAD_TASK1"

