# $OpenXM: OpenXM_contrib2/windows/Makefile,v 1.7 2014/05/14 03:50:15 ohara Exp $

!if "$(ARCH)" == ""
ARCH=$(PROCESSOR_ARCHITECTURE)
!endif

!if "$(ARCH)" == "AMD64"
OS = win64
CANDLE_ARCH = -nologo -arch x64
PGDIR=ProgramFiles64Folder
PRODUCTID=0C667F26-E4B0-4669-A4DE-2929622F9BA2
!else
OS = win32
CANDLE_ARCH = -nologo
PGDIR=ProgramFilesFolder
PRODUCTID=DA2D90B3-D4D8-44EE-B121-3451D329DE02
!endif

UPGRADEID=B7A582F5-0484-4030-9D38-BFAE00707F0D
PRODUCTVER=1.0.4

TARGET= asir$(OS).msi
SRCS= asir.wxs _lib.wxs _contrib.wxs _help.wxs _share.wxs _doc.wxs
# OBJS= asir.wixobj _lib.wixobj _contrib.wixobj _help.wixobj _share.wixobj
OBJS= $(SRCS:.wxs=.wixobj)

CANDLE = candle -nologo
CANDLE_FLAGS = $(CANDLE_ARCH) -dprogramfilesDir=$(PGDIR) \
  -dupgradeID=$(UPGRADEID) -dproductID=$(PRODUCTID) -dproductVer=$(PRODUCTVER) \
  -dlibDir=asir\lib\asir -dcontribDir=asir\lib\asir-contrib \
  -dhelpDir=asir\help -dshareDir=asir\share -ddocDir=asir\doc
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
# _share.wxs:
# 	cd asir
# 	$(HEAT) dir share $(HEATFLAGS) -dr SHARE -cg ShareGrp -var "var.shareDir" -out ..\_share.wxs
# 	cd ..
#
# _contrib.wxs:
# 	cd asir\lib
# 	$(HEAT) dir asir-contrib $(HEATFLAGS) -dr LIB_ASIR_CONTRIB -cg ContribGrp -var "var.contribDir" -out ..\..\_contrib.wxs
# 	cd ..\..
#
# _doc.wxs:
# 	cd asir
# 	$(HEAT) dir doc $(HEATFLAGS) -dr DOC -cg DocGrp -var "var.docDir" -out ..\_doc.wxs
# 	cd ..
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
