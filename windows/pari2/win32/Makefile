# Makefile for Pari\GP -- i586 running Windows 95\NT.
# For Microsoft Visual C version 5.
#
#  This file was created by hand. Update it when config\ evolves
#

CC         = cl
CPPFLAGS   = -I"c:\program files\microsoft visual studio\vc98\include" -I. -I$(PARISRC)\headers
CFLAGS     = -nologo -Gs -Ox -MT
C0FLAGS     = -nologo -Gs
LD         = cl
LDFLAGS    = -nologo
CPP        = cl -nologo -E -I.
PARISRC    = ..\..\..\..\OpenXM_contrib\pari-2.2\src

LIBS = "c:\program files\microsoft visual studio\vc98\lib\libc.lib" "c:\program files\microsoft visual studio\vc98\lib\libcmt.lib" "c:\program files\microsoft visual studio\vc98\lib\oldnames.lib"  "c:\program files\microsoft visual studio\vc98\lib\uuid.lib" "c:\program files\microsoft visual studio\vc98\lib\libcp.lib" "c:\program files\microsoft visual studio\vc98\lib\kernel32.lib"

RM = del

# Graphic library.
PLOTFILE   = plotnull.c
PLOTCFLAGS =
PLOTLIBS   =

#RLINCLUDE = -I..\readline
#RLLIBS    = ..\readline\libreadline.a

OBJS   =  kernel.obj mp.obj alglin1.obj alglin2.obj arith1.obj arith2.obj base1.obj base2.obj base3.obj base4.obj base5.obj bibli1.obj bibli2.obj buch1.obj buch2.obj buch3.obj buch4.obj gen1.obj gen2.obj gen3.obj highlvl.obj ifactor1.obj polarit1.obj polarit2.obj polarit3.obj rootpol.obj trans1.obj trans2.obj trans3.obj elliptic.obj galois.obj galconj.obj subgroup.obj kummer.obj mpqs.obj nffactor.obj stark.obj subfield.obj thue.obj anal.obj compat.obj errmsg.obj es.obj helpmsg.obj init.obj sumiter.obj
OBJSGP =  gp.obj gp_init.obj gp_rl.obj plot.obj plotport.obj
OINLINE =  mpinl.obj

dft: gp.exe libpariw.lib

all: gp.exe libpariw.lib

libpariw.lib: $(OBJS)
	-$(RM) $@
	lib -nologo -out:$@ $(OBJS)

gp.exe: $(OBJS) $(OBJSGP)
	$(LD) $(LDFLAGS) -Fegp.exe $(OBJS) $(OBJSGP) $(RLLIBS) $(PLOTLIBS) $(LIBS)
	@type gp.log

cleantest:
	$(RM) out.* pari.ps

clean: cleantest
	-$(RM) *.obj *.s libpariw.lib gp.exe gp.log *.BAK

HEADERS = pariinl.h $(PARISRC)\headers\pari.h $(PARISRC)\headers\paritype.h $(PARISRC)\headers\paricast.h $(PARISRC)\headers\paricom.h $(PARISRC)\headers\parierr.h $(PARISRC)\headers\parigen.h $(PARISRC)\headers\paristio.h $(PARISRC)\headers\parisys.h $(PARISRC)\headers\pariport.h

kernel.obj: $(PARISRC)\kernel\ix86\l0asm.c $(PARISRC)\kernel\ix86\l0asm.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -Fokernel.obj $(PARISRC)\kernel\ix86\l0asm.c
mp.obj: $(HEADERS) $(PARISRC)\kernel\none\mp.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -Fomp.obj $(PARISRC)\kernel\none\mp.c
mpinl.obj: $(HEADERS) $(PARISRC)\kernel\none\level1.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -Fompinl.obj $(PARISRC)\kernel\none\level1.c

