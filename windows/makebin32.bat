@rem $OpenXM: OpenXM_contrib2/windows/makebin32.bat,v 1.13 2015/08/13 13:45:26 ohara Exp $
@echo off

set ARCH=x86
set INCLUDE=%INCLUDE%;%ProgramFiles(x86)%\Microsoft SDKs\Windows\v7.1A\Include

setlocal
set PATH=%PATH%;c:\cygwin\bin;c:\cygwin64\bin
if not exist gc ( bash -c 'PATH=/bin:$PATH ./extract_gc.sh' )
endlocal

pushd gc
nmake -f NT_STATIC_THREADS_MAKEFILE gc.lib
popd

for %%i in ( asir2000lib engine2000 mcpp asir32gui post-msg-asirgui ..\asir2000 ) do (
  pushd %%i
  nmake -f Makefile.vc
  popd
)
