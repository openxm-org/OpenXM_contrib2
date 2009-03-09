@rem $OpenXM$
@echo off

for %%i in ( pari20 asir2000lib engine2000 cpp asir32gui ) do (
  cd %%i
  nmake -f Makefile.vc6
  cd ..
)

cd asir32gui
nmake -f Makefile.vc6 LANG=en
cd ..

cd ..\asir2000
nmake -f Makefile.vc6
cd ..\windows
