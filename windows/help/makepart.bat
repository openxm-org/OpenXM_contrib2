@echo off
set name=%1
set lang=%2
set srcdir=..\..\..\OpenXM\src\asir-contrib\packages\doc
mkdir src
del /Q src\*.*

nkf32 -s %srcdir%\%name%\%name%-%lang%.texi > src\%name%-%lang%.texi
nkf32 -s %srcdir%\%name%\%name%.%lang% > src\%name%.%lang%
nkf32 -s %srcdir%\%name%\%name%-auto.%lang% > src\%name%-auto.%lang%

cd src
jperl ..\texi2html -menu -split_node %name%-%lang%.texi
cd ..
mkdir html
del /Q html\*.*
move src\*.* html
gen_hh_new html . %name% %lang%
hhc %name%help-%lang%.hhp
del %name%help-%lang%.hhc
del %name%help-%lang%.hhp
del %name%help-%lang%.hhk
