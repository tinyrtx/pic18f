        LIST
;*******************************************************************************
; tinyRTX Filename: ucfg.inc (User ConFiGuration)
;
; Copyright 2015 Sycamore Software, Inc.  ** www.tinyRTX.com **
; Distributed under the terms of the GNU Lesser General Purpose License v3
;
; This file is part of tinyRTX. tinyRTX is free software: you can redistribute
; it and/or modify it under the terms of the GNU Lesser General Public License
; version 3 as published by the Free Software Foundation.
;
; tinyRTX is distributed in the hope that it will be useful, but WITHOUT ANY
; WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
; A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
; details.
;
; You should have received a copy of the GNU Lesser General Public License
; (filename copying.lesser.txt) and the GNU General Public License (filename
; copying.txt) along with tinyRTX.  If not, see <http://www.gnu.org/licenses/>.
;
;*******************************************************************************
;
; Revision history:
;   06May15  Stephen_Higgins@KairosAutonomi.com  Created from scratch.
;   28May15  Stephen_Higgins@KairosAutonomi.com
;               Move selection of board to assembler macro to keep w project.
;   09Jul15 Stephen_Higgins@KairosAutonomi
;               Split UCFG_DJPCB_280B into
;               UCFG_KA280BI (I/O) and UCFG_KA280BT (Transmission).
;   13Jul15 Stephen_Higgins@KairosAutonomi.com
;               Replace hardcoded <CR> with UCFG_SSIO_EOMC (End Of Msg Char) so
;               incoming msg string may be terminated by any character.
;   22Jul15 Stephen_Higgins@KairosAutonomi.com
;               Moved all CONFIG here as CONFIG also needed in SBTL.
;               Then moved them to uapp_ucfg.inc to only incl in UAPP and SBTL.
;   30Jul15 Stephen_Higgins@KairosAutonomi.com
;               Combine UCFG_KA280BI and UCFG_KA280BT into UCFG_DJPCB_280B.
;   27Aug15 Stephen_Higgins@KairosAutonomi.com
;               Split UCFG_DJPCB_280B into UCFG_KA280BI and UCFG_KA280BT. (again)
;   04Sep15 Stephen_Higgins@KairosAutonomi.com
;               Add UCFG_KA107I.
;   10Nov15 Stephen_Higgins@KairosAutonomi.com
;               Add UCFG_FSR generic FSR.
;   19Feb16 Stephen_Higgins@KairosAutonomi.com
;               Add compile time messages for IFDEF __DEBUG.
;   08Feb17 Stephen_Higgins@KairosAutonomi.com
;               Use UCFG_SSIO_SOMC (Start Of Msg Char) if UCFG_SSIO_SOMC_ACTIVE
;               to discard all characters before SOMC received.
;   09Feb17 Stephen_Higgins@KairosAutonomi.com
;               Add UCFG_KA280BT_ARGO using _UCFG_KA280BI settings for now.
;
;*******************************************************************************
; NOTE: This file MUST be (manually) kept in synch with ucfg.h !!!
;*******************************************************************************
        NOLIST
;
;   Define all supported board configurations.
;
#define UCFG_PD2P_2002      0x01    ; Microchip PICDEM2PLUS 2002 (has ext 4MHz)
#define UCFG_PD2P_2010      0x02    ; Microchip PICDEM2PLUS 2010 (no ext 4MHz)
#define UCFG_KA107I         0x10    ; Kairos Autonomi 107I: Quad and Video Mux
#define UCFG_KA280BI        0x11    ; Kairos Autonomi 280B: Utility Module I/O
#define UCFG_KA280BT        0x12    ; Kairos Autonomi 280B: Transmission & I/O
;
;   Define all supported processors.
;
#define UCFG_18F452     0x01        ; 40 pins.
#define UCFG_18F2620    0x02        ; 28 pins.
;
;   Define logical true and false.
;
#define UCFG_FALSE  0
#define UCFG_TRUE   0xFF
;
;*******************************************************************************
;
    IFDEF __DEBUG
        MESSG "_DEBUG defined: Debug build"
    ELSE
        MESSG "_DEBUG not defined: Release build"
    ENDIF
