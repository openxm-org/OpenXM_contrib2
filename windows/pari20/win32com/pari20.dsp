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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "." /I "..\pari-2.0.17.beta\src\headers" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
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
# ADD CPP /nologo /MT /W3 /Gm /GX /ZI /Od /I "." /I "..\pari-2.0.17.beta\src\headers" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FD /GZ /c
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

SOURCE="..\pari-2.0.17.beta\src\basemath\alglin1.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\alglin2.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\language\anal.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\arith1.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\arith2.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\base1.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\base2.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\base3.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\base4.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\base5.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\bibli1.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\bibli2.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\buch1.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\buch2.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\buch3.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\buch4.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\language\compat.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\modules\elliptic.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\language\errmsg.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\language\es.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\galconj.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\modules\galois.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\gen1.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\gen2.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\gen3.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\language\helpmsg.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\ifactor1.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\language\init.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\modules\kummer.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\kernel\ix86\l0asm.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\kernel\none\mp.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\modules\mpqs.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\modules\nffactor.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\polarit1.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\polarit2.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\polarit3.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\rootpol.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\modules\stark.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\modules\subfield.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\subgroup.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\language\sumiter.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\modules\thue.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\trans1.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\trans2.c"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\basemath\trans3.c"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\language\anal.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\kernel\ix86\l0asm.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\kernel\ix86\level0.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\headers\pari.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\headers\pari68k.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\headers\paricast.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\headers\paricom.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\headers\paridecl.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\headers\parierr.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\headers\parigen.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\headers\parinf.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\headers\pariport.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\headers\paristio.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\headers\parisys.h"
# End Source File
# Begin Source File

SOURCE="..\pari-2.0.17.beta\src\headers\paritype.h"
# End Source File
# End Group
# End Target
# End Project
