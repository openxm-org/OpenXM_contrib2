@echo off

devenv pari20\win32com\pari20.sln /build Release
devenv ..\asir2000\asir2000.sln /build Release
devenv asir2000lib\asir2000lib.sln /build Release
devenv pari20\win32\pari20.sln /build Release
devenv engine2000\engine2000.sln /build Release

del asir32gui\Release\asir32gui.res
copy asir32gui\asir32gui.rc.JP asir32gui\asir32gui.rc
devenv asir32gui\asir32gui.sln /build Release
move asir32gui\Release\asirgui.exe asir32gui\Release\asirgui-ja.exe 

del asir32gui\Release\asir32gui.res
copy asir32gui\asir32gui.rc.EG asir32gui\asir32gui.rc
devenv asir32gui\asir32gui.sln /build Release
move asir32gui\Release\asirgui.exe asir32gui\Release\asirgui-en.exe 

devenv cpp\cpp.sln /build Release

cd help
call makehelp JP
move asirhelp.chm asirhelp-ja.chm
call makehelp EG
move asirhelp.chm asirhelp-en.chm
call makecmanhelp ja
call makecmanhelp en
call makecall ja
call makecall en

cd ..
call makedist
