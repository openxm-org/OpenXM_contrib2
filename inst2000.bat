@rem set instdir=%1
set instdir="c:\program files\asir"
copy asir2000\release\asir.exe %instdir%\bin
copy asir2000\lib\* %instdir%\lib"
copy windows\engine2000\release\engine.exe %instdir%\bin
copy windows\asir32gui\release\asirgui.exe %instdir%\bin
copy windows\cpp\release\cpp.exe %instdir%\bin
copy windows\help\asirhelp.chm %instdir%\bin
