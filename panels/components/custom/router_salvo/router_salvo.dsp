# Microsoft Developer Studio Project File - Name="router_salvo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=router_salvo - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "router_salvo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "router_salvo.mak" CFG="router_salvo - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "router_salvo - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "router_salvo - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "router_salvo"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "router_salvo - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "./"
# PROP BASE Intermediate_Dir ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "./"
# PROP Intermediate_Dir ""
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MD /W3 /GX /O1 /I ".\router_salvo" /I "$(CC_ROOT)\$(CC_SYSTEM)\source\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DOEXPORT_SCRIPT" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /D "QT_NO_DEBUG" /FD -Zm200 /c
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "bncs_config.lib" "decodecaptionstring.lib" "bncs_string.lib" "bncs_script_helper.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" delayimp.lib /nologo /dll /machine:IX86 /nodefaultlib:"libcd" /out:"router_salvo.dll" /libpath:"$(CC_ROOT)\$(CC_SYSTEM)\source\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "router_salvo - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD CPP /nologo /MDd /W3 /GX /Z7 /Od /I ".\router_salvo" /I "$(CC_ROOT)\$(CC_SYSTEM)\source\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DOEXPORT_SCRIPT" /D "QT_DLL" /D "QT_THREAD_SUPPORT" /FD /GZ -Zm200 /c
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /machine:IX86
# ADD LINK32 "bncs_config.lib" "decodecaptionstring.lib" "bncs_string.lib" "bncs_script_helper.lib" "kernel32.lib" "user32.lib" "gdi32.lib" "comdlg32.lib" "advapi32.lib" "shell32.lib" "ole32.lib" "oleaut32.lib" "uuid.lib" "imm32.lib" "winmm.lib" "wsock32.lib" "winspool.lib" /nologo /dll /debug /machine:IX86 /nodefaultlib:"libcd" /nodefaultlib:"MSVCRT" /out:"router_salvo.dll" /implib:".\router_salvo.lib" /pdbtype:sept /libpath:"$(CC_ROOT)\$(CC_SYSTEM)\source\lib"

!ENDIF 

# Begin Target

# Name "router_salvo - Win32 Release"
# Name "router_salvo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=router_salvo.cpp
# End Source File
# Begin Source File

SOURCE=.\router_salvo.rc
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\source\include\bncs_script_helper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\source\include\bncs_string.h
# End Source File
# Begin Source File

SOURCE=..\..\..\source\include\bncs_stringlist.h
# End Source File
# Begin Source File

SOURCE=router_salvo.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\p1.bncs_ui
# End Source File
# End Target
# End Project
