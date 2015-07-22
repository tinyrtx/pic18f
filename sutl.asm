        LIST
;*******************************************************************************
; tinyRTX Filename: sutl.asm (System UTiLities)
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
;   16Oct03 SHiggins@tinyRTX.com    Created from scratch.
;   30Jul14 SHiggins@tinyRTX.com    Reduce from 4 tasks to 3 to reduce stack needs.
;   12Aug14 SHiggins@tinyRTX.com    Converted from PIC16877 to PIC18F452.
;   15Aug14 SHiggins@tinyRTX.com    Added SRTX_ComputedBraRCall and SRTX_ComputedGotoCall.
;   25Aug14 SHiggins@tinyRTX.com    Created when majority of srtx.asm converted to C.
;   21May15 Stephen_Higgins@KairosAutonomi.com
;               Substitute #include <ucfg.inc> for <p18f452.inc>.
;   17Jul15 Stephen_Higgins@KairosAutonomi.com
;               Add SUTL_InvokeBootloader and SUTL_DisableBootloader.
;
;*******************************************************************************
;
        #include <ucfg.inc>          ; includes processor definitions.
;
;*******************************************************************************
;
; SUTL service variables.
;
SUTL_UdataSec UDATA
;
; Temp variable for SUTL_ComputedBraRCall and SUTL_ComputedGotoCall, because rotates
;  can only be done through a memory location.
;
SUTL_TempRotate         res     1
;
;*******************************************************************************
;
; SUTL Services.
;
SUTL_CodeSec  CODE  
;
;*******************************************************************************
;
; SUTL Computed Branch / Relative Call:
;   Use the value in W as an offset to the program counter saved in the TOS
;   (Top Of Stack) registers.  This allows a computed branch or relative call
;   into a table of program addresses, useful for implementing state tables.
;
;   Either BRA or RCALL instructions will work because those instructions use
;   2 bytes each.  There is a separate routine for using GOTO or CALL instructions.
;
;       return                      ; Jump to computed addr.
;
; To use this feature in a calling program, set it up as follows:
;       ...
;       banksel State_Variable
;       movf    State_Variable, W       ; Get state variable into W.
;       call    SUTL_ComputedBraRcall   ; W = offset, index into state machine jump table.
;
; (Note that this table must IMMEDIATELY follow the code above.)
;
;       bra     State0_Routine      ; Program label for State 0 routine (W = 0)
;       bra     State1_Routine      ; Program label for State 1 routine (W = 1)
;       bra     State2_Routine      ; Program label for State 2 routine (W = 2)
;       ...
;
; (Alteratively, the RCALL instruction could be used instead of BRA.)
;
;       rcall   State0_Routine      ; Program label for State 0 routine (W = 0)
;       rcall   State1_Routine      ; Program label for State 1 routine (W = 1)
;       rcall   State2_Routine      ; Program label for State 2 routine (W = 2)
;       ...
;
;*******************************************************************************
;
        GLOBAL  SUTL_ComputedBraRCall
SUTL_ComputedBraRCall
;
        banksel SUTL_TempRotate
        movwf   SUTL_TempRotate     ; Move input arg W to where we can rotate it.
        rlncf   SUTL_TempRotate     ; Index * 2 = offset in words (2 bytes).
        movf    SUTL_TempRotate,W   ; Move Index * 2 back to W.
        addwf   TOSL, F             ; Add jump offset in W to return address.
        btfsc   STATUS, C           ; If there was an overflow..
        incf    TOSH, F             ; ..then adjust the high byte ret addr too.
;
;   Notice we don't bother with TOSU because program memory is under 64K bytes
;   But if we were to bother, it would look like...
;
;       btfsc   STATUS, C           ; If there was an overflow..
;       incf    TOSU, F             ; ..then adjust the upper byte ret addr too.
;
        return                      ; Jump to computed addr.
