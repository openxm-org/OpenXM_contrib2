@echo off
set lang=%1
set srcdir=..\..\..\OpenXM\src\asir-contrib\packages\doc
mkdir src
del /Q src\*.*

nkf32 -s %srcdir%\jtexinfo.tex > src\jtexinfo.tex
nkf32 -s %srcdir%\texinfo-js.tex > src\texinfo-js.tex

copy %srcdir%\opening.texi src\opening.texi
nkf32 -s %srcdir%\otherfunc.texi > src\otherfunc.texi
nkf32 -s %srcdir%\template1.texi > src\template1.texi
nkf32 -s %srcdir%\template2.texi > src\template2.texi
nkf32 -s %srcdir%\top.texi > src\top.texi

nkf32 -s %srcdir%\Diff-%lang%.texi > src\Diff-%lang%.texi
nkf32 -s %srcdir%\Dmodule-%lang%.texi > src\Dmodule-%lang%.texi
nkf32 -s %srcdir%\Matrix-%lang%.texi > src\Matrix-%lang%.texi
nkf32 -s %srcdir%\base-auto.%lang% > src\base-auto.%lang%
nkf32 -s %srcdir%\base.%lang% > src\base.%lang%
nkf32 -s %srcdir%\calculus.%lang% > src\calculus.%lang%
nkf32 -s %srcdir%\cman-%lang%.texi > src\cman-%lang%.texi
nkf32 -s %srcdir%\complex.%lang% > src\complex.%lang%
nkf32 -s %srcdir%\dsolv.%lang% > src\dsolv.%lang%
nkf32 -s %srcdir%\glib-auto.%lang% > src\glib-auto.%lang%
nkf32 -s %srcdir%\glib.%lang% > src\glib.%lang%
nkf32 -s %srcdir%\graphic.%lang% > src\graphic.%lang%
nkf32 -s %srcdir%\hypergeometric.%lang% > src\hypergeometric.%lang%
nkf32 -s %srcdir%\intro.%lang% > src\intro.%lang%
nkf32 -s %srcdir%\matrix-auto.%lang% > src\matrix-auto.%lang%
nkf32 -s %srcdir%\matrix.%lang% > src\matrix.%lang%
nkf32 -s %srcdir%\misc.%lang% > src\misc.%lang%
nkf32 -s %srcdir%\names.%lang% > src\names.%lang%
nkf32 -s %srcdir%\number-auto.%lang% > src\number-auto.%lang%
nkf32 -s %srcdir%\number.%lang% > src\number.%lang%
nkf32 -s %srcdir%\opening.texi  src\opening-%lang%.texi
nkf32 -s %srcdir%\otherfunc.texi > src\otherfunc.%lang%
nkf32 -s %srcdir%\gnuplot.%lang% > src\gnuplot.%lang%
nkf32 -s %srcdir%\m.%lang% > src\m.%lang%
nkf32 -s %srcdir%\phc.%lang% > src\phc.%lang%
nkf32 -s %srcdir%\oxrfc100.%lang% > src\oxrfc100.%lang%
nkf32 -s %srcdir%\sm1.%lang% > src\sm1.%lang%
nkf32 -s %srcdir%\tigers.%lang% > src\tigers.%lang%
nkf32 -s %srcdir%\oxom.%lang% > src\oxom.%lang%
nkf32 -s %srcdir%\pfp-auto.%lang% > src\pfp-auto.%lang%
nkf32 -s %srcdir%\pfp.%lang% > src\pfp.%lang%
nkf32 -s %srcdir%\plucker-auto.%lang% > src\plucker-auto.%lang%
nkf32 -s %srcdir%\plucker.%lang% > src\plucker.%lang%
nkf32 -s %srcdir%\poly-auto.%lang% > src\poly-auto.%lang%
nkf32 -s %srcdir%\poly.%lang% > src\poly.%lang%
nkf32 -s %srcdir%\print-auto.%lang% > src\print-auto.%lang%
nkf32 -s %srcdir%\print.%lang% > src\print.%lang%
nkf32 -s %srcdir%\series.%lang% > src\series.%lang%
nkf32 -s %srcdir%\sm1-auto.%lang% > src\sm1-auto.%lang%
nkf32 -s %srcdir%\util-auto.%lang% > src\util-auto.%lang%
nkf32 -s %srcdir%\util.%lang% > src\util.%lang%
nkf32 -s %srcdir%\windows.%lang% > src\windows.%lang%

cd src
jperl ..\texi2html -menu -split_node cman-%lang%.texi
cd ..
mkdir html
del /Q html\*.*
move src\*.* html
gen_hh_new html . cman %lang%
hhc cmanhelp-%lang%.hhp
