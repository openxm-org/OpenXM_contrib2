@echo off
set lang=%1
set srcdir=..\..\..\OpenXM\src\asir-doc
mkdir parts
mkdir parts\builtin
mkdir src
mkdir src\parts
mkdir src\parts\builtin

nkf32 -s %srcdir%\jtexinfo.tex > src\jtexinfo.tex
nkf32 -s %srcdir%\texinfo-js.tex > src\texinfo-js.tex

nkf32 -s %srcdir%\man.texi > man.texi
nkf32 -s %srcdir%\parts\algnum.texi > parts\algnum.texi
nkf32 -s %srcdir%\parts\appendix.texi > parts\appendix.texi
nkf32 -s %srcdir%\parts\asir.texi > parts\asir.texi
nkf32 -s %srcdir%\parts\builtin.texi > parts\builtin.texi
nkf32 -s %srcdir%\parts\debug.texi > parts\debug.texi
nkf32 -s %srcdir%\parts\ff.texi > parts\ff.texi
nkf32 -s %srcdir%\parts\groebner.texi > parts\groebner.texi
nkf32 -s %srcdir%\parts\intro.texi > parts\intro.texi
nkf32 -s %srcdir%\parts\process.texi > parts\process.texi
nkf32 -s %srcdir%\parts\risa.texi > parts\risa.texi
nkf32 -s %srcdir%\parts\type.texi > parts\type.texi
nkf32 -s %srcdir%\parts\builtin\array.texi > parts\builtin\array.texi
nkf32 -s %srcdir%\parts\builtin\bit.texi > parts\builtin\bit.texi
nkf32 -s %srcdir%\parts\builtin\function.texi > parts\builtin\function.texi
nkf32 -s %srcdir%\parts\builtin\io.texi > parts\builtin\io.texi
nkf32 -s %srcdir%\parts\builtin\list.texi > parts\builtin\list.texi
nkf32 -s %srcdir%\parts\builtin\misc.texi > parts\builtin\misc.texi
nkf32 -s %srcdir%\parts\builtin\num.texi > parts\builtin\num.texi
nkf32 -s %srcdir%\parts\builtin\poly.texi > parts\builtin\poly.texi
nkf32 -s %srcdir%\parts\builtin\string.texi > parts\builtin\string.texi
nkf32 -s %srcdir%\parts\builtin\structure.texi > parts\builtin\structure.texi
nkf32 -s %srcdir%\parts\builtin\type.texi > parts\builtin\type.texi
nkf32 -s %srcdir%\parts\builtin\upoly.texi > parts\builtin\upoly.texi

extract_man %lang% man.texi parts\algnum.texi src
extract_man %lang% parts\appendix.texi parts\asir.texi parts\builtin.texi src
extract_man %lang% parts\debug.texi parts\ff.texi parts\groebner.texi parts\intro.texi src
extract_man %lang% parts\process.texi parts\risa.texi parts\type.texi parts\builtin\array.texi src
extract_man %lang% parts\builtin\bit.texi parts\builtin\function.texi parts\builtin\io.texi src
extract_man %lang% parts\builtin\list.texi parts\builtin\misc.texi parts\builtin\num.texi src
extract_man %lang% parts\builtin\poly.texi parts\builtin\string.texi parts\builtin\structure.texi src
extract_man %lang% parts\builtin\type.texi parts\builtin\upoly.texi src

cd src
jperl ..\texi2html -menu -split_node man.texi
cd ..
mkdir html
move src\*.* html
gen_hh html .
hhc asirhelp.hhp
