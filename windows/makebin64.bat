@rem $OpenXM: OpenXM_contrib2/windows/makebin64.bat,v 1.12 2013/08/30 18:18:11 ohara Exp $
@echo off

set ARCH=AMD64

if not exist gc-7.2 ( c:\cygwin\bin\bash -c 'PATH=/bin:$PATH ./extract_gc.sh' )
pushd gc-7.2
nmake -f NT_X64_STATIC_THREADS_MAKEFILE gc.lib
popd

pushd pari20
nmake -f Makefile.vc6
popd

for %%i in ( asir2000lib engine2000 mcpp asir32gui post-msg-asirgui ..\asir2000 ) do (
  pushd %%i
  nmake -f Makefile.vc
  popd
)
