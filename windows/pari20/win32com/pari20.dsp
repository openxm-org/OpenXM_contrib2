# Microsoft Developer Studio Project File - Name="pari20" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=pari20 - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "pari20.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "pari20.mak" CFG="pari20 - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "pari20 - Win32 Release" ("Win32 (x86) Static Library" 用)
!MESSAGE "pari20 - Win32 Debug" ("Win32 (x86) Static Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pari20 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "." /I "..\..\..\..\OpenXM_contrib\pari\src\headers" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\libpari.lib"

!ELSEIF  "$(CFG)" == "pari20 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /I "." /I "..\..\..\..\OpenXM_contrib\pari\src\headers" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\libpari.lib"

!ENDIF 

# Begin Target

# Name "pari20 - Win32 Release"
# Name "pari20 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\alglin1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\alglin2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\language\anal.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\arith1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\arith2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\base1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\base2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\base3.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\base4.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\base5.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\bibli1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\bibli2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\buch1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\buch2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\buch3.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\buch4.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\language\compat.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\modules\elliptic.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\language\errmsg.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\language\es.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\galconj.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\modules\galois.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\gen1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\gen2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\gen3.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\language\helpmsg.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\ifactor1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\language\init.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\modules\kummer.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\kernel\ix86\l0asm.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\kernel\none\mp.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\modules\mpqs.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\modules\nffactor.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\polarit1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\polarit2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\polarit3.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\rootpol.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\modules\stark.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\modules\subfield.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\subgroup.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\language\sumiter.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\modules\thue.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\trans1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\trans2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\basemath\trans3.c"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\language\anal.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\kernel\ix86\l0asm.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\kernel\ix86\level0.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\headers\pari.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\headers\pari68k.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\headers\paricast.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\headers\paricom.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\headers\paridecl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\headers\parierr.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\headers\parigen.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\headers\parinf.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\headers\pariport.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\headers\paristio.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\headers\parisys.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari\src\headers\paritype.h"
# End Source File
# End Group
# End Target
# End Project
