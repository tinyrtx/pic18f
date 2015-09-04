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

Other stuff that will happen automatically:
A) When using a project containing C source code, the tools will define linker macros
    and pass them to the linker script, such as _CRUNTIME.
