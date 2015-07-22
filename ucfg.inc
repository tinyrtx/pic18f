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
; NOTE: This file must be (manually) kept in synch with ucfg.h !!!
; ****************************************************************
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
;
;*******************************************************************************
        NOLIST
;
;   Define all supported board configurations.
;
#define UCFG_PD2P_2002      0x01    ; Microchip PICDEM2PLUS 2002 (has ext 4MHz)
#define UCFG_PD2P_2010      0x02    ; Microchip PICDEM2PLUS 2010 (no ext 4MHz)
#define UCFG_KA280BI        0x10    ; Kairos Autonomi 280B: Utility Module I/O
#define UCFG_KA280BT        0x11    ; Kairos Autonomi 280B: Transmission & I/O
;
;   Define all supported processors.
;
#define UCFG_18F452     0x01        ; 40 pins.
#define UCFG_18F2620    0x02        ; 28 pins.
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
        MESSG "Processor defined: 18F452"
    ENDIF
    IFDEF __18F2620
        #include <p18f2620.inc>
        #define UCFG_PROC   UCFG_18F2620    ; Allows logical expressions.
        MESSG "Processor defined: 18F2620"
    ENDIF

    IFDEF _UCFG_PD2P02
        #define UCFG_BOARD  UCFG_PD2P_2002  ; Allows logical expressions.
        #define UCFG_SSIO_EOMC  0x0d        ; End Of Msg Char = <CR>
        MESSG "Board defined: Microchip PICDEM2PLUS 2002"
    ENDIF
    IFDEF _UCFG_PD2P10
        #define UCFG_BOARD  UCFG_PD2P_2010  ; Allows logical expressions.
        #define UCFG_SSIO_EOMC  0x0d        ; End Of Msg Char = <CR>
        MESSG "Board defined: Microchip PICDEM2PLUS 2010"
    ENDIF
    IFDEF _UCFG_KA280BI
        #define UCFG_BOARD  UCFG_KA280BI    ; Allows logical expressions.
        #define UCFG_SSIO_EOMC  0x5d        ; End Of Msg Char = "]"
        MESSG "Board defined: KA280BI"
    ENDIF
    IFDEF _UCFG_KA280BT
        #define UCFG_BOARD  UCFG_KA280BT    ; Allows logical expressions.
        #define UCFG_SSIO_EOMC  0x5d        ; End Of Msg Char = "]"
        MESSG "Board defined: KA280BT"
    ENDIF

;*******************************************************************************
;
;   Set CONFIG bits. Valid values found in <"processor".inc>, e.g. <p18f2620.inc>.
;
;*******************************************************************************
;
;   UCFG_18F452 specified. Board doesn't matter (currently) so common to all boards.
;   ********************************************************************************

    IF UCFG_PROC==UCFG_18F452
        CONFIG  OSC = RCIO          ; RC oscillator w/ OSC2 configured as RA6
        CONFIG  OSCS = OFF          ; Oscillator system clock switch option is disabled (main oscillator is source)
        CONFIG  BOR = ON            ; Brown-out Reset enabled
        CONFIG  BORV = 20           ; VBOR set to 2.0V
        CONFIG  CCP2MUX = ON        ; CCP2 input/output is multiplexed with RC1
        CONFIG  STVR = ON           ; Stack full/underflow will cause Reset
    ENDIF

;   UCFG_18F2620 specified.
;   ***********************

    IF UCFG_PROC==UCFG_18F2620

;       UCFG_PD2P_2002 specified.
;       *************************

        IF UCFG_BOARD==UCFG_PD2P_2002
        CONFIG  OSC = RCIO6         ; External RC oscillator, port function on RA6
        ENDIF

;       UCFG_PD2P_2010 specified.
;       *************************

        IF UCFG_BOARD==UCFG_PD2P_2010
        CONFIG  OSC = INTIO67       ; Internal oscillator block, port function on RA6 and RA7
        ENDIF

;       UCFG_KA280BI or UCFG_KA280BT specified.
;       ***************************************

        IF UCFG_BOARD==UCFG_KA280BI || UCFG_BOARD==UCFG_KA280BT
        CONFIG  OSC = HSPLL         ; HS oscillator, PLL enabled (Clock Frequency = 4 x Fosc1)
        ENDIF

;       UCFG_18F2620 common to all boards.
;       **********************************

        CONFIG  FCMEN = OFF         ; Fail-Safe Clock Monitor disabled
        CONFIG  IESO = OFF          ; Oscillator Switchover mode disabled
        CONFIG  BOREN = SBORDIS     ; Brown-out Reset enabled in hardware only (SBOREN is disabled)
        CONFIG  BORV = 3            ; Minimum setting
        CONFIG  CCP2MX = PORTC      ; CCP2 input/output is multiplexed with RC1
        CONFIG  PBADEN = ON         ; PORTB<4:0> pins are configured as analog input channels on Reset
        CONFIG  LPT1OSC = OFF       ; Timer1 configured for higher power operation
        CONFIG  MCLRE = ON          ; MCLR pin enabled; RE3 input pin disabled
        CONFIG  STVREN = ON         ; Stack full/underflow will cause Reset
        CONFIG  XINST = OFF         ; Instruction set extension and Indexed Addressing mode disabled (Legacy mode)
    ENDIF

;******************************************************************************
;
;   CONFIG common to all processors and all boards.
;   ***********************************************

        CONFIG  PWRT = OFF          ; PWRT disabled
        CONFIG  WDT = OFF           ; WDT disabled (control is placed on the SWDTEN bit)
        CONFIG  WDTPS = 128         ; 1:128
        CONFIG  LVP = OFF           ; Single-Supply ICSP disabled
        CONFIG  CP0 = OFF           ; Block 0 (000800-003FFFh) not code-protected
        CONFIG  CP1 = OFF           ; Block 1 (004000-007FFFh) not code-protected
        CONFIG  CP2 = OFF           ; Block 2 (008000-00BFFFh) not code-protected
        CONFIG  CP3 = OFF           ; Block 3 (00C000-00FFFFh) not code-protected
        CONFIG  CPB = OFF           ; Boot block (000000-0007FFh) not code-protected
        CONFIG  CPD = OFF           ; Data EEPROM not code-protected
        CONFIG  WRT0 = OFF          ; Block 0 (000800-003FFFh) not write-protected
        CONFIG  WRT1 = OFF          ; Block 1 (004000-007FFFh) not write-protected
        CONFIG  WRT2 = OFF          ; Block 2 (008000-00BFFFh) not write-protected
        CONFIG  WRT3 = OFF          ; Block 3 (00C000-00FFFFh) not write-protected
        CONFIG  WRTC = OFF          ; Configuration registers (300000-3000FFh) not write-protected
        CONFIG  WRTB = OFF          ; Boot Block (000000-0007FFh) not write-protected
        CONFIG  WRTD = OFF          ; Data EEPROM not write-protected
        CONFIG  EBTR0 = OFF         ; Block 0 (000800-003FFFh) not protected from table reads executed in other blocks
        CONFIG  EBTR1 = OFF         ; Block 1 (004000-007FFFh) not protected from table reads executed in other blocks
        CONFIG  EBTR2 = OFF         ; Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks
        CONFIG  EBTR3 = OFF         ; Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks
        CONFIG  EBTRB = OFF         ; Boot Block (000000-0007FFh) not protected from table reads executed in other blocks;

;*******************************************************************************
        LIST
