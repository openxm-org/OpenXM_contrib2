@rem $OpenXM: OpenXM_contrib2/windows/makebin32.bat,v 1.15 2018/09/09 07:41:01 ohara Exp $
@echo off

set ARCH=x86

setlocal
set PATH=%PATH%;c:\cygwin\bin;c:\cygwin64\bin;c:\msys64\usr\bin
if not exist gc ( bash -c 'PATH=/bin:$PATH ./extract_gc.sh' )
endlocal

pushd gc
nmake -f NT_STATIC_THREADS_MAKEFILE gc.lib
popd

for %%i in ( asir2000lib engine2000 mcpp asir32gui post-msg-asirgui ..\asir2018 ) do (
  pushd %%i
  nmake -f Makefile.vc
  popd
)