alglin1.obj: $(HEADERS)  $(PARISRC)\basemath\alglin1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foalglin1.obj $(PARISRC)\basemath\alglin1.c
alglin2.obj: $(HEADERS)  $(PARISRC)\basemath\alglin2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foalglin2.obj $(PARISRC)\basemath\alglin2.c
arith1.obj: $(HEADERS)  $(PARISRC)\basemath\arith1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foarith1.obj $(PARISRC)\basemath\arith1.c
arith2.obj: $(HEADERS)  $(PARISRC)\basemath\arith2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foarith2.obj $(PARISRC)\basemath\arith2.c
base1.obj: $(HEADERS) $(PARISRC)\headers\parinf.h $(PARISRC)\basemath\base1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobase1.obj $(PARISRC)\basemath\base1.c
base2.obj: $(HEADERS)  $(PARISRC)\basemath\base2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobase2.obj $(PARISRC)\basemath\base2.c
base3.obj: $(HEADERS)  $(PARISRC)\basemath\base3.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobase3.obj $(PARISRC)\basemath\base3.c
base4.obj: $(HEADERS)  $(PARISRC)\basemath\base4.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobase4.obj $(PARISRC)\basemath\base4.c
base5.obj: $(HEADERS)  $(PARISRC)\basemath\base5.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobase5.obj $(PARISRC)\basemath\base5.c
bibli1.obj: $(HEADERS) $(PARISRC)\headers\parinf.h $(PARISRC)\basemath\bibli1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobibli1.obj $(PARISRC)\basemath\bibli1.c
bibli2.obj: $(HEADERS)  $(PARISRC)\basemath\bibli2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobibli2.obj $(PARISRC)\basemath\bibli2.c
buch1.obj: $(HEADERS)  $(PARISRC)\basemath\buch1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobuch1.obj $(PARISRC)\basemath\buch1.c
buch2.obj: $(HEADERS) $(PARISRC)\headers\parinf.h $(PARISRC)\basemath\buch2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobuch2.obj $(PARISRC)\basemath\buch2.c
buch3.obj: $(HEADERS)  $(PARISRC)\basemath\buch3.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobuch3.obj $(PARISRC)\basemath\buch3.c
buch4.obj: $(HEADERS)  $(PARISRC)\basemath\buch4.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobuch4.obj $(PARISRC)\basemath\buch4.c
gen1.obj: $(HEADERS)  $(PARISRC)\basemath\gen1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fogen1.obj $(PARISRC)\basemath\gen1.c
gen2.obj: $(HEADERS)  $(PARISRC)\basemath\gen2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fogen2.obj $(PARISRC)\basemath\gen2.c
gen3.obj: $(HEADERS)  $(PARISRC)\basemath\gen3.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fogen3.obj $(PARISRC)\basemath\gen3.c
ifactor1.obj: $(HEADERS)  $(PARISRC)\basemath\ifactor1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foifactor1.obj $(PARISRC)\basemath\ifactor1.c
polarit1.obj: $(HEADERS)  $(PARISRC)\basemath\polarit1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fopolarit1.obj $(PARISRC)\basemath\polarit1.c
polarit2.obj: $(HEADERS)  $(PARISRC)\basemath\polarit2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fopolarit2.obj $(PARISRC)\basemath\polarit2.c
polarit3.obj: $(HEADERS)  $(PARISRC)\basemath\polarit3.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fopolarit3.obj $(PARISRC)\basemath\polarit3.c
rootpol.obj: $(HEADERS)  $(PARISRC)\basemath\rootpol.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Forootpol.obj $(PARISRC)\basemath\rootpol.c
trans1.obj: $(HEADERS)  $(PARISRC)\basemath\trans1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fotrans1.obj $(PARISRC)\basemath\trans1.c
trans2.obj: $(HEADERS)  $(PARISRC)\basemath\trans2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fotrans2.obj $(PARISRC)\basemath\trans2.c
trans3.obj: $(HEADERS)  $(PARISRC)\basemath\trans3.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fotrans3.obj $(PARISRC)\basemath\trans3.c
elliptic.obj: $(HEADERS)  $(PARISRC)\modules\elliptic.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foelliptic.obj $(PARISRC)\modules\elliptic.c
galois.obj: $(HEADERS)  $(PARISRC)\modules\galois.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fogalois.obj $(PARISRC)\modules\galois.c
galconj.obj: $(HEADERS)  $(PARISRC)\basemath\galconj.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fogalconj.obj $(PARISRC)\basemath\galconj.c
subgroup.obj: $(HEADERS)  $(PARISRC)\basemath\subgroup.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fosubgroup.obj $(PARISRC)\basemath\subgroup.c
kummer.obj: $(HEADERS)  $(PARISRC)\modules\kummer.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fokummer.obj $(PARISRC)\modules\kummer.c
mpqs.obj: $(HEADERS)  $(PARISRC)\modules\mpqs.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fompqs.obj $(PARISRC)\modules\mpqs.c
nffactor.obj: $(HEADERS)  $(PARISRC)\modules\nffactor.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fonffactor.obj $(PARISRC)\modules\nffactor.c
stark.obj: $(HEADERS)  $(PARISRC)\modules\stark.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fostark.obj $(PARISRC)\modules\stark.c
subfield.obj: $(HEADERS)  $(PARISRC)\modules\subfield.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fosubfield.obj $(PARISRC)\modules\subfield.c
thue.obj: $(HEADERS)  $(PARISRC)\modules\thue.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fothue.obj $(PARISRC)\modules\thue.c
anal.obj: $(HEADERS) $(PARISRC)\language\anal.h $(PARISRC)\headers\parinf.h $(PARISRC)\language\anal.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foanal.obj $(PARISRC)\language\anal.c
compat.obj: $(HEADERS)  $(PARISRC)\language\compat.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Focompat.obj $(PARISRC)\language\compat.c
errmsg.obj: $(HEADERS)  $(PARISRC)\language\errmsg.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foerrmsg.obj $(PARISRC)\language\errmsg.c
es.obj: $(HEADERS) $(PARISRC)\language\anal.h $(PARISRC)\language\es.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foes.obj $(PARISRC)\language\es.c
helpmsg.obj: $(HEADERS)  $(PARISRC)\language\helpmsg.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fohelpmsg.obj $(PARISRC)\language\helpmsg.c
init.obj: $(HEADERS) $(PARISRC)\language\anal.h $(PARISRC)\language\init.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foinit.obj $(PARISRC)\language\init.c
sumiter.obj: $(HEADERS) $(PARISRC)\language\anal.h $(PARISRC)\language\sumiter.c
	$(CC) -c $(C0FLAGS) $(CPPFLAGS)  -Fosumiter.obj $(PARISRC)\language\sumiter.c
