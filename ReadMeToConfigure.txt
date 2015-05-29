//
//  ReadMeToConfigure.txt - tinyRTX configuration for multiple PIC workspaces.
//
// Revision history:
//  27May15 Stephen_Higgins@KairosAutonomi.com  Created from scratch.
//  27May15 Stephen_Higgins@KairosAutonomi.com
//              Move board selection from ucfg.inc/h to project MPASM macro.
//

1) Most files will remain common and untouched.
    a) All .asm and .inc files that are completely used regardless of 
        whether C is being used, will always be used.
    b) The .asm and .inc files which are also called from C (when C is
        being used) will have .h files corresponding to the .inc files,
        but not necessarily all the same headers.
    c) The .asm and .inc files which are completely replaced when C is
        being used will have matching .c and .h files.
    d) All these file coexist in the single ""tinyRTX pic18" directory,
        and the project file "tinyRTX whatever.mcp" selects the appropriate
        files.  Unselected files are simply not used.

2) Ensure project has correctly set the processor in the IDE:
    a) For example, if using the pic18f452:
        Configure/Select Device.../Device should be set to PIC18F452.
        
3) Ensure project has correctly set board selection in the MPASM macros:
    a) For example, if using the Microchip PICDEM2PLUS 2002 (has ext 4MHz):
        Project/Build Options.../Project/MPASM Assembler/Macro Definitions
        should contain the string "__PD2P02"  (don't add the quotes)
    b) This will pass macro definitions to the assembler,
        to control conditional assembly.
    c) _UCFG_PD2P02 - Microchip PICDEM2PLUS 2002 (has ext 4MHz)
    d) _UCFG_PD2P10 - Microchip PICDEM2PLUS 2010 (no ext 4MHz)
    e) _UCFG_KA280B - Kairos Autonomi 280B: Utility Module I/O
        
4) Ensure project has correctly set board selection in the MPLAB C18 macros:
    (ONLY applicable if using ASM/C)
    a) For example, if using the Microchip PICDEM2PLUS 2002 (has ext 4MHz):
        Project/Build Options.../Project/MPLAB C18/Macro Definitions
        should contain the string "__PD2P02"  (don't add the quotes)
    b) This will pass macro definitions to the assembler,
        to control conditional assembly.
    c) _UCFG_PD2P02 - Microchip PICDEM2PLUS 2002 (has ext 4MHz)
    d) _UCFG_PD2P10 - Microchip PICDEM2PLUS 2010 (no ext 4MHz)
    e) _UCFG_KA280B - Kairos Autonomi 280B: Utility Module I/O
        
5) Ensure project has correctly set _UCFG_USING_C in the MPASM command:
    a) For example, if using the pic18f452-c:
        Project/Build Options.../Project/MPASM Assembler/Macro Definitions
        should contain the string "_UCFG_USING_C"  (don't add the quotes)
    b) This will pass macro definitions to the assembler,
        so _UCFG_USING_C can be tested to control conditional assembly.
    c) IMPORTANT, if project does not use C, check to ensure no string "_UCFG_USING_C"

6) Ensure project has correctly set the processor in the MPLINK command:
    a) For example, if using the pic18f452:
        Project/Build Options.../Project/MPLINK Linker/Use Alternate Settings
        should contain the string "/u__18F452"  (don't add the quotes)
    b) This will pass a linker macro definition to the linker script,
        such as __18F452, __18F2620, to control conditional linking.

Other stuff that will happen automatically:
A) When using a project containing C source code, the tools will define linker macros
    and pass them to the linker script, such as _CRUNTIME.
