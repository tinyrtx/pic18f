        LIST
;*******************************************************************************
; tinyRTX Filename: sbtl_devices.inc (System BooTLoader devices)
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
; Revision history:
;   22Jul15 Stephen_Higgins@KairosAutonomi.com
;               Created from Microchip devices.inc.  Should follow Microchip
;               licensing but they didn't have a header in that file.  Will
;               defer to whatever typical licensing they show in their headers.
;
;*******************************************************************************
        NOLIST

#ifdef __18F452
     #define DEVICEID                .33
     #define WRITE_FLASH_BLOCKSIZE   .8
     #define ERASE_FLASH_BLOCKSIZE   .64
     #define END_FLASH               0x8000
     #define END_GPR                 0x600
#endif

#ifdef __18F2620
     #define DEVICEID                .100
     #define WRITE_FLASH_BLOCKSIZE   .64
     #define ERASE_FLASH_BLOCKSIZE   .64
     #define END_FLASH               0x10000
     #define END_GPR                 0xF80
#endif
