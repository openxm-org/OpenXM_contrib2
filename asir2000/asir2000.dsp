# Microsoft Developer Studio Project File - Name="asir2000" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=asir2000 - Win32 Release
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "asir2000.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "asir2000.mak" CFG="asir2000 - Win32 Release"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "asir2000 - Win32 Release" ("Win32 (x86) Console Application" 用)
!MESSAGE "asir2000 - Win32 Debug" ("Win32 (x86) Console Application" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "asir2000 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MT /W3 /GX /Z7 /Ox /Ot /Og /Oi /Op /I ".\include" /I ".\parse" /I "..\windows\pari2\include" /D "NDEBUG" /D "VISUAL" /D "WIN32" /D "__WIN32__" /D "CONSOLE" /D "SILENT" /D "ALL_INTERIOR_POINTERS" /D "LARGE_CONFIG" /D PARI=1 /D INET=1 /D LAPACK=0 /D DO_PLOT=0 /D USE_FLOAT=0 /D "LONG_IS_32BIT" /D "HMEXT" /D "ULONG_NOT_DEFINED" /D "DES_ENC" /FR /YX /FD /D ASIR_LIBDIR=\"/asir/stdlib\" /c
# SUBTRACT CPP /Oa /Ow
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ..\windows\pari2\win32com\libpari.lib ws2_32.lib advapi32.lib /nologo /stack:0x1000000 /subsystem:console /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"msvcrt.lib" /out:".\Release/asir.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "asir2000 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /YX /c
# ADD CPP /nologo /MTd /W3 /GX /Z7 /Od /I "..\windows\pari2\include" /I ".\include" /I ".\parse" /D "_DEBUG" /D "VISUAL" /D "WIN32" /D "__WIN32__" /D "CONSOLE" /D "SILENT" /D "ALL_INTERIOR_POINTERS" /D "LARGE_CONFIG" /D PARI=1 /D INET=1 /D LAPACK=0 /D DO_PLOT=0 /D USE_FLOAT=0 /D "LONG_IS_32BIT" /D "HMEXT" /D "ULONG_NOT_DEFINED" /D "DES_ENC" /FR /YX /FD /D ASIR_LIBDIR=\"/asir/stdlib\" /c
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386
# ADD LINK32 ..\windows\pari2\win32com\libpari.lib ws2_32.lib advapi32.lib /nologo /stack:0x1000000 /subsystem:console /incremental:no /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /nodefaultlib:"msvcrtd.lib" /out:".\Debug/asir.exe"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "asir2000 - Win32 Release"
# Name "asir2000 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\engine\_distm.c
# End Source File
# Begin Source File

SOURCE=.\engine\A.c
# End Source File
# Begin Source File

SOURCE=.\builtin\al.c
# End Source File
# Begin Source File

SOURCE=.\engine\alg.c
# End Source File
# Begin Source File

SOURCE=.\builtin\algnum.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\allchblk.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\alloc.c
# End Source File
# Begin Source File

SOURCE=.\parse\arith.c
# End Source File
# Begin Source File

SOURCE=.\builtin\array.c
# End Source File
# Begin Source File

SOURCE=.\parse\asir2000_dummy.c
# End Source File
# Begin Source File

SOURCE=.\asm\asmiw.c
# End Source File
# Begin Source File

SOURCE=.\builtin\batch.c
# End Source File
# Begin Source File

SOURCE=.\engine\bf.c
# End Source File
# Begin Source File

SOURCE=.\io\biovar.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\blacklst.c
# End Source File
# Begin Source File

SOURCE=.\io\bload.c
# End Source File
# Begin Source File

SOURCE=.\io\bsave.c
# End Source File
# Begin Source File

SOURCE=.\engine\C.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\checksums.c
# End Source File
# Begin Source File

SOURCE=.\io\cio.c
# End Source File
# Begin Source File

SOURCE=.\parse\comp.c
# End Source File
# Begin Source File

SOURCE=.\builtin\compobj.c
# End Source File
# Begin Source File

SOURCE=.\io\cpexpr.c
# End Source File
# Begin Source File

SOURCE=.\engine\cplx.c
# End Source File
# Begin Source File

SOURCE=.\builtin\cplxnum.c
# End Source File
# Begin Source File

SOURCE=.\parse\cpp.c
# End Source File
# Begin Source File

SOURCE=.\builtin\ctrl.c
# End Source File
# Begin Source File

SOURCE=.\engine\D.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\dbg_mlc.c
# End Source File
# Begin Source File

SOURCE=.\asm\ddM.c
# End Source File
# Begin Source File

SOURCE=.\asm\ddN.c
# End Source File
# Begin Source File

SOURCE=.\parse\debug.c
# End Source File
# Begin Source File

SOURCE=.\io\des.c
# End Source File
# Begin Source File

SOURCE=.\fft\dft.c
# End Source File
# Begin Source File

SOURCE=.\engine\dist.c
# End Source File
# Begin Source File

SOURCE=.\engine\distm.c
# End Source File
# Begin Source File

SOURCE=".\builtin\dp-supp.c"
# End Source File
# Begin Source File

SOURCE=.\builtin\dp.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\dyn_load.c
# End Source File
# Begin Source File

SOURCE=.\engine\E.c
# End Source File
# Begin Source File

SOURCE=.\engine\Ebug.c

!IF  "$(CFG)" == "asir2000 - Win32 Release"

# ADD CPP /Oi /Oy
# SUBTRACT CPP /Ox /Og

!ELSEIF  "$(CFG)" == "asir2000 - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\builtin\ec.c
# End Source File
# Begin Source File

SOURCE=.\parse\eval.c
# End Source File
# Begin Source File

SOURCE=.\parse\evall.c
# End Source File
# Begin Source File

SOURCE=.\engine\EZ.c
# End Source File
# Begin Source File

SOURCE=.\engine\F.c
# End Source File
# Begin Source File

SOURCE=.\builtin\fctr.c
# End Source File
# Begin Source File

SOURCE=.\fft\fft_primes.c
# End Source File
# Begin Source File

SOURCE=.\builtin\file.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\finalize.c
# End Source File
# Begin Source File

SOURCE=.\builtin\ftab.c
# End Source File
# Begin Source File

SOURCE=.\parse\function.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\gc_risa.c
# ADD CPP /D "WINNT"
# End Source File
# Begin Source File

SOURCE=.\builtin\gf.c
# End Source File
# Begin Source File

SOURCE=.\asm\gf2m_mul_w.c
# End Source File
# Begin Source File

SOURCE=.\engine\gf2n.c
# End Source File
# Begin Source File

SOURCE=.\engine\gfpn.c
# End Source File
# Begin Source File

SOURCE=.\engine\gfs.c
# End Source File
# Begin Source File

SOURCE=.\engine\gfspn.c
# End Source File
# Begin Source File

SOURCE=.\parse\glob.c
# End Source File
# Begin Source File

SOURCE=.\builtin\gr.c
# End Source File
# Begin Source File

SOURCE=.\engine\H.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\headers.c
# End Source File
# Begin Source File

SOURCE=.\builtin\help.c
# End Source File
# Begin Source File

SOURCE=.\engine\Hgfs.c
# End Source File
# Begin Source File

SOURCE=.\engine\init.c
# End Source File
# Begin Source File

SOURCE=.\builtin\int.c
# End Source File
# Begin Source File

SOURCE=.\io\io.c
# End Source File
# Begin Source File

SOURCE=.\parse\kwd.c
# End Source File
# Begin Source File

SOURCE=.\parse\lex.c
# End Source File
# Begin Source File

SOURCE=.\builtin\list.c
# End Source File
# Begin Source File

SOURCE=.\engine\lmi.c
# End Source File
# Begin Source File

SOURCE=.\parse\load.c
# End Source File
# Begin Source File

SOURCE=.\engine\M.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\mach_dep.c
# End Source File
# Begin Source File

SOURCE=.\parse\main.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\malloc.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\mallocx.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\mark.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\mark_rts.c
# End Source File
# Begin Source File

SOURCE=.\engine\mat.c
# End Source File
# Begin Source File

SOURCE=.\builtin\math.c
# End Source File
# Begin Source File

SOURCE=.\engine\Mgfs.c
# End Source File
# Begin Source File

SOURCE=.\engine\mi.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\misc.c
# End Source File
# Begin Source File

SOURCE=.\builtin\miscf.c
# End Source File
# Begin Source File

SOURCE=.\engine\mt19937.c
# End Source File
# Begin Source File

SOURCE=.\engine\N.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\new_hblk.c
# End Source File
# Begin Source File

SOURCE=.\engine\NEZ.c
# End Source File
# Begin Source File

SOURCE=.\engine\num.c
# End Source File
# Begin Source File

SOURCE=.\builtin\numerical.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\obj_map.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\os_dep.c
# End Source File
# Begin Source File

SOURCE=.\io\ox.c
# End Source File
# Begin Source File

SOURCE=.\engine\P.c
# End Source File
# Begin Source File

SOURCE=.\engine\pari.c
# End Source File
# Begin Source File

SOURCE=.\builtin\parif.c
# End Source File
# Begin Source File

SOURCE=.\parse\parser.c
# End Source File
# Begin Source File

SOURCE=.\engine\PD.c
# End Source File
# Begin Source File

SOURCE=.\builtin\pdiv.c
# End Source File
# Begin Source File

SOURCE=.\engine\PDM.c
# End Source File
# Begin Source File

SOURCE=.\io\pexpr.c
# End Source File
# Begin Source File

SOURCE=.\builtin\pf.c
# End Source File
# Begin Source File

SOURCE=.\engine\PM.c
# End Source File
# Begin Source File

SOURCE=.\fft\polmul.c
# End Source File
# Begin Source File

SOURCE=.\builtin\poly.c
# End Source File
# Begin Source File

SOURCE=.\builtin\print.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\ptr_chck.c
# End Source File
# Begin Source File

SOURCE=.\engine\PU.c
# End Source File
# Begin Source File

SOURCE=.\engine\PUM.c
# End Source File
# Begin Source File

SOURCE=.\parse\puref.c
# End Source File
# Begin Source File

SOURCE=.\parse\pvar.c
# End Source File
# Begin Source File

SOURCE=.\engine\Q.c
# End Source File
# Begin Source File

SOURCE=.\engine\QM.c
# End Source File
# Begin Source File

SOURCE=.\parse\quote.c
# End Source File
# Begin Source File

SOURCE=.\engine\R.c
# End Source File
# Begin Source File

SOURCE=.\builtin\rat.c
# End Source File
# Begin Source File

SOURCE=.\engine\real.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\real_malloc.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\reclaim.c
# End Source File
# Begin Source File

SOURCE=.\builtin\reduct.c
# End Source File
# Begin Source File

SOURCE=.\builtin\result.c
# End Source File
# Begin Source File

SOURCE=.\engine\RU.c
# End Source File
# Begin Source File

SOURCE=.\builtin\sha1.c
# End Source File
# Begin Source File

SOURCE=.\io\sio.c
# End Source File
# Begin Source File

SOURCE=.\io\spexpr.c
# End Source File
# Begin Source File

SOURCE=.\engine\str.c
# End Source File
# Begin Source File

SOURCE=.\builtin\strobj.c
# End Source File
# Begin Source File

SOURCE=.\parse\struct.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\stubborn.c
# End Source File
# Begin Source File

SOURCE=.\builtin\subst.c
# End Source File
# Begin Source File

SOURCE=.\io\tcpf.c
# End Source File
# Begin Source File

SOURCE=.\builtin\time.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\typd_mlc.c
# End Source File
# Begin Source File

SOURCE=.\builtin\type.c
# End Source File
# Begin Source File

SOURCE=.\engine\Ui.c
# End Source File
# Begin Source File

SOURCE=.\engine\up.c
# End Source File
# Begin Source File

SOURCE=.\engine\up2.c
# End Source File
# Begin Source File

SOURCE=.\engine\up_gf2n.c
# End Source File
# Begin Source File

SOURCE=.\engine\up_lm.c
# End Source File
# Begin Source File

SOURCE=.\parse\util.c
# End Source File
# Begin Source File

SOURCE=.\builtin\var.c
# End Source File
# Begin Source File

SOURCE=.\engine\vect.c
# End Source File
# Begin Source File

SOURCE=.\gc5.3\win32_threads.c
# End Source File
# Begin Source File

SOURCE=.\io\ws_fileio.c
# End Source File
# Begin Source File

SOURCE=.\parse\xdeb.c
# End Source File
# Begin Source File

SOURCE=.\parse\ytab.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\include\b.h
# End Source File
# Begin Source File

SOURCE=.\include\base.h
# End Source File
# Begin Source File

SOURCE=".\include\ca-27.h"
# End Source File
# Begin Source File

SOURCE=.\include\ca.h
# End Source File
# Begin Source File

SOURCE=.\parse\comp.h
# End Source File
# Begin Source File

SOURCE=.\parse\cpp.h
# End Source File
# Begin Source File

SOURCE=.\fft\dft.h
# End Source File
# Begin Source File

SOURCE=.\include\inline.h
# End Source File
# Begin Source File

SOURCE=.\include\prime.h
# End Source File
# Begin Source File

SOURCE=.\include\version.h
# End Source File
# Begin Source File

SOURCE=.\parse\y.tab.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
