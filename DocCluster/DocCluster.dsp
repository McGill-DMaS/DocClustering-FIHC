# Microsoft Developer Studio Project File - Name="DocCluster" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=DocCluster - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DocCluster.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DocCluster.mak" CFG="DocCluster - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DocCluster - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "DocCluster - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DocCluster - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "include\\" /I "..\BFLib\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "include\\" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib MsXml2.lib Shlwapi.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "DocCluster - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "include\\" /I "..\BFLib\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "include\\" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib MsXml2.lib Shlwapi.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DocCluster - Win32 Release"
# Name "DocCluster - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\DCCluster.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCClusterMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCClusterMgr2.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCClusterWH.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCClutoMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCController.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCDebug.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCDocCluster.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCDocMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCDocument.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCDocVector.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCEvalClass.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCEvalMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCFreqItem.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCFreqItemMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCKMeansMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCKMVector.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCOutputMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCPreprocessMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCStemHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCStopwordHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCTreeBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCUnrefinedDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCWordNode.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DCWordsBTree.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DocCluster.rc
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\DCCluster.h
# End Source File
# Begin Source File

SOURCE=.\include\DCClusterMgr.h
# End Source File
# Begin Source File

SOURCE=.\include\DCClusterMgr2.h
# End Source File
# Begin Source File

SOURCE=.\include\DCClusterWH.h
# End Source File
# Begin Source File

SOURCE=.\include\DCClutoMgr.h
# End Source File
# Begin Source File

SOURCE=.\include\DCController.h
# End Source File
# Begin Source File

SOURCE=.\include\DCDebug.h
# End Source File
# Begin Source File

SOURCE=.\include\DCDef.hpp
# End Source File
# Begin Source File

SOURCE=.\include\DCDocCluster.h
# End Source File
# Begin Source File

SOURCE=.\include\DCDocMgr.h
# End Source File
# Begin Source File

SOURCE=.\include\DCDocument.h
# End Source File
# Begin Source File

SOURCE=.\include\DCDocVector.h
# End Source File
# Begin Source File

SOURCE=.\include\DCEvalClass.h
# End Source File
# Begin Source File

SOURCE=.\include\DCEvalMgr.h
# End Source File
# Begin Source File

SOURCE=.\include\DCFreqItem.h
# End Source File
# Begin Source File

SOURCE=.\include\DCFreqItemMgr.h
# End Source File
# Begin Source File

SOURCE=.\include\DCKMeansMgr.h
# End Source File
# Begin Source File

SOURCE=.\include\DCKMVector.h
# End Source File
# Begin Source File

SOURCE=.\include\DCOutputMgr.h
# End Source File
# Begin Source File

SOURCE=.\include\DCPreprocessMgr.h
# End Source File
# Begin Source File

SOURCE=.\include\DCStemHandler.h
# End Source File
# Begin Source File

SOURCE=.\include\DCStopwordHandler.h
# End Source File
# Begin Source File

SOURCE=.\include\DCTreeBuilder.h
# End Source File
# Begin Source File

SOURCE=.\include\DCUnrefinedDoc.h
# End Source File
# Begin Source File

SOURCE=.\include\DCWordNode.h
# End Source File
# Begin Source File

SOURCE=.\include\DCWordsBTree.h
# End Source File
# Begin Source File

SOURCE=.\include\Resource.h
# End Source File
# Begin Source File

SOURCE=.\include\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
