;*******************************************************************************
; tinyRTX Filename: susr.asm (System USeR interface)
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
; Revision History:
;  31Oct03  SHiggins@tinyRTX.com  Created to isolate user vars and routine calls.
;  24Feb04  SHiggins@tinyRTX.com  Add trace.
;  29Jul14  SHiggins@tinyRTX.com  Moved UAPP_Timer1Init to MACRO to save stack.
;  30Jul14  SHiggins@tinyRTX.com  Reduce from 4 tasks to 3 to reduce stack needs.
;  12Aug14  SHiggins@tinyRTX.com  Converted from PIC16877 to PIC18F452.
;  02Sep14  SHiggins@tinyRTX.com    Removed smTrace calls in SUSR_TaskI2C.
;                                   Although smTrace has int protection, it does not
;                                   seem to be sufficient, and
;                                   calling smTrace in an ISR such as TaskI2C clobbers
;                                   both the trace buffer and RAM.  Unresolved.
;  03Sep14  SHiggins@tinyRTX.com    Rename SUSR_TaskI2C to SUSR_ISR_I2C, remove SUSR_UdataSec.
;                                   New SUSR_TaskI2C now invokes UI2C_MsgTC74ProcessData.
;   14Apr15 Stephen_Higgins@KairosAutonomi.com
;               Converted from PIC18F452 to PIC18F2620.
;   17Apr15 Stephen_Higgins@KairosAutonomi.com
;               Added usio.inc, SSIO_Init, UAPP_POR_Init_PhaseA.
;   27Apr15 Stephen_Higgins@KairosAutonomi.com
;               Call USIO_TxLCDMsgToSIO instead of SLCD_RefreshLine2.
;               Removed unnecessary banksel's for SFR's in access RAM.
;   06May15 Stephen_Higgins@KairosAutonomi.com
;               Use conditional compiles for various processors and boards.
;               Spit out task markers on RS-232 for state of health.
;               Substitute #include <ucfg.inc> for <p18f452.inc>.
;   22May15 Stephen_Higgins@KairosAutonomi.
;               Only call UI2C_MsgTC74ProcessData and UI2C_MsgDone when board supports it.
;   28May15 Stephen_Higgins@KairosAutonomi.
;               Replace redirecting calls without traces with goto's.
;               Move most logic out of here to UAPP.asm.
;               Deleted SUSR_ISR_I2C, now SISD calls SI2C_Tbl_HwState directly.
;   09Jun15 Stephen_Higgins@KairosAutonomi.com  
;               Add SUSR_BkgdTask.
;   20Jan16 Stephen_Higgins@KairosAutonomi.com
;               Add SUSR_ISR_INT0 and SUSR_ISR_INT1.
;
;*******************************************************************************
;
        errorlevel -302 
;
        #include <ucfg.inc>          ; includes processor definitions.
        #include <strc.inc>
        #include <uapp.inc>
        #include <usio.inc>
;
    IF UCFG_BOARD==UCFG_PD2P_2002 || UCFG_BOARD==UCFG_PD2P_2010
        #include <ui2c.inc>
    ENDIF
;
;*******************************************************************************
;
; SUSR: System to User Redirection.
;
; These routines provide the interface from the SRTX (System Real Time eXecutive)
;   and SISD (Interrupt Service Routine Director) to user application code.
;
SUSR_CodeSec    CODE
;
; User initialization at Power-On Reset Phase A.
;  Application time-critical initialization, no interrupts.
;
        GLOBAL  SUSR_POR_PhaseA
SUSR_POR_PhaseA
        goto    UAPP_POR_Init_PhaseA    ; User app POR time-critical init.
;
;*******************************************************************************
;
; User initialization at Power-On Reset Phase B.
;  Application non-time critical initialization.
;
        GLOBAL  SUSR_POR_PhaseB
SUSR_POR_PhaseB
        goto    UAPP_POR_Init_PhaseB    ; User app POR Init. (Enables global and peripheral ints.)
;
;*******************************************************************************
;
; User initialization of timebase timer and corresponding interrupt.
;
        GLOBAL  SUSR_Timebase
SUSR_Timebase
        goto    UAPP_Timer1Init         ; Re-init Timer1 so new Timer1 int in 100ms.
;
; UAPP_Timer1Init enabled Timer1 interrupts.
;
;*******************************************************************************
;
;   Background task measures PWM. (User initialization of Timer0 (no interrupts.))
;
        GLOBAL  SUSR_BkgdTask
SUSR_BkgdTask
        goto    UAPP_BkgdTask         ; User background task.
        return
;
;*******************************************************************************
;
; User interface to Task1.
;
        GLOBAL  SUSR_Task1
SUSR_Task1
        smTraceL STRC_TSK_BEG_1
        call UAPP_Task1
        smTraceL STRC_TSK_END_1
        return
;
;*******************************************************************************
;
; User interface to Task2.
;
        GLOBAL  SUSR_Task2
SUSR_Task2
        smTraceL STRC_TSK_BEG_2
        call UAPP_Task2
        smTraceL STRC_TSK_END_2
        return
;
;*******************************************************************************
;
; User interface to Task3.
;
        GLOBAL  SUSR_Task3
SUSR_Task3
        smTraceL STRC_TSK_BEG_3
        call UAPP_Task3
        smTraceL STRC_TSK_END_3
        return
;
;*******************************************************************************
;
; User interface to TaskADC.
;
        GLOBAL  SUSR_TaskADC
SUSR_TaskADC
        smTraceL STRC_TSK_BEG_ADC
        call    UAPP_TaskADC         ; Convert A/D result and do something with it.
        smTraceL STRC_TSK_END_ADC
        return
;
;*******************************************************************************
;
; User interface to TaskI2C.
;
        GLOBAL  SUSR_TaskI2C
SUSR_TaskI2C
        smTraceL STRC_TSK_BEG_I2C
;
    IF UCFG_BOARD==UCFG_PD2P_2002 || UCFG_BOARD==UCFG_PD2P_2010
        call    UI2C_MsgTC74ProcessData ; Process data from TC74 message.
    ENDIF
;
        smTraceL STRC_TSK_END_I2C
        return
;
;*******************************************************************************
;
; User interface to TaskSIO.
;
        GLOBAL  SUSR_TaskSIO_MsgRcvd
SUSR_TaskSIO_MsgRcvd
        smTraceL STRC_TSK_BEG_SIO_RX
        call    USIO_MsgReceived        ; Process SIO received msg.
        smTraceL STRC_TSK_END_SIO_RX
        return
;
;*******************************************************************************
;
; User handling when I2C message completed.
;
        GLOBAL  SUSR_TaskI2C_MsgDone
SUSR_TaskI2C_MsgDone
;
    IF UCFG_BOARD==UCFG_PD2P_2002 || UCFG_BOARD==UCFG_PD2P_2010
        goto    UI2C_MsgDone
    ELSE
        return
    ENDIF
;
;*******************************************************************************
;
; User handling when INT0 receiuved.
;
        GLOBAL  SUSR_ISR_INT0
SUSR_ISR_INT0
;
    IF UCFG_BOARD==UCFG_KA107I
        goto    UAPP_ISR_PWStateMachineInt0
    ELSE
        return
    ENDIF
;
;*******************************************************************************
;
; User handling when INT1 receiuved.
;
        GLOBAL  SUSR_ISR_INT1
SUSR_ISR_INT1
;
    IF UCFG_BOARD==UCFG_KA107I
        goto    UAPP_ISR_PWStateMachineInt1
    ELSE
        return
    ENDIF
;
        end
