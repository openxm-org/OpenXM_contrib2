@rem $OpenXM: OpenXM_contrib2/windows/makebin32.bat,v 1.7 2013/08/05 11:41:40 ohara Exp $
@echo off

set ARCH=x86

if not exist gc-7.2 ( echo run ./extract_gc.sh in cyngwin & exit /b 1 )
pushd gc-7.2
nmake -f NT_STATIC_THREADS_MAKEFILE gc.lib
popd

pushd pari20
nmake -f Makefile.vc6
popd

for %%i in ( asir2000lib engine2000 mcpp asir32gui ) do (
  pushd %%i
  nmake -f Makefile.vc
  popd
)

pushd asir32gui
nmake -f Makefile.vc LANG=en
popd

pushd ..\asir2000
nmake -f Makefile.vc
popd