gp.obj: $(HEADERS) $(PARISRC)\language\anal.h $(PARISRC)\gp\gp.h .\paricfg.h $(PARISRC)\gp\gp.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -I$(PARISRC)\language $(RLINCLUDE) -Fogp.obj $(PARISRC)\gp\gp.c
gp_init.obj: $(HEADERS) $(PARISRC)\graph\rect.h $(PARISRC)\gp\gp_init.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -I$(PARISRC)\graph -Fogp_init.obj $(PARISRC)\gp\gp_init.c
gp_rl.obj: $(HEADERS) $(PARISRC)\language\anal.h $(PARISRC)\gp\gp.h .\paricfg.h $(PARISRC)\gp\gp_rl.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -I$(PARISRC)\language $(RLINCLUDE) -Fogp_rl.obj $(PARISRC)\gp\gp_rl.c
highlvl.obj: $(HEADERS)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -Fohighlvl.obj $(PARISRC)\gp\highlvl.c
plot.obj: $(HEADERS) $(PARISRC)\graph\rect.h $(PARISRC)\graph\$(PLOTFILE)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(PLOTCFLAGS) -Foplot.obj $(PARISRC)\graph\$(PLOTFILE)
plotport.obj: $(HEADERS) $(PARISRC)\graph\rect.h $(PARISRC)\graph\plotport.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -I$(PARISRC)\graph -Foplotport.obj $(PARISRC)\graph\plotport.c
