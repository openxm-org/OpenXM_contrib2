@echo off
if afo%1 == afo goto USAGE

msdev asir2000\asir2000.dsp /make "asir2000 - release"
msdev windows\asir2000lib\asir2000lib.dsp /make "asir2000lib - release"
msdev windows\engine2000\engine2000.dsp /make "engine2000 - release"
del windows\asir32gui\release\asir32gui.res
copy windows\asir32gui\asir32gui.rc.%1 windows\asir32gui\asir32gui.rc
msdev windows\asir32gui\asir32gui.dsp /make "asir32gui - release"
msdev windows\cpp\cpp.dsp /make "cpp - release"
cd windows\help
call makehelp %1
cd ..\..
goto END

:USAGE
echo usage : make2000 EG   or    make2000 JP

:END
