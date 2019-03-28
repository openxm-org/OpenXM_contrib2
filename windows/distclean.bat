@echo off

if exist asir   ( rmdir /s /q asir )
if exist gc ( rmdir /s /q gc )
for %%i in ( asir2000lib engine2000 mcpp asir32gui post-msg-asirgui ..\asir2018 ) do (
  pushd %%i
  nmake -f Makefile.vc distclean
  popd
)

exit /b 0
