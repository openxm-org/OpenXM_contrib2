# Microsoft Developer Studio Project File - Name="pari224" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=pari224 - Win32 Debug
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "pari224.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "pari224.mak" CFG="pari224 - Win32 Debug"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "pari224 - Win32 Release" ("Win32 (x86) Static Library" 用)
!MESSAGE "pari224 - Win32 Debug" ("Win32 (x86) Static Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pari224 - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "." /I "..\..\..\..\OpenXM_contrib\pari-2.2\src\headers" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\libpariw.lib"

!ELSEIF  "$(CFG)" == "pari224 - Win32 Debug"

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
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /I "." /I "..\..\..\..\OpenXM_contrib\pari-2.2\src\headers" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\libpariw.lib"

!ENDIF 

# Begin Target

# Name "pari224 - Win32 Release"
# Name "pari224 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\alglin1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\alglin2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\language\anal.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\modules\aprcl.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\arith1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\arith2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\base1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\base2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\base3.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\base4.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\base5.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\bibli1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\bibli2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\buch1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\buch2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\buch3.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\buch4.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\language\compat.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\modules\elliptic.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\language\errmsg.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\language\es.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\galconj.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\modules\galois.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\gen1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\gen2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\gen3.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\language\helpmsg.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\ifactor1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\language\init.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\modules\kummer.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\kernel\ix86\l0asm.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\kernel\none\mp.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\modules\mpqs.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\modules\nffactor.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\perm.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\polarit1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\polarit2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\polarit3.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\rootpol.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\modules\stark.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\subcyclo.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\modules\subfield.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\subgroup.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\language\sumiter.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\modules\thue.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\trans1.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\trans2.c"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\basemath\trans3.c"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\language\anal.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\kernel\ix86\l0asm.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\kernel\ix86\level0.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\headers\pari.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\headers\pari68k.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\headers\paricast.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\headers\paricom.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\headers\paridecl.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\headers\parierr.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\headers\parigen.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\headers\parinf.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\headers\pariport.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\headers\paristio.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\headers\parisys.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\..\OpenXM_contrib\pari-2.2\src\headers\paritype.h"
# End Source File
# End Group
# End Target
# End Project