;
;*******************************************************************************
;
; SUTL Computed Goto / Call:
;   Use the value in W as an offset to the program counter saved in the TOS
;   (Top Of Stack) registers.  This allows a computed goto or call
;   into a table of program addresses, useful for implementing state tables.
;
;   Either GOTO or CALL instructions will work because those instructions use
;   4 bytes each.  There is a separate routine for using BRA or RCALL instructions.
;
;       return                      ; Jump to computed addr.
;
; To use this feature in a calling program, set it up as follows:
;       ...
;       banksel State_Variable
;       movf    State_Variable, W       ; Get state variable into W.
;       call    SUTL_ComputedGotoCall   ; W = offset, index into state machine jump table.
;
; (Note that this table must IMMEDIATELY follow the code above.)
;
;       goto    State0_Routine      ; Program label for State 0 routine (W = 0)
;       goto    State1_Routine      ; Program label for State 1 routine (W = 1)
;       goto    State2_Routine      ; Program label for State 2 routine (W = 2)
;       ...
;
; (Alteratively, the CALL instruction could be used instead of GOTO.)
;
;       call    State0_Routine      ; Program label for State 0 routine (W = 0)
;       call    State1_Routine      ; Program label for State 1 routine (W = 1)
;       call    State2_Routine      ; Program label for State 2 routine (W = 2)
;       ...
;
;*******************************************************************************
;
        GLOBAL  SUTL_ComputedGotoCall
SUTL_ComputedGotoCall
        banksel SUTL_TempRotate
        movwf   SUTL_TempRotate     ; Move input arg W to where we can rotate it.
        rlncf   SUTL_TempRotate     ; Index * 4 = offset in double words (4 bytes).
        rlncf   SUTL_TempRotate
        movf    SUTL_TempRotate,W   ; Move Index * 2 back to W.
        addwf   TOSL, F             ; Add jump offset in W to return address.
        btfsc   STATUS, C           ; If there was an overflow..
        incf    TOSH, F             ; ..then adjust the high byte ret addr too.
;
;   Notice we don't bother with TOSU because program memory is under 64K bytes
;   But if we were to bother, it would look like...
;
;       btfsc   STATUS, C           ; If there was an overflow..
;       incf    TOSU, F             ; ..then adjust the upper byte ret addr too.
;
        return                      ; Jump to computed addr.
;
;*******************************************************************************
;
; SUTL Invoke Bootloader:
;   Put some magic values in RAM so when bootloader starts, it knows to stay in
;   bootloader mode and not transfer control to the application.
;
;   NOTE: Stomps the memory at the high end of the lower ACCESS RAM (0x78-0x7F)
;   because we are going to reset from here so no one cares about current state.
;   By the time execution reaches whichever routine uses those variables, they
;   will have been initialized again through reset logic.
;
;   Fixed byte sequence at 0x78-0x7F = 0x0F A5 69 C3 E1 D2 87 4B
;
;*******************************************************************************
;
        GLOBAL  SUTL_InvokeBootloader
SUTL_InvokeBootloader
        movlw   0x0F    ; This is a fixed byte sequence.
        movwf   0x78
        movlw   0xA5
        movwf   0x79
        movlw   0x69
        movwf   0x7A
        movlw   0xC3
        movwf   0x7B
        movlw   0xE1
        movwf   0x7C
        movlw   0xD2
        movwf   0x7D
        movlw   0x87
        movwf   0x7E
        movlw   0x4B
        movwf   0x7F
;
;   Reset the processor, let bootloader find byte sequence.
;
        reset
;
;*******************************************************************************
;
; SUTL Disable Bootloader:
;   Clean out the magic values in RAM so that if processor resets and bootloader
;   starts, it doesn't think we are trying to invoke bootloader simply because
;   we have those values leftover in there from last SUTL_InvokeBootloader call.
;
;   Of course, this only disables the inadvertant invocation of bootloader set
;   up by SUTL_InvokeBootloader.  The bootloader will also stay invoked if:
;       a) RX line is pulled low coming out of reset
;       b) Bootloader can't find any application to which to transfer control
;
;   NOTE: Stomps the memory at the high end of the lower ACCESS RAM (0x78-0x7F).
;   Therefore this should be called VERY early in reset processing.
;
;*******************************************************************************
;
        GLOBAL  SUTL_DisableBootloader
SUTL_DisableBootloader
        clrf    0x78
        clrf    0x79
        clrf    0x7A
        clrf    0x7B
        clrf    0x7C
        clrf    0x7D
        clrf    0x7E
        clrf    0x7F
        return
        end