@echo off
set lang=%1
set srcdir=..\..\..\OpenXM\src\asir-contrib\packages\doc
mkdir src

nkf32 -s %srcdir%\jtexinfo.tex > src\jtexinfo.tex
nkf32 -s %srcdir%\texinfo-js.tex > src\texinfo-js.tex

nkf32 -s %srcdir%\cman-%lang%.texi > src\cman-%lang%.texi
nkf32 -s %srcdir%\base-auto-%lang%.texi > src\base-auto-%lang%.texi
nkf32 -s %srcdir%\base-%lang%.texi > src\base-%lang%.texi
nkf32 -s %srcdir%\calculus-%lang%.texi > src\calculus-%lang%.texi
nkf32 -s %srcdir%\complex-%lang%.texi > src\complex-%lang%.texi
nkf32 -s %srcdir%\graphic-%lang%.texi > src\graphic-%lang%.texi
nkf32 -s %srcdir%\hypergeometric-%lang%.texi > src\hypergeometric-%lang%.texi
nkf32 -s %srcdir%\matrix-auto-%lang%.texi > src\matrix-auto-%lang%.texi
nkf32 -s %srcdir%\matrix-%lang%.texi > src\matrix-%lang%.texi
nkf32 -s %srcdir%\misc-%lang%.texi > src\misc-%lang%.texi
nkf32 -s %srcdir%\names-%lang%.texi > src\names-%lang%.texi
nkf32 -s %srcdir%\number-%lang%.texi > src\number-%lang%.texi
nkf32 -s %srcdir%\pfp-auto-%lang%.texi > src\pfp-auto-%lang%.texi
nkf32 -s %srcdir%\poly-auto-%lang%.texi > src\poly-auto-%lang%.texi
nkf32 -s %srcdir%\poly-%lang%.texi > src\poly-%lang%.texi
nkf32 -s %srcdir%\print-auto-%lang%.texi > src\print-auto-%lang%.texi
nkf32 -s %srcdir%\print-%lang%.texi > src\print-%lang%.texi
nkf32 -s %srcdir%\series-%lang%.texi > src\series-%lang%.texi

cd src
jperl ..\texi2html -menu -split_node cman-%lang%.texi
cd ..
mkdir html
del /Q html\*.*
move src\*.* html
gen_hh_new html . cman %lang%
hhc cmanhelp-%lang%.hhp
