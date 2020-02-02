# $OpenXM: OpenXM_contrib2/windows/Makefile,v 1.13 2020/02/02 05:24:44 ohara Exp $

!if "$(ARCH)" == ""
ARCH=$(PROCESSOR_ARCHITECTURE)
!endif

!if "$(ARCH)" == "AMD64"
OS = win64
CANDLE_ARCH = -nologo -arch x64
PGDIR=ProgramFiles64Folder
PRODUCTID=5FCFE322-2C5D-4AF6-AB56-490A86831FD7
!else
OS = win32
CANDLE_ARCH = -nologo
PGDIR=ProgramFilesFolder
PRODUCTID=4B09EBA0-88B0-4CA5-B284-AA13AC6CCA72
!endif

# DO NOT CHANGE UPGRADEID!!
UPGRADEID=B7A582F5-0484-4030-9D38-BFAE00707F0D
PRODUCTVER=1.1.0

TARGET= asir$(OS).msi
SRCS= asir.wxs _lib.wxs _contrib.wxs _share.wxs
# OBJS= asir.wixobj _lib.wixobj _contrib.wixobj _help.wixobj _share.wixobj
OBJS= $(SRCS:.wxs=.wixobj)

CANDLE = candle -nologo
CANDLE_FLAGS = $(CANDLE_ARCH) -dprogramfilesDir=$(PGDIR) \
  -dupgradeID=$(UPGRADEID) -dproductID=$(PRODUCTID) -dproductVer=$(PRODUCTVER) \
  -dlibDir=asir\lib\asir -dcontribDir=asir\lib\asir-contrib \
  -dshareDir=asir\share
LIGHT = light -nologo
LTFLAGS = -ext WixUIExtension -ext WixUtilExtension
HEAT = heat
HEATFLAGS = -nologo -gg -g1 -sfrag -srd

.SUFFIXES: .wxs .wixobj

.wxs.wixobj:
	$(CANDLE) $(CANDLE_FLAGS) $<

all: build

build: ..\asir-gc\gc\gc.lib asir2000lib\asir2000lib.lib engine2000\engine.exe asir32gui\asirgui.exe mcpp\cpp.exe post-msg-asirgui\cmdasir.exe ..\asir2018\asir.exe

..\asir-gc\gc\gc.lib:
	cd ..\asir-gc && $(MAKE) -e -f Makefile.vc

asir2000lib\asir2000lib.lib: ..\asir-gc\gc\gc.lib
	cd $(@D) && $(MAKE) -e -f Makefile.vc ARCH=$(ARCH)

engine2000\engine.exe: asir2000lib\asir2000lib.lib
	cd $(@D) && $(MAKE) -e -f Makefile.vc ARCH=$(ARCH)

asir32gui\asirgui.exe:
	cd $(@D) && $(MAKE) -e -f Makefile.vc

mcpp\cpp.exe:
	cd $(@D) && $(MAKE) -e -f Makefile.vc

post-msg-asirgui\cmdasir.exe:
	cd $(@D) && $(MAKE) -e -f Makefile.vc

..\asir2018\asir.exe: ..\asir-gc\gc\gc.lib
	cd $(@D) && $(MAKE) -e -f Makefile.vc ARCH=$(ARCH)

msi: $(TARGET)
	copy /b $(TARGET) asir_$(OS)_%DATE:/=.%.msi

wxs: $(SRCS)

zip: build
	@makepkg.bat

$(TARGET): $(OBJS) LICENSE.rtf zip
	$(LIGHT) $(LTFLAGS) -out $(TARGET) $(OBJS)

## generating wxs files by heat if the directory asir exists

!IFDEF WXS
_lib.wxs: zip
	cd asir\lib && $(HEAT) dir asir $(HEATFLAGS) -dr LIB_ASIR -cg LibGrp -var "var.libDir" -out ..\..\_lib.wxs

_help.wxs: zip
	cd asir && $(HEAT) dir help $(HEATFLAGS) -dr HELP  -cg HelpGrp -var "var.helpDir"  -out ..\_help.wxs

_contrib.wxs: zip
	cd asir\lib && $(HEAT) dir asir-contrib $(HEATFLAGS) -dr LIB_ASIR_CONTRIB -cg ContribGrp -var "var.contribDir" -out ..\..\_contrib.wxs
!ENDIF

wxsclean:
	@del /q _*.wxs

clean:
	@del /q *.wixobj
	@del /q *.wixpdb

distclean: clean
	@if exist asir ( rmdir /s /q asir )
	@for %i in ( ..\asir-gc asir2000lib engine2000 mcpp asir32gui post-msg-asirgui ..\asir2018 ) do ( pushd %i && nmake -f Makefile.vc distclean && popd)

msiclean:
	@del /q $(TARGET)
	@del /q asir_$(OS)_*.msi
	@del /q asir_$(OS)_*.zip
