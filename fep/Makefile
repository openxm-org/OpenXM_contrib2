#
# Copyright (c) 1987-1991 by Software Research Associates, Inc.
#
# $Header$
#
# Options:
#	KANJI:	SHIFT-JIS kanji code handling on ASCII KANJI UNIX
#
CC=gcc
TARGET	= fep
HEADER	= fep_defs.h fep_glob.h fep_funcs.h fep_stat.h
SRC	= fep_main.c fep_edit.c fep_hist.c fep_com.c fep_funcs.c fep_set.c \
	    fep_vi.c fep_util.c fep_alias.c
OBJ	= fep_main.o fep_edit.o fep_hist.o fep_com.o fep_funcs.o fep_set.o \
	    fep_vi.o fep_util.o fep_alias.o
DISTRIBUTION\
	= Makefile fep.1 $(HEADER) $(SRC) feprc.sample README

INSTDIR	= ${PREFIX}/bin

#STAT	= -DSTAT#		Delete # for statistic info
#KANJI	= -DKANJI#		Delete # for KANJI handling
CONFIG	= $(STAT) $(KANJI) -DTERMIOS -D_ALL_SOURCE

#DEBUG	= -g
#DEBUG	= -O

CFLAGS	+= $(DEBUG) $(CONFIG)
LDFLAGS = -ltermcap

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJ) $(LDFLAGS)

argv: fep_com.c
	$(CC) -o argv -DMKARGDEBUG fep_util.c

install: $(TARGET)
	install -s $(TARGET) $(INSTDIR)
	gzip -9n -c fep.1 > ${PREFIX}/man/man1/fep.1.gz

fep.1: fep.jman
	sed -e '/^\.EG/d' -e '/^\.JP/,/^\.EJ/d' -e '/^\.\\"KS/,/^\.\\"KE/d' \
	-e '/^\.\\"K/d' fep.jman > $@

README.jis: README
	jcat -IJ README > $@

fep.tar: $(DISTRIBUTION)
	tar cvf $@ $(DISTRIBUTION)

# rogue-monster version shar
shar: fep.shr1

TARGETSIZE=40000
fep.shr1: $(DISTRIBUTION)
	shar -f fep -m $(TARGETSIZE) $(DISTRIBUTION)

# tektronix shar
tshar: $(DISTRIBUTION)
	shar $(DISTRIBUTION) > fep.shar

clean:
	rm -f *.o $(TARGET)

distversion: $(DISTRIBUTION)
	@ident $(DISTRIBUTION) | grep Header
	
fep_main.o:	fep_defs.h fep_glob.h fep_funcs.h fep_stat.h
fep_com.o:	fep_defs.h fep_glob.h fep_funcs.h
fep_edit.o:	fep_defs.h fep_glob.h fep_funcs.h
fep_funcs.o:	fep_funcs.h
fep_hist.o:	fep_defs.h fep_glob.h fep_funcs.h
fep_set.o:	fep_defs.h fep_glob.h
fep_util.o:	fep_defs.h
fep_vi.o:	fep_defs.h fep_glob.h fep_funcs.h
