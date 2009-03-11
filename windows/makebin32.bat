@rem $OpenXM$
@echo off

set ARCH=x86
if not defined WITH_PARI set WITH_PARI=yes

cd gc-7.1
nmake -f NT_STATIC_THREADS_MAKEFILE gc.lib
cd ..

if "%WITH_PARI%" == "yes" (
  cd pari20
  nmake -f Makefile.vc6
  cd ..
)

for %%i in ( asir2000lib engine2000 cpp asir32gui ) do (
  cd %%i
  nmake -f Makefile.vc
  cd ..
)

cd asir32gui
nmake -f Makefile.vc LANG=en
cd ..

cd ..\asir2000
nmake -f Makefile.vc
cd ..\windows
