@rem $OpenXM$
@echo off

cd gc-7.1
nmake -f NT_X64_STATIC_THREADS_MAKEFILE gc.lib
cd ..

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
