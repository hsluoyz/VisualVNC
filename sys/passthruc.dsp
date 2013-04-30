# Microsoft Developer Studio Project File - Name="passthruc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=passthruc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "passthruc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "passthruc.mak" CFG="passthruc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "passthruc - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "passthruc - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "passthruc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f passthruc.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "passthruc.exe"
# PROP BASE Bsc_Name "passthruc.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "build.bat %WDKROOT% free D: D:\workspaces\vc\sys"
# PROP Rebuild_Opt "/a"
# PROP Target_File "passthruc.exe"
# PROP Bsc_Name "passthruc.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "passthruc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Cmd_Line "NMAKE /f passthruc.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "passthruc.exe"
# PROP BASE Bsc_Name "passthruc.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Cmd_Line "build.bat %WDKPATH% checked"
# PROP Rebuild_Opt "/a"
# PROP Target_File "passthruc.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "passthruc - Win32 Release"
# Name "passthruc - Win32 Debug"

!IF  "$(CFG)" == "passthruc - Win32 Release"

!ELSEIF  "$(CFG)" == "passthruc - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\analysispacket.c
# End Source File
# Begin Source File

SOURCE=.\Hook.c
# End Source File
# Begin Source File

SOURCE=.\miniport.c
# End Source File
# Begin Source File

SOURCE=.\passthru.c
# End Source File
# Begin Source File

SOURCE=.\passthru.rc
# End Source File
# Begin Source File

SOURCE=.\protocol.c
# End Source File
# Begin Source File

SOURCE=..\TestNdis\TestNdisDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\CommonIO.h
# End Source File
# Begin Source File

SOURCE=.\passthru.h
# End Source File
# Begin Source File

SOURCE=.\precomp.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Build.bat
# End Source File
# Begin Source File

SOURCE=.\makefile
# End Source File
# Begin Source File

SOURCE=.\sources
# End Source File
# End Group
# End Target
# End Project
