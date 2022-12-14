@rem $OpenXM: OpenXM_contrib2/windows/help/makehelp2.bat,v 1.6 2016/06/29 15:24:11 ohara Exp $
@echo off

setlocal

set PATH=..;..\..;%PATH%;c:\cygwin\bin;c:\cygwin64\bin;c:\msys64\usr\bin
set lang=%~1
set PERL=perl

REM 相対パスを絶対パスに変換する
REM 重要!! %%~fi と ) の間に空白を入れてはならない!!
for %%i in ( ..\..\..\OpenXM\src ) do (set oxsrc=%%~fi)
for %%i in ( ..\..\asir2000\lib )  do (set srcdir4=%%~fi)

set srcdir1=%oxsrc%\asir-doc
set srcdir2=%oxsrc%\asir-contrib\packages\doc
set srcdir3=%oxsrc%\asir-contrib\packages\src

mkdir %lang%
pushd %lang%

REM case 1: asir 標準マニュアル

mkdir asir\parts\builtin
pushd asir

  for %%i in ( texinfo.tex texinfo.ini txi-ja.tex txi-en.tex ) do (
    nkf32 -e %srcdir1%\%%i > %%i
  )

  for %%i in ( man.texi parts\algnum.texi parts\appendix.texi parts\asir.texi parts\builtin.texi parts\debug.texi parts\ff.texi parts\groebner.texi parts\intro.texi parts\process.texi parts\risa.texi parts\type.texi parts\builtin\array.texi parts\builtin\bit.texi parts\builtin\function.texi parts\builtin\io.texi parts\builtin\list.texi parts\builtin\misc.texi parts\builtin\module.texi parts\builtin\num.texi parts\builtin\numeric.texi parts\builtin\poly.texi parts\builtin\string.texi parts\builtin\structure.texi parts\builtin\type.texi parts\builtin\upoly.texi ) do (
    extract_man %lang% %srcdir1%\%%i | nkf32 -e > %%i
  )

  %PERL% ../../texi2html -menu -split_node man.texi
  call :html_euc_to_sjis
popd

gen_hh asir . 
hhc asirhelp.hhp

REM case 2: asir-contrib マニュアル

mkdir cman
pushd cman

  nkf32 -e %srcdir2%\top\cman-%lang%.texi > cman-%lang%.texi
  nkf32 -e %srcdir3%\names.rr > names.rr

  for %%i in ( base calculus complex glib graphic hypergeometric intro load matrix misc names number oxrfc100 oxshell poly print series util windows ) do (
    nkf32 -e %srcdir2%\top\%%i.oxw | oxweave C %lang% > %%i.%lang%
  )

  for %%i in ( base matrix number poly print util ) do (
    oxweave usage < names.rr | oxgentexi --category %%i --%lang% --upnode %%i --noSorting --author "OpenXM.org" --infoName asir-contrib-%%i > %%i-auto.%lang%
  )

  for %%i in ( glib ) do (
    nkf32 -e %srcdir4%\%%i | oxweave usage | oxgentexi --category %%i --%lang% --upnode %%i --noSorting --author "OpenXM.org" --infoName asir-contrib-%%i > %%i-auto.%lang%
  )

  for %%i in ( oxshell ) do (
    nkf32 -e %srcdir3%\%%i.rr | oxweave usage | oxgentexi --category %%i --%lang% --upnode %%i --noSorting --author "OpenXM.org" --infoName asir-contrib-%%i > %%i-auto.%lang%
  )

  %PERL% ../../texi2html -menu -split_node cman-%lang%.texi
  call :html_euc_to_sjis
popd

gen_hh cman . %lang%
hhc cmanhelp.hhp

REM 個別の場合

REM case 3a: *.{oxg,oxw} から生成。

for %%i in ( dsolv gnuplot mathematica ok_diff ok_dmodule phc tigers ) do (
  mkdir %%i
  pushd %%i
    nkf32 -e %srcdir2%\%%i\%%i.oxg | oxweave usage-%lang% | oxgentexi --noSorting --author "OpenXM.org" --infoName asir-contrib-%%i --title "%%i manual" > %%i-%lang%.texi
    nkf32 -e %srcdir2%\%%i\%%i.oxw  | oxweave C %lang% >  %%i.%lang%
    %PERL% ../../texi2html -menu -split_node %%i-%lang%.texi
    call :html_euc_to_sjis
  popd
  gen_hh %%i . %lang%
  hhc %%ihelp.hhp
)

REM case 3b: plucker, sm1: *.{oxg,oxw,rr} から生成。

