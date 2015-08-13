@rem $OpenXM: OpenXM_contrib2/windows/makebin32.bat,v 1.12 2014/12/31 19:57:00 ohara Exp $
@echo off

set ARCH=x86
set INCLUDE=%INCLUDE%;%ProgramFiles(x86)%\Microsoft SDKs\Windows\v7.1A\Include

if not exist gc ( c:\cygwin\bin\bash -c 'PATH=/bin:$PATH ./extract_gc.sh' )
pushd gc
nmake -f NT_STATIC_THREADS_MAKEFILE gc.lib
popd

for %%i in ( asir2000lib engine2000 mcpp asir32gui post-msg-asirgui ..\asir2000 ) do (
  pushd %%i
  nmake -f Makefile.vc
  popd
)
