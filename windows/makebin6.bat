@rem $OpenXM: OpenXM_contrib2/windows/makebin6.bat,v 1.1 2009/03/09 09:33:30 ohara Exp $
@echo off

for %%i in ( pari20 asir2000lib engine2000 cpp asir32gui ) do (
  pushd %%i
  nmake -f Makefile.vc6
  popd
)

pushd asir32gui
nmake -f Makefile.vc6 LANG=en
popd

pushd ..\asir2000
nmake -f Makefile.vc6
popd
