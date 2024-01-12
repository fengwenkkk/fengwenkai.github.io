# Microsoft Developer Studio Project File - Name="dtp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=dtp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "dtp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dtp.mak" CFG="dtp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dtp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dtp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "dtp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GX /O2 /I ".." /I "../common" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "dtp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../lib"
# PROP Intermediate_Dir "debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /GX /ZI /Od /I "../plat/public" /I "./" /I "./common" /I "tcp_server" /I "tcp_client" /I "udp_server" /I "udp_client" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "dtp - Win32 Release"
# Name "dtp - Win32 Debug"
# Begin Source File

SOURCE=.\common\dtp_func.cpp
# End Source File
# Begin Source File

SOURCE=.\tcp_client\dtp_tcp_client.cpp
# End Source File
# Begin Source File

SOURCE=.\tcp_client\dtp_tcp_client_msg_handle.cpp
# End Source File
# Begin Source File

SOURCE=.\tcp_client\dtp_tcp_client_send_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\tcp_server\dtp_tcp_server.cpp
# End Source File
# Begin Source File

SOURCE=.\tcp_server\dtp_tcp_server_msg_handle.cpp
# End Source File
# Begin Source File

SOURCE=.\tcp_server\dtp_tcp_server_send_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\udp_client\dtp_udp_client.cpp
# End Source File
# Begin Source File

SOURCE=.\udp_client\dtp_udp_client_msg_handle.cpp
# End Source File
# Begin Source File

SOURCE=.\udp_client\dtp_udp_client_send_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\udp_server\dtp_udp_server.cpp
# End Source File
# Begin Source File

SOURCE=.\udp_server\dtp_udp_server_msg_handle.cpp
# End Source File
# Begin Source File

SOURCE=.\udp_server\dtp_udp_server_send_thread.cpp
# End Source File
# Begin Source File

SOURCE=.\common\DTPClient.cpp
# End Source File
# Begin Source File

SOURCE=.\common\DTPServer.cpp
# End Source File
# End Target
# End Project
