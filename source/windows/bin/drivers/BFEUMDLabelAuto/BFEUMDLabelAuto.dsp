# Microsoft Developer Studio Project File - Name="BFEUMDLabelAuto" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=BFEUMDLabelAuto - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BFEUMDLabelAuto.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BFEUMDLabelAuto.mak" CFG="BFEUMDLabelAuto - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BFEUMDLabelAuto - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "BFEUMDLabelAuto - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""PQ_TV/source/windows/bin/drivers/BFEUMDLabelAuto" ::branch::"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BFEUMDLabelAuto - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\common" /I "$(CC_ROOT)\$(CC_SYSTEM)\source\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comctl32.lib version.lib csicln.lib asyncPost.lib bncsif32.lib csiclndb.lib extinfo.lib bncs_string.lib bncs_config.lib asyncpost.lib /nologo /subsystem:windows /map /machine:I386 /libpath:"$(CC_ROOT)\$(CC_SYSTEM)\source\lib"

!ELSEIF  "$(CFG)" == "BFEUMDLabelAuto - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\common" /I "$(CC_ROOT)\$(CC_SYSTEM)\source\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib comctl32.lib version.lib csiclndb.lib asyncPostdb.lib bncsif32.lib csiclndb.lib extinfodb.lib bncs_string.lib bncs_config.lib asyncpost.lib /nologo /subsystem:windows /map /debug /machine:I386 /pdbtype:sept /libpath:"$(CC_ROOT)\$(CC_SYSTEM)\source\lib"

!ENDIF 

# Begin Target

# Name "BFEUMDLabelAuto - Win32 Release"
# Name "BFEUMDLabelAuto - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BFEUMDLabelAuto.cpp
# End Source File
# Begin Source File

SOURCE=.\BFEUMDLabelAuto.rc
# End Source File
# Begin Source File

SOURCE=.\BFEUMDLabelDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\except.cpp
# End Source File
# Begin Source File

SOURCE=.\FiFoBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\KSCLabelData.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\RevsRouterData.cpp
# End Source File
# Begin Source File

SOURCE=.\RevsSNCSData.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BFEUMDLabelAuto.h
# End Source File
# Begin Source File

SOURCE=.\BFEUMDLabelCONSTS.h
# End Source File
# Begin Source File

SOURCE=.\BFEUMDLabelDatabase.h
# End Source File
# Begin Source File

SOURCE=.\except.h
# End Source File
# Begin Source File

SOURCE=.\FiFoBuffer.h
# End Source File
# Begin Source File

SOURCE=.\KSCLabelData.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\RevsRouterData.h
# End Source File
# Begin Source File

SOURCE=.\RevsSNCSData.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\BFEUMDLabelAuto.ico
# End Source File
# Begin Source File

SOURCE=.\Logo2.bmp
# End Source File
# Begin Source File

SOURCE=.\small.ico
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Common\bncs_auto_helper.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\bncs_auto_helper.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\BFEUMDLabelAuto.txt
# End Source File
# End Target
# End Project
