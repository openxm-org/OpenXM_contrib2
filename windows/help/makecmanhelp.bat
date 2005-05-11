@echo off
set lang=%1
set srcdir=..\..\..\OpenXM\src\asir-contrib\packages\doc
mkdir src
del /Q src\*.*

nkf32 -s %srcdir%\top\cman-%lang%.texi > src\cman-%lang%.texi
nkf32 -s %srcdir%\top\base-auto.%lang% > src\base-auto.%lang%
nkf32 -s %srcdir%\top\base.%lang% > src\base.%lang%
nkf32 -s %srcdir%\top\calculus.%lang% > src\calculus.%lang%
nkf32 -s %srcdir%\top\complex.%lang% > src\complex.%lang%
nkf32 -s %srcdir%\top\glib-auto.%lang% > src\glib-auto.%lang%
nkf32 -s %srcdir%\top\glib.%lang% > src\glib.%lang%
nkf32 -s %srcdir%\top\graphic.%lang% > src\graphic.%lang%
nkf32 -s %srcdir%\top\hypergeometric.%lang% > src\hypergeometric.%lang%
nkf32 -s %srcdir%\top\intro.%lang% > src\intro.%lang%
nkf32 -s %srcdir%\top\matrix-auto.%lang% > src\matrix-auto.%lang%
nkf32 -s %srcdir%\top\matrix.%lang% > src\matrix.%lang%
nkf32 -s %srcdir%\top\misc.%lang% > src\misc.%lang%
nkf32 -s %srcdir%\top\names.%lang% > src\names.%lang%
nkf32 -s %srcdir%\top\number-auto.%lang% > src\number-auto.%lang%
nkf32 -s %srcdir%\top\number.%lang% > src\number.%lang%
nkf32 -s %srcdir%\top\oxrfc100.%lang% > src\oxrfc100.%lang%
nkf32 -s %srcdir%\top\oxshell-auto.%lang% > src\oxshell-auto.%lang%
nkf32 -s %srcdir%\top\oxshell.%lang% > src\oxshell.%lang%
nkf32 -s %srcdir%\top\poly-auto.%lang% > src\poly-auto.%lang%
nkf32 -s %srcdir%\top\poly.%lang% > src\poly.%lang%
nkf32 -s %srcdir%\top\print-auto.%lang% > src\print-auto.%lang%
nkf32 -s %srcdir%\top\print.%lang% > src\print.%lang%
nkf32 -s %srcdir%\top\series.%lang% > src\series.%lang%
nkf32 -s %srcdir%\top\util-auto.%lang% > src\util-auto.%lang%
nkf32 -s %srcdir%\top\util.%lang% > src\util.%lang%
nkf32 -s %srcdir%\top\windows.%lang% > src\windows.%lang%

cd src
jperl ..\texi2html -menu -split_node cman-%lang%.texi
cd ..
mkdir html
del /Q html\*.*
move src\*.* html
gen_hh_new html . cman %lang%
hhc cmanhelp-%lang%.hhp
