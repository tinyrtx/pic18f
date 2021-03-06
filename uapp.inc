        LIST
;**********************************************************************************************
; tinyRTX Filename: uapp.inc (User APPlication interface)
;
; Copyright 2014 Sycamore Software, Inc.  ** www.tinyRTX.com **
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
; Revision history:
;  31Oct03  SHiggins@tinyRTX.com  Created from scratch.
;  29Jul14  SHiggins@tinyRTX.com  Moved UAPP_Timer1Init to MACRO to save stack.
;  29Apr15  Stephen_Higgins@KairosAutonomi.com
;               Added UAPP_POR_Init_PhaseA.
;   06May15 Stephen_Higgins@KairosAutonomi.com
;               Moved UAPP_Timer1Init from MACRO to allow conditional assembly.
;   09Jun15 Stephen_Higgins@KairosAutonomi.com  
;               Added UAPP_BkgdTask.
;   22Jun15 Stephen_Higgins@KairosAutonomi.com
;               Added UAPP_PutByteRxBuffer(), UAPP_ParseRxMsg().
;   10Nov15 Stephen_Higgins@KairosAutonomi.com
;               Change Rx names to Rc for internal consistency.
;	20Jan16 Stephen_Higgins@KairosAutonomi.com
;               Add UAPP_ISR_PWStateMachineInt0 and UAPP_ISR_PWStateMachineInt1.
;	04Feb17 Stephen_Higgins@KairosAutonomi.com
;               Add IF UCFG_BOARD==UCFG_KA107I for UAPP_ISR_PWStateMachineInt0/1.
;
;*********************************************************************************************
        NOLIST
;
        EXTERN      UAPP_POR_Init_PhaseA
        EXTERN      UAPP_POR_Init_PhaseB
        EXTERN      UAPP_BkgdTask
        EXTERN      UAPP_Timer1Init
        EXTERN      UAPP_TaskADC
        EXTERN      UAPP_Task1
        EXTERN      UAPP_Task2
        EXTERN      UAPP_Task3
        EXTERN      UAPP_PutByteRcBuffer
        EXTERN      UAPP_ParseRcMsg
;
        IF UCFG_BOARD==UCFG_KA107I
        EXTERN      UAPP_ISR_PWStateMachineInt0
        EXTERN      UAPP_ISR_PWStateMachineInt1
        ENDIF
;
        LIST
