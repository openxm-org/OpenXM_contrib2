# $OpenXM: OpenXM_contrib2/windows/Makefile,v 1.11 2016/06/29 15:37:21 ohara Exp $

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

all:

msi: $(TARGET)
	copy /b $(TARGET) asir_$(OS)_%DATE:/=.%.msi

wxs: $(SRCS)

zip:
	@makepkg.bat

$(TARGET): $(OBJS) LICENSE.rtf zip
	$(LIGHT) $(LTFLAGS) -out $(TARGET) $(OBJS)

## generating wxs files by heat if the directory asir exists

# _lib.wxs:
# 	cd asir\lib
# 	$(HEAT) dir asir $(HEATFLAGS) -dr LIB_ASIR -cg LibGrp -var "var.libDir" -out ..\..\_lib.wxs
# 	cd ..\..
#
# _help.wxs:
# 	cd asir
# 	$(HEAT) dir help $(HEATFLAGS) -dr HELP  -cg HelpGrp -var "var.helpDir"  -out ..\_help.wxs
# 	cd ..
#
# _contrib.wxs:
# 	cd asir\lib
# 	$(HEAT) dir asir-contrib $(HEATFLAGS) -dr LIB_ASIR_CONTRIB -cg ContribGrp -var "var.contribDir" -out ..\..\_contrib.wxs
# 	cd ..\..
#
# wxsclean:
# 	@del /q _*.wxs

clean:
	@del /q *.wixobj
	@del /q *.wixpdb

distclean: clean
	@distclean.bat

msiclean:
	@del /q $(TARGET)
	@del /q asir_$(OS)_*.msi
	@del /q asir_$(OS)_*.zip
