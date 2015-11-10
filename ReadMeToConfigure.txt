//
//  ReadMeToConfigure.txt - tinyRTX configuration for multiple PIC workspaces.
//
// Revision history:
//  27May15 Stephen_Higgins@KairosAutonomi.com  Created from scratch.
//  27May15 Stephen_Higgins@KairosAutonomi.com
//              Move board selection from ucfg.inc/h to project MPASM macro.
//  09Jul15 Stephen_Higgins@KairosAutonomi.com
//              Split UCFG_DJPCB_280B into UCFG_KA280BI and UCFG_KA280BT.
//  30Jul15 Stephen_Higgins@KairosAutonomi.com
//              Combine UCFG_KA280BI and UCFG_KA280BT into UCFG_DJPCB_280B.
//  27Aug15 Stephen_Higgins@KairosAutonomi.com
//              Split UCFG_DJPCB_280B into UCFG_KA280BI and UCFG_KA280BT. (again)
//  04Sep15 Stephen_Higgins@KairosAutonomi.com
//              Add UCFG_KA107I and some more explanatory text.
//  09Sep15 Stephen_Higgins@KairosAutonomi.com
//              Added toolsuites.  Added project directories settings.
//
//  Overview: There are multiple projects coexisting in the pic18 directory.
//      Each project has it's own specific project files (whatever.mcp/mcs/mcw)
//      and application (uapp_whatever.c).  All projects share a bunch of
//      common code like most of Sxxx.x files and many of the Uxxx.x files.
//      When adding a new project its easiest to take an existing workspace
//      (which includes the project and some useful windows) and clone it by
//      doing a File / Save Workspace As... and giving it a new name.  This
//      automatically creates a new project with the new name too.  Then
//      update the new project with the following steps.

1) Most files will remain common and untouched.
    a) All .asm and .inc files that are completely used regardless of
        whether C is being used, will always be used.
    b) The .asm and .inc files which are also called from C (when C is
        being used) will have .h files corresponding to the .inc files,
        but not necessarily all the same headers.
    c) The .asm and .inc files which are completely replaced when C is
        being used will have matching .c and .h files.
    d) All these file coexist in the single "tinyRTX pic18" directory,
        and the project file "tinyRTX whatever.mcp" selects the appropriate
        files.  Unselected files are simply not used.

2) Replace the main application source file.
    a) In the Project Explorer window remove the old application by
        right-clicking on it and using Remove.  This leaves the file in
        the directory for the old project to use.
    b) In the project Explorer window add the new application by
        right-clicking on "Source Files" and using Add Files.
        (This assumes you've already created a new application, likely
        by cloning an existing one.)

3) Ensure project has correctly set the processor in the IDE:
    a) For example, if using the pic18f452:
        Configure/Select Device.../Device should be set to PIC18F452.
        
4) Ensure project has correctly set board selection in the MPASM macros:
    a) For example, if using the Microchip PICDEM2PLUS 2002 (has ext 4MHz):
        Project/Build Options.../Project/MPASM Assembler/Macro Definitions
        should contain the string "_UCFG_PD2P02"  (don't add the quotes)
    b) This will pass macro definitions to the assembler,
        to control conditional assembly.
    c) _UCFG_PD2P02 - Microchip PICDEM2PLUS 2002 (has ext 4MHz)
    d) _UCFG_PD2P10 - Microchip PICDEM2PLUS 2010 (no ext 4MHz)
    e) _UCFG_KA280BI - Kairos Autonomi 280B: Utility Module I/O
    f) _UCFG_KA280BT - Kairos Autonomi 280B: Transmission Module I/O
    g) _UCFG_KA107I - Kairos Autonomi 107I: Quad and Video Mux
        
5) Ensure project has correctly set board selection in the MPLAB C18 macros:
    (ONLY applicable if using ASM/C)
    a) For example, if using the Microchip PICDEM2PLUS 2002 (has ext 4MHz):
        Project/Build Options.../Project/MPLAB C18/Macro Definitions
        should contain the string "_UCFG_PD2P02"  (don't add the quotes)
    b) This will pass macro definitions to the compiler,
        to control conditional compilation.
    c) _UCFG_PD2P02 - Microchip PICDEM2PLUS 2002 (has ext 4MHz)
    d) _UCFG_PD2P10 - Microchip PICDEM2PLUS 2010 (no ext 4MHz)
    e) _UCFG_KA280BI - Kairos Autonomi 280B: Utility Module I/O
    f) _UCFG_KA280BT - Kairos Autonomi 280B: Transmission Module I/O
    g) _UCFG_KA107I - Kairos Autonomi 107I: Quad and Video Mux

