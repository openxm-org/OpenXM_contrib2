# Microsoft Developer Studio Project File - Name="asir2000lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=asir2000lib - Win32 Release
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "asir2000lib.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "asir2000lib.mak" CFG="asir2000lib - Win32 Release"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "asir2000lib - Win32 Release" ("Win32 (x86) Static Library" 用)
!MESSAGE "asir2000lib - Win32 Debug" ("Win32 (x86) Static Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "asir2000lib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /MT /W3 /GX /Ox /Ot /Og /Oi /Op /I "..\..\asir2000\include" /I "..\..\asir2000\parse" /I "..\pari2\include" /I "..\..\asir2000\plot" /D "DES_ENC" /D "VISUAL_LIB" /D "VISUAL" /D "WIN32" /D "__WIN32__" /D "CONSOLE" /D "ALL_INTERIOR_POINTERS" /D "SILENT" /D PARI=1 /D INET=1 /D KAN=0 /D LAPACK=0 /D DO_PLOT=0 /D READLINE=0 /D REMOTE=0 /D USE_FLOAT=0 /D "LONG_IS_32BIT" /D "HMEXT" /D "ULONG_NOT_DEFINED" /D "_MBCS" /D "LARGE_CONFIG" /D DO_PLOT=1 /FD /D ASIR_LIBDIR=\"c:/asir/stdlib\" /c
# ADD BASE RSC /l 0x411
# ADD RSC /l 0x411
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "asir2000lib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "..\..\asir2000\include" /I "..\..\asir2000\parse" /I "..\pari2\include" /I "..\..\asir2000\plot" /D "..\..\asir2000\parse" /D "DES_ENC" /D "VISUAL_LIB" /D "VISUAL" /D "WIN32" /D "__WIN32__" /D "CONSOLE" /D "ALL_INTERIOR_POINTERS" /D "SILENT" /D PARI=1 /D INET=1 /D KAN=0 /D LAPACK=0 /D DO_PLOT=0 /D READLINE=0 /D REMOTE=0 /D USE_FLOAT=0 /D "LONG_IS_32BIT" /D "HMEXT" /D "ULONG_NOT_DEFINED" /D "_MBCS" /D "LARGE_CONFIG" /D DO_PLOT=1 /FD /I ASIR_LIBDIR=\"c:/asir/stdlib\" /c
# ADD BASE RSC /l 0x411
# ADD RSC /l 0x411
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "asir2000lib - Win32 Release"
# Name "asir2000lib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\..\asir2000\engine\_distm.c
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\A.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\al.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\alg.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\algnum.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\allchblk.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\alloc.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\arith.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\array.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\asir_lib.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\asm\asmiw.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\batch.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\bf.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\io\biovar.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\blacklst.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\io\bload.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\io\bsave.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\C.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\checksums.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\io\cio.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\comp.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\compobj.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\io\cpexpr.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\cplx.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\cplxnum.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\cpp.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\ctrl.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\D.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\dbg_mlc.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\asm\ddM.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\asm\ddN.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\debug.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\io\des.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\fft\dft.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\dist.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\distm.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\dp-supp.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\dp.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\dyn_load.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\E.c"

!IF  "$(CFG)" == "asir2000lib - Win32 Release"

# ADD CPP /O2

!ELSEIF  "$(CFG)" == "asir2000lib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\Ebug.c"

!IF  "$(CFG)" == "asir2000lib - Win32 Release"

# SUBTRACT CPP /Ox /Og /Oi

!ELSEIF  "$(CFG)" == "asir2000lib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\ec.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\eval.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\evall.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\EZ.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\F.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\fctr.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\fft\fft_primes.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\file.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\finalize.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\ftab.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\function.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\gc_risa.c"
# ADD CPP /D "WINNT"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\gf.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\asm\gf2m_mul_w.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\gf2n.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\gfpn.c"
# End Source File
# Begin Source File

SOURCE=..\..\asir2000\engine\gfs.c
# End Source File
# Begin Source File

SOURCE=..\..\asir2000\engine\gfspn.c
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\glob.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\gr.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\H.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\headers.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\help.c"
# End Source File
# Begin Source File

SOURCE=..\..\asir2000\engine\Hgfs.c
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\init.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\int.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\io\io.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\kwd.c"

!IF  "$(CFG)" == "asir2000lib - Win32 Release"

# SUBTRACT CPP /X /D "LARGE_CONFIG" /D KAN=0 /D READLINE=0 /D REMOTE=0

!ELSEIF  "$(CFG)" == "asir2000lib - Win32 Debug"

# SUBTRACT CPP /D "LARGE_CONFIG" /D KAN=0 /D READLINE=0 /D REMOTE=0

!ENDIF 

# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\lex.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\list.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\lmi.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\load.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\M.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\mach_dep.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\main.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\malloc.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\mallocx.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\mark.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\mark_rts.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\mat.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\math.c"
# End Source File
# Begin Source File

SOURCE=..\..\asir2000\engine\Mgfs.c
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\mi.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\misc.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\miscf.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\mt19937.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\N.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\new_hblk.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\NEZ.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\num.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\numerical.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\obj_map.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\os_dep.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\io\ox.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\P.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\pari.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\parif.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\parser.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\PD.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\pdiv.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\PDM.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\io\pexpr.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\pf.c"
# End Source File
# Begin Source File

SOURCE=..\..\asir2000\plot\plotf.c
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\PM.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\fft\polmul.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\poly.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\print.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\ptr_chck.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\PU.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\PUM.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\puref.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\pvar.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\Q.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\QM.c"
# End Source File
# Begin Source File

SOURCE=..\..\asir2000\parse\quote.c
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\R.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\rat.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\real.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\real_malloc.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\reclaim.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\reduct.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\result.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\RU.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\sha1.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\io\sio.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\io\spexpr.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\stdio.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\str.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\strobj.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\struct.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\stubborn.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\subst.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\io\tcpf.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\time.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\typd_mlc.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\type.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\Ui.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\up.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\up2.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\up_gf2n.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\up_lm.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\util.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\builtin\var.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\engine\vect.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\gc5.3\win32_threads.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\io\ws_fileio.c"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\xdeb.c"
# End Source File
# Begin Source File

SOURCE=..\..\asir2000\parse\ytab.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE="..\..\asir2000\parse\comp.h"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\cpp.h"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\fft\dft.h"
# End Source File
# Begin Source File

SOURCE="..\..\asir2000\parse\y.tab.h"
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
