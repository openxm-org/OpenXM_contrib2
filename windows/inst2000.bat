if afo%1 == afo goto DEFAULT
set lang=%1
goto MAIN

:DEFAULT
set lang=ja

:MAIN
set instdir="c:\program files\asir"
mkdir %instdir%\bin
mkdir %instdir%\lib
mkdir %instdir%\lib-asir-contrib
xcopy /S /Q /Y %lang%\asir\bin %instdir%\bin
xcopy /S /Q /Y %lang%\asir\lib %instdir%\lib
xcopy /S /Q /Y %lang%\asir\lib-asir-contrib %instdir%\lib-asir-contrib