6) Ensure project has correctly set _UCFG_USING_C in the MPASM command:
    a) For example, if using the pic18f452-c:
        Project/Build Options.../Project/MPASM Assembler/Macro Definitions
        should contain the string "_UCFG_USING_C"  (don't add the quotes)
    b) This will pass macro definitions to the assembler,
        so _UCFG_USING_C can be tested to control conditional assembly.
    c) IMPORTANT, if project does not use C, check to ensure no string "_UCFG_USING_C"

7) Ensure project has correctly set the processor in the MPLINK command:
    a) For example, if using the pic18f452:
        Project/Build Options.../Project/MPLINK Linker/Use Alternate Settings
        should contain the string "/u__18F452"  (don't add the quotes)
    b) This will pass a linker macro definition to the linker script,
        such as __18F452, __18F2620, to control conditional linking.

8) Ensure the correct active toolsuite is set:
    Microchip C18 Toolsuite:
        MPASM Assembler v5.54:      C:\Program Files\Microchip\mplabc18\v3.47\mpasm\mpasmwin.exe
        MPLINK Object Linker v4.49: C:\Program Files\Microchip\MPASM Suite\mplink.exe
        MPLAB C18 C Compiler v3.47: C:\Program Files\Microchip\mplabc18\v3.47\bin\mcc18.exe
        MPLIB Librarian v5.00:      C:\Program Files\Microchip\mplabc18\v3.47\bin\mplib.exe

    Microchip MPASM Toolsuite:
        MPASM Assembler v5.51:      C:\Program Files\Microchip\MPASM Suite\MPASMWIN.exe
        MPLINK Object Linker v4.49: C:\Program Files\Microchip\MPASM Suite\mplink.exe
        MPLIB Librarian v4.49:      C:\Program Files\Microchip\MPASM Suite\mplib.exe

    Microchip XC8 Toolsuite:
        Microchip MPLAB XC8 Compiler: C:\Program Files\Microchip\xc8\v1.35\bin\xc8.exe

    HI-TECH PICC Toolsuite:
        PICC Compiler (picc.exe):   C:\HT-PIC\BIN\PICC.EXE
        PICC Assembler (picc.exe):  C:\HT-PIC\BIN\PICC.EXE
        PICC Linker (picc.exe):     C:\HT-PIC\BIN\PICC.EXE

//  Toolsuite notes for setting XC8, found on the internet:
//  I've just had to reinstall MPLAB8.92 and XC8 onto a new laptop (after mine died),
//  and been through the same bother. However, to work, it MUST be in the folder
//  containing the DLL file, which in my case was:
//  C:\Program Files (x86)\Microchip\xc8\v1.34\binand it MUST be run as an admin.
//  (SRH note: I did not find in necessary to run it as an ADMIN.)
//  I ended up not using the batch file, but running regsvr32 from the command line
//  as follows. Run cmd.exe as admin, by clicking START, typing in "cmd",
//  right-clicking cmd.exe when it appeared, and selecting "Run as administrator".
//  I then typed these lines:
//  cd  "C:\Program Files (x86)\Microchip\xc8\v1.34\bin"
//  regsvr32 mplabxc8.dll
//  That way you get to see the window saying it worked (or didn't). The batch file
//  suppresses the confirmation.

9) Ensure the project directories are set correctly:
    a) Project/Build Options.../Project/Directories/Output Directory = output
    b) Project/Build Options.../Project/Directories/Intermediary Directory = temp
    c) Project/Build Options.../Project/Directories/Include Search Path = ""
    d) Project/Build Options.../Project/Directories/Library Search Path = C:\Program Files\Microchip\mplabc18\v3.47\lib 
    e) Project/Build Options.../Project/Directories/Linker-Script Search Path = ""

Other stuff that will happen automatically:
A) When using a project containing C source code, the tools will define linker macros
    and pass them to the linker script, such as _CRUNTIME.
