@echo off
set lang=ja
set instdir=%lang%\asir
mkdir %instdir%
mkdir %instdir%\bin
mkdir %instdir%\help
mkdir %instdir%\lib
mkdir %instdir%\lib-asir-contrib
xcopy /S /Q /Y /EXCLUDE:exfiles ..\..\OpenXM\src\asir-contrib\packages\src %instdir%\lib-asir-contrib
copy ..\asir2000\release\asir.exe %instdir%\bin
copy ..\asir2000\lib\* %instdir%\lib"
copy engine2000\release\engine.exe %instdir%\bin
copy asir32gui\release\asirgui-%lang%.exe %instdir%\bin\asirgui.exe
copy cpp\release\cpp.exe %instdir%\bin
copy help\asirhelp-%lang%.chm %instdir%\help\asirhelp.chm
copy help\cmanhelp-%lang%.chm %instdir%\help\cmanhelp.chm
copy help\dsolvhelp-%lang%.chm %instdir%\help\dsolvhelp.chm
copy help\gnuplothelp-%lang%.chm %instdir%\help\gnuplothelp.chm
copy help\mathematicahelp-%lang%.chm %instdir%\help\mathematicahelp.chm
copy help\ok_diffhelp-%lang%.chm %instdir%\help\ok_diffhelp.chm
copy help\ok_dmodulehelp-%lang%.chm %instdir%\help\ok_dmodulehelp.chm
copy help\omhelp-%lang%.chm %instdir%\help\omhelp.chm
copy help\pfpcohhelp-%lang%.chm %instdir%\help\pfpcohhelp.chm
copy help\phchelp-%lang%.chm %instdir%\help\phchelp.chm
copy help\pluckerhelp-%lang%.chm %instdir%\help\pluckerhelp.chm
copy help\sm1help-%lang%.chm %instdir%\help\sm1help.chm
copy help\tigershelp-%lang%.chm %instdir%\help\tigershelp.chm
copy help\todo_parametrizehelp-%lang%.chm %instdir%\help\todo_parametrizehelp.chm
copy help\noro_matrixhelp-%lang%.chm %instdir%\help\noro_matrixhelp.chm
copy help\noro_mwlhelp-%lang%.chm %instdir%\help\noro_mwlhelp.chm
copy help\noro_pdhelp-%lang%.chm %instdir%\help\noro_pdhelp.chm
copy help\nn_ndbfhelp-%lang%.chm %instdir%\help\nn_ndbfhelp.chm
copy help\ns_twistedloghelp-%lang%.chm %instdir%\help\ns_twistedloghelp.chm
copy asir32gui\UseCurrentDir %instdir%\bin
cd %lang%
tar czf asirwin-%lang%.tgz asir
cd ..

set lang=en
set instdir=%lang%\asir
mkdir %instdir%
mkdir %instdir%\bin
mkdir %instdir%\help
mkdir %instdir%\lib
mkdir %instdir%\lib-asir-contrib
xcopy /S /Q /Y /EXCLUDE:exfiles ..\..\OpenXM\src\asir-contrib\packages\src %instdir%\lib-asir-contrib
copy ..\asir2000\release\asir.exe %instdir%\bin
copy ..\asir2000\lib\* %instdir%\lib"
copy engine2000\release\engine.exe %instdir%\bin
copy asir32gui\release\asirgui-%lang%.exe %instdir%\bin\asirgui.exe
copy cpp\release\cpp.exe %instdir%\bin
copy help\asirhelp-%lang%.chm %instdir%\help\asirhelp.chm
copy help\cmanhelp-%lang%.chm %instdir%\help\cmanhelp.chm
copy help\dsolvhelp-%lang%.chm %instdir%\help\dsolvhelp.chm
copy help\gnuplothelp-%lang%.chm %instdir%\help\gnuplothelp.chm
copy help\mathematicahelp-%lang%.chm %instdir%\help\mathematicahelp.chm
copy help\ok_diffhelp-%lang%.chm %instdir%\help\ok_diffhelp.chm
copy help\ok_dmodulehelp-%lang%.chm %instdir%\help\ok_dmodulehelp.chm
copy help\omhelp-%lang%.chm %instdir%\help\omhelp.chm
copy help\pfpcohhelp-%lang%.chm %instdir%\help\pfpcohhelp.chm
copy help\phchelp-%lang%.chm %instdir%\help\phchelp.chm
copy help\pluckerhelp-%lang%.chm %instdir%\help\pluckerhelp.chm
copy help\sm1help-%lang%.chm %instdir%\help\sm1help.chm
copy help\tigershelp-%lang%.chm %instdir%\help\tigershelp.chm
copy help\todo_parametrizehelp-%lang%.chm %instdir%\help\todo_parametrizehelp.chm
copy help\noro_matrixhelp-%lang%.chm %instdir%\help\noro_matrixhelp.chm
copy help\noro_mwlhelp-%lang%.chm %instdir%\help\noro_mwlhelp.chm
copy help\noro_pdhelp-%lang%.chm %instdir%\help\noro_pdhelp.chm
copy help\nn_ndbfhelp-%lang%.chm %instdir%\help\nn_ndbfhelp.chm
copy help\ns_twistedloghelp-%lang%.chm %instdir%\help\ns_twistedloghelp.chm
copy asir32gui\UseCurrentDir %instdir%\bin
cd %lang%
tar czf asirwin-%lang%.tgz asir
cd ..