for %%i in ( plucker sm1  ) do (
  mkdir %%i
  pushd %%i
    nkf32 -e %srcdir2%\%%i\%%i.oxg | oxweave usage-%lang% | oxgentexi --noSorting --author "OpenXM.org" --infoName asir-contrib-%%i --title "%%i manual" > %%i-%lang%.texi
    nkf32 -e %srcdir2%\%%i\%%i.oxw | oxweave C %lang% >  %%i.%lang%
    nkf32 -e %srcdir3%\%%i.rr | oxweave usage | oxgentexi --category %name% --en --upnode %%i --noSorting --author "OpenXM.org" --infoName asir-contrib-%%i  > %%i-auto.%lang%
    %PERL% ../../texi2html -menu -split_node %%i-%lang%.texi
    call :html_euc_to_sjis
  popd
  gen_hh %%i . %lang%
  hhc %%ihelp.hhp
)

REM case 3c: pfpcoh:  *.{oxg,oxw,rr} から生成。

for %%i in ( pfpcoh  ) do (
  mkdir %%i
  pushd %%i
    nkf32 -e %srcdir2%\%%i\%%i.oxg | oxweave usage-%lang% | oxgentexi --noSorting --author "OpenXM.org" --infoName asir-contrib-%%i --title "%%i manual" > %%i-%lang%.texi
    nkf32 -e %srcdir2%\%%i\%%i.oxw | oxweave C %lang% >  %%i.%lang%
    nkf32 -e %srcdir3%\%%i.rr %srcdir3%\pfphom.rr | oxweave usage | oxgentexi --%lang% --upnode %%i --noSorting --author "OpenXM.org" --infoName asir-contrib-%%i  > %%i-auto.%lang%
    %PERL% ../../texi2html -menu -split_node %%i-%lang%.texi
    call :html_euc_to_sjis
  popd
  gen_hh %%i . %lang%
  hhc %%ihelp.hhp
)

REM case 3d: om: *.{oxg,rr} から生成。

for %%i in ( om ) do (
  mkdir %%i
  pushd %%i
    nkf32 -e %srcdir2%\%%i\%%i.oxg | oxweave usage-%lang% | oxgentexi --noSorting --author "OpenXM.org" --infoName asir-contrib-%%i --title "%%i manual" > %%i-%lang%.texi
    echo /*^&%lang% > %%i.oxw
    nkf32 -e %srcdir3%\%%i.rr >> %%i.oxw
    echo */ >> %%i.oxw
    oxweave C %lang% < %%i.oxw >  %%i.%lang%
    %PERL% ../../texi2html -menu -split_node %%i-%lang%.texi
    call :html_euc_to_sjis
  popd
  gen_hh %%i . %lang%
  hhc %%ihelp.hhp
)

REM case 4a: *.texi から生成。

for %%i in ( nn_ndbf ) do (
  mkdir %%i
  pushd %%i
    nkf32 -e %srcdir2%\%%i\%%i.texi > %%i.texi
    extract_man %lang% %%i.texi > %%i-%lang%.texi
    oxreplace --old @fref --new @ref %%i-%lang%.texi
    %PERL% ../../texi2html -menu -split_node %%i-%lang%.texi
    call :html_euc_to_sjis
  popd
  gen_hh %%i . %lang%
  hhc %%ihelp.hhp
)

REM case 4b: *-{ja,en}.texi から生成。

for %%i in ( ns_twistedlog ) do (
  mkdir %%i
  pushd %%i
    nkf32 -e %srcdir2%\%%i\%%i-%lang%.texi > %%i-%lang%.texi
    %PERL% ../../texi2html -menu -split_node %%i-%lang%.texi
    call :html_euc_to_sjis
  popd
  gen_hh %%i . %lang%
  hhc %%ihelp.hhp
)

REM case 4c: *-ja.texi から生成。日本語のみ。

for %%i in ( noro_matrix noro_mwl noro_pd todo_parametrize ) do (
  if exist %srcdir2%\%%i\%%i-%lang%.texi (
    mkdir %%i
    pushd %%i
      nkf32 -e %srcdir2%\%%i\%%i-%lang%.texi > %%i-%lang%.texi
      %PERL% ../../texi2html -menu -split_node %%i-%lang%.texi
      call :html_euc_to_sjis
    popd
    gen_hh %%i . %lang%
    hhc %%ihelp.hhp
  )
)

popd

exit /b 0

:html_euc_to_sjis
for %%i in ( *.html ) do (
  rename %%i __tmp
  nkf32 -Es __tmp > %%i
  del /q __tmp
)

exit /b 0