;
;*******************************************************************************
;
;   Include appropriate processor definition file, define UCFG_PROC.
;   Define UCFG_BOARD, set UCFG_SSIO_EOMC.
;   Output message of configuration of board and procesor.
;
    IFDEF __18F452
        #include <p18f452.inc>
        #define UCFG_PROC   UCFG_18F452     ; Allows logical expressions.
        #define UCFG_FSR    FSR             ; Generic FSR is (only) FSR.
        MESSG "Processor defined: 18F452"
    ENDIF
    IFDEF __18F2620
        #include <p18f2620.inc>
        #define UCFG_PROC   UCFG_18F2620    ; Allows logical expressions.
        #define UCFG_FSR    FSR0            ; Generic FSR is FSR0.
        MESSG "Processor defined: 18F2620"
    ENDIF

    IFDEF _UCFG_PD2P02
        #define UCFG_BOARD  UCFG_PD2P_2002  ; Allows logical expressions.
        #define UCFG_SSIO_EOMC  0x0d        ; End Of Msg Char = <CR>
        #define UCFG_SSIO_SOMC_ACTIVE   UCFG_FALSE  ; Don't use SOMC.
        MESSG "Board defined: Microchip PICDEM2PLUS 2002"
    ENDIF
    IFDEF _UCFG_PD2P10
        #define UCFG_BOARD  UCFG_PD2P_2010  ; Allows logical expressions.
        #define UCFG_SSIO_EOMC  0x0d        ; End Of Msg Char = <CR>
        #define UCFG_SSIO_SOMC_ACTIVE   UCFG_FALSE  ; Don't use SOMC.
        MESSG "Board defined: Microchip PICDEM2PLUS 2010"
    ENDIF
    IFDEF _UCFG_KA107I
        #define UCFG_BOARD              UCFG_KA107I ; Allows logical expressions.
        #define UCFG_SSIO_SOMC_ACTIVE   UCFG_TRUE   ; Use SOMC.
        #define UCFG_SSIO_SOMC          0x5B        ; Start Of Msg Char = "["
        #define UCFG_SSIO_EOMC          0x5d        ; End Of Msg Char = "]"
        MESSG "Board defined: KA107I"
    ENDIF
    IFDEF _UCFG_KA280BI
        #define UCFG_BOARD              UCFG_KA280BI; Allows logical expressions.
        #define UCFG_SSIO_SOMC_ACTIVE   UCFG_TRUE   ; Use SOMC.
        #define UCFG_SSIO_SOMC          0x5B        ; Start Of Msg Char = "["
        #define UCFG_SSIO_EOMC          0x5d        ; End Of Msg Char = "]"
        MESSG "Board defined: KA280BI"
    ENDIF
    IFDEF _UCFG_KA280BT
        #define UCFG_BOARD              UCFG_KA280BT; Allows logical expressions.
        #define UCFG_SSIO_SOMC_ACTIVE   UCFG_TRUE   ; Use SOMC.
        #define UCFG_SSIO_SOMC          0x5B        ; Start Of Msg Char = "["
        #define UCFG_SSIO_EOMC          0x5d        ; End Of Msg Char = "]"
        MESSG "Board defined: KA280BT"
    ENDIF
    IFDEF _UCFG_KA280BT_ARGO
        #define UCFG_BOARD              UCFG_KA280BI; Allows logical expressions.
        #define UCFG_SSIO_SOMC_ACTIVE   UCFG_TRUE   ; Use SOMC.
        #define UCFG_SSIO_SOMC          0x5B        ; Start Of Msg Char = "["
        #define UCFG_SSIO_EOMC          0x5d        ; End Of Msg Char = "]"
        MESSG "Board defined: KA280BT ARGO"
    ENDIF

        LIST
