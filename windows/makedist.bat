set lang=ja
set instdir=%lang%\asir
mkdir %instdir%
mkdir %instdir%\bin
mkdir %instdir%\lib
copy ..\asir2000\release\asir.exe %instdir%\bin
copy ..\asir2000\lib\* %instdir%\lib"
copy engine2000\release\engine.exe %instdir%\bin
copy asir32gui\release\asirgui-%lang%.exe %instdir%\bin\asirgui.exe
copy cpp\release\cpp.exe %instdir%\bin
copy help\asirhelp-%lang%.chm %instdir%\bin\asirhelp.chm
copy asir32gui\UseCurrentDir %instdir%\bin
cd %lang%
tar czf asirwin-%lang%.tgz asir
cd ..

set lang=en
set instdir=%lang%\asir
mkdir %instdir%
mkdir %instdir%\bin
mkdir %instdir%\lib
copy ..\asir2000\release\asir.exe %instdir%\bin
copy ..\asir2000\lib\* %instdir%\lib"
copy engine2000\release\engine.exe %instdir%\bin
copy asir32gui\release\asirgui-%lang%.exe %instdir%\bin\asirgui.exe
copy cpp\release\cpp.exe %instdir%\bin
copy help\asirhelp-%lang%.chm %instdir%\bin\asirhelp.chm
copy asir32gui\UseCurrentDir %instdir%\bin
cd %lang%
tar czf asirwin-%lang%.tgz asir
cd ..
