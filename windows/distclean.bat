@echo off

if exist asir   ( rmdir /s /q asir )
if exist gc-7.2 ( rmdir /s /q gc-7.2 )
if exist pari20\pari-2.0.17.beta ( rmdir /s /q pari20\pari-2.0.17.beta )
pushd pari20
nmake -f Makefile.vc6 distclean
popd
for %%i in ( asir2000lib engine2000 mcpp asir32gui post-msg-asirgui ..\asir2000 ) do (
  pushd %%i
  nmake -f Makefile.vc distclean
  popd
)

exit /b 0
