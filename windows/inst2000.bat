if afo%1 == afo goto DEFAULT
set lang=%1
goto MAIN

:DEFAULT
set lang=ja

:MAIN
set instdir="c:\program files\asir"
copy %lang%\asir\bin\*.* %instdir%\bin
copy %lang%\asir\lib\*.* %instdir%\lib
