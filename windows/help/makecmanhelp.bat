@echo off
set lang=%1
set srcdir=..\..\..\OpenXM\src\asir-contrib\packages\doc
mkdir src
del /Q src\*.*

nkf32 -s %srcdir%\jtexinfo.tex > src\jtexinfo.tex
nkf32 -s %srcdir%\texinfo-js.tex > src\texinfo-js.tex

nkf32 -s %srcdir%\opening.texi > src\opening.texi
nkf32 -s %srcdir%\otherfunc.texi > src\otherfunc.texi
nkf32 -s %srcdir%\template1.texi > src\template1.texi
nkf32 -s %srcdir%\template2.texi > src\template2.texi
nkf32 -s %srcdir%\top.texi > src\top.texi

nkf32 -s %srcdir%\Diff-%lang%.texi > src\Diff-%lang%.texi
nkf32 -s %srcdir%\Dmodule-%lang%.texi > src\Dmodule-%lang%.texi
nkf32 -s %srcdir%\base-auto-%lang%.texi > src\base-auto-%lang%.texi
nkf32 -s %srcdir%\base-%lang%.texi > src\base-%lang%.texi
nkf32 -s %srcdir%\calculus-%lang%.texi > src\calculus-%lang%.texi
nkf32 -s %srcdir%\cman-%lang%.texi > src\cman-%lang%.texi
nkf32 -s %srcdir%\complex-%lang%.texi > src\complex-%lang%.texi
nkf32 -s %srcdir%\dsolv-%lang%.texi > src\dsolv-%lang%.texi
nkf32 -s %srcdir%\glib-auto-%lang%.texi > src\glib-auto-%lang%.texi
nkf32 -s %srcdir%\glib-%lang%.texi > src\glib-%lang%.texi
nkf32 -s %srcdir%\graphic-%lang%.texi > src\graphic-%lang%.texi
nkf32 -s %srcdir%\hypergeometric-%lang%.texi > src\hypergeometric-%lang%.texi
nkf32 -s %srcdir%\intro-%lang%.texi > src\intro-%lang%.texi
nkf32 -s %srcdir%\longname-%lang%.texi > src\longname-%lang%.texi
nkf32 -s %srcdir%\matrix-auto-%lang%.texi > src\matrix-auto-%lang%.texi
nkf32 -s %srcdir%\matrix-%lang%.texi > src\matrix-%lang%.texi
nkf32 -s %srcdir%\misc-%lang%.texi > src\misc-%lang%.texi
nkf32 -s %srcdir%\names-%lang%.texi > src\names-%lang%.texi
nkf32 -s %srcdir%\number-%lang%.texi > src\number-%lang%.texi
nkf32 -s %srcdir%\oxgnuplot-%lang%.texi > src\oxgnuplot-%lang%.texi
nkf32 -s %srcdir%\oxmath-%lang%.texi > src\oxmath-%lang%.texi
nkf32 -s %srcdir%\oxom-%lang%.texi > src\oxom-%lang%.texi
nkf32 -s %srcdir%\oxphc-%lang%.texi > src\oxphc-%lang%.texi
nkf32 -s %srcdir%\oxsm1-%lang%.texi > src\oxsm1-%lang%.texi
nkf32 -s %srcdir%\oxtigers-%lang%.texi > src\oxtigers-%lang%.texi
nkf32 -s %srcdir%\oxxm-%lang%.texi > src\oxxm-%lang%.texi
nkf32 -s %srcdir%\pfp-auto-%lang%.texi > src\pfp-auto-%lang%.texi
nkf32 -s %srcdir%\pfp-%lang%.texi > src\pfp-%lang%.texi
nkf32 -s %srcdir%\plucker-auto-%lang%.texi > src\plucker-auto-%lang%.texi
nkf32 -s %srcdir%\plucker-%lang%.texi > src\plucker-%lang%.texi
nkf32 -s %srcdir%\poly-auto-%lang%.texi > src\poly-auto-%lang%.texi
nkf32 -s %srcdir%\poly-%lang%.texi > src\poly-%lang%.texi
nkf32 -s %srcdir%\print-auto-%lang%.texi > src\print-auto-%lang%.texi
nkf32 -s %srcdir%\print-%lang%.texi > src\print-%lang%.texi
nkf32 -s %srcdir%\series-%lang%.texi > src\series-%lang%.texi
nkf32 -s %srcdir%\sm1-auto-%lang%.texi > src\sm1-auto-%lang%.texi
nkf32 -s %srcdir%\windows-%lang%.texi > src\windows-%lang%.texi

cd src
jperl ..\texi2html -menu -split_node cman-%lang%.texi
cd ..
mkdir html
del /Q html\*.*
move src\*.* html
gen_hh_new html . cman %lang%
hhc cmanhelp-%lang%.hhp
