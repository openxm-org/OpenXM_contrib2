@echo off

msdev asir2000\asir2000.dsp /make "asir2000 - release"
msdev windows\asir2000lib\asir2000lib.dsp /make "asir2000lib - release"
msdev windows\engine2000\engine2000.dsp /make "engine2000 - release"

del windows\asir32gui\Release\asir32gui.res
copy windows\asir32gui\asir32gui.rc.JP windows\asir32gui\asir32gui.rc
msdev windows\asir32gui\asir32gui.dsp /make "asir32gui - release"
move windows\asir32gui\Release\asirgui.exe windows\asir32gui\Release\asirgui-ja.exe 

del windows\asir32gui\Release\asir32gui.res
copy windows\asir32gui\asir32gui.rc.EG windows\asir32gui\asir32gui.rc
msdev windows\asir32gui\asir32gui.dsp /make "asir32gui - release"
move windows\asir32gui\Release\asirgui.exe windows\asir32gui\Release\asirgui-en.exe 

msdev windows\cpp\cpp.dsp /make "cpp - release"

cd windows\help
call makehelp JP
move asirhelp.chm asirhelp-ja.chm
call makehelp EG
move asirhelp.chm asirhelp-en.chm
call makecmanhelp ja
call makecmanhelp en

cd ..
call makedist

cd ..
