# Makefile for Pari\GP -- i586 running Windows 95\NT.
# For Microsoft Visual C version 5.
#
#  This file was created by hand. Update it when config\ evolves
#

CC         = cl
CPPFLAGS   = -I"c:\program files\microsoft visual studio\vc98\include" -I. -I..\src\headers
CFLAGS     = -nologo -Gs -Ox -MT
C0FLAGS     = -nologo -Gs
LD         = cl
LDFLAGS    = -nologo
CPP        = cl -nologo -E -I.

LIBS = "c:\program files\microsoft visual studio\vc98\lib\libc.lib" "c:\program files\microsoft visual studio\vc98\lib\oldnames.lib"  "c:\program files\microsoft visual studio\vc98\lib\uuid.lib"  "c:\program files\microsoft visual studio\vc98\lib\libcp.lib" "c:\program files\microsoft visual studio\vc98\lib\kernel32.lib"

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
	$(LD) $(LDFLAGS) -Fegp.exe $(OBJS) $(OBJSGP) $(RLLIBS) $(PLOTLIBS) $(LIBS) > gp.log
	@type gp.log

cleantest:
	$(RM) out.* pari.ps

clean: cleantest
	-$(RM) *.obj *.s libpariw.lib gp.exe gp.log *.BAK

HEADERS = pariinl.h ..\src\headers\pari.h ..\src\headers\paritype.h ..\src\headers\paricast.h ..\src\headers\paricom.h ..\src\headers\parierr.h ..\src\headers\parigen.h ..\src\headers\paristio.h ..\src\headers\parisys.h ..\src\headers\pariport.h

kernel.obj: ..\src\kernel\ix86\l0asm.c ..\src\kernel\ix86\l0asm.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -Fokernel.obj ..\src\kernel\ix86\l0asm.c
mp.obj: $(HEADERS) ..\src\kernel\none\mp.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -Fomp.obj ..\src\kernel\none\mp.c
mpinl.obj: $(HEADERS) ..\src\kernel\none\level1.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -Fompinl.obj ..\src\kernel\none\level1.c

alglin1.obj: $(HEADERS)  ..\src\basemath\alglin1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foalglin1.obj ..\src\basemath\alglin1.c
alglin2.obj: $(HEADERS)  ..\src\basemath\alglin2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foalglin2.obj ..\src\basemath\alglin2.c
arith1.obj: $(HEADERS)  ..\src\basemath\arith1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foarith1.obj ..\src\basemath\arith1.c
arith2.obj: $(HEADERS)  ..\src\basemath\arith2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foarith2.obj ..\src\basemath\arith2.c
base1.obj: $(HEADERS) ..\src\headers\parinf.h ..\src\basemath\base1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobase1.obj ..\src\basemath\base1.c
base2.obj: $(HEADERS)  ..\src\basemath\base2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobase2.obj ..\src\basemath\base2.c
base3.obj: $(HEADERS)  ..\src\basemath\base3.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobase3.obj ..\src\basemath\base3.c
base4.obj: $(HEADERS)  ..\src\basemath\base4.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobase4.obj ..\src\basemath\base4.c
base5.obj: $(HEADERS)  ..\src\basemath\base5.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobase5.obj ..\src\basemath\base5.c
bibli1.obj: $(HEADERS) ..\src\headers\parinf.h ..\src\basemath\bibli1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobibli1.obj ..\src\basemath\bibli1.c
bibli2.obj: $(HEADERS)  ..\src\basemath\bibli2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobibli2.obj ..\src\basemath\bibli2.c
buch1.obj: $(HEADERS)  ..\src\basemath\buch1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobuch1.obj ..\src\basemath\buch1.c
buch2.obj: $(HEADERS) ..\src\headers\parinf.h ..\src\basemath\buch2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobuch2.obj ..\src\basemath\buch2.c
buch3.obj: $(HEADERS)  ..\src\basemath\buch3.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobuch3.obj ..\src\basemath\buch3.c
buch4.obj: $(HEADERS)  ..\src\basemath\buch4.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fobuch4.obj ..\src\basemath\buch4.c
gen1.obj: $(HEADERS)  ..\src\basemath\gen1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fogen1.obj ..\src\basemath\gen1.c
gen2.obj: $(HEADERS)  ..\src\basemath\gen2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fogen2.obj ..\src\basemath\gen2.c
gen3.obj: $(HEADERS)  ..\src\basemath\gen3.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fogen3.obj ..\src\basemath\gen3.c
ifactor1.obj: $(HEADERS)  ..\src\basemath\ifactor1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foifactor1.obj ..\src\basemath\ifactor1.c
polarit1.obj: $(HEADERS)  ..\src\basemath\polarit1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fopolarit1.obj ..\src\basemath\polarit1.c
polarit2.obj: $(HEADERS)  ..\src\basemath\polarit2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fopolarit2.obj ..\src\basemath\polarit2.c
polarit3.obj: $(HEADERS)  ..\src\basemath\polarit3.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fopolarit3.obj ..\src\basemath\polarit3.c
rootpol.obj: $(HEADERS)  ..\src\basemath\rootpol.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Forootpol.obj ..\src\basemath\rootpol.c
trans1.obj: $(HEADERS)  ..\src\basemath\trans1.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fotrans1.obj ..\src\basemath\trans1.c
trans2.obj: $(HEADERS)  ..\src\basemath\trans2.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fotrans2.obj ..\src\basemath\trans2.c
trans3.obj: $(HEADERS)  ..\src\basemath\trans3.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fotrans3.obj ..\src\basemath\trans3.c
elliptic.obj: $(HEADERS)  ..\src\modules\elliptic.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foelliptic.obj ..\src\modules\elliptic.c
galois.obj: $(HEADERS)  ..\src\modules\galois.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fogalois.obj ..\src\modules\galois.c
galconj.obj: $(HEADERS)  ..\src\basemath\galconj.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fogalconj.obj ..\src\basemath\galconj.c
subgroup.obj: $(HEADERS)  ..\src\basemath\subgroup.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fosubgroup.obj ..\src\basemath\subgroup.c
kummer.obj: $(HEADERS)  ..\src\modules\kummer.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fokummer.obj ..\src\modules\kummer.c
mpqs.obj: $(HEADERS)  ..\src\modules\mpqs.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fompqs.obj ..\src\modules\mpqs.c
nffactor.obj: $(HEADERS)  ..\src\modules\nffactor.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fonffactor.obj ..\src\modules\nffactor.c
stark.obj: $(HEADERS)  ..\src\modules\stark.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fostark.obj ..\src\modules\stark.c
subfield.obj: $(HEADERS)  ..\src\modules\subfield.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fosubfield.obj ..\src\modules\subfield.c
thue.obj: $(HEADERS)  ..\src\modules\thue.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fothue.obj ..\src\modules\thue.c
anal.obj: $(HEADERS) ..\src\language\anal.h ..\src\headers\parinf.h ..\src\language\anal.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foanal.obj ..\src\language\anal.c
compat.obj: $(HEADERS)  ..\src\language\compat.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Focompat.obj ..\src\language\compat.c
errmsg.obj: $(HEADERS)  ..\src\language\errmsg.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foerrmsg.obj ..\src\language\errmsg.c
es.obj: $(HEADERS) ..\src\language\anal.h ..\src\language\es.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foes.obj ..\src\language\es.c
helpmsg.obj: $(HEADERS)  ..\src\language\helpmsg.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Fohelpmsg.obj ..\src\language\helpmsg.c
init.obj: $(HEADERS) ..\src\language\anal.h ..\src\language\init.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS)  -Foinit.obj ..\src\language\init.c
sumiter.obj: $(HEADERS) ..\src\language\anal.h ..\src\language\sumiter.c
	$(CC) -c $(C0FLAGS) $(CPPFLAGS)  -Fosumiter.obj ..\src\language\sumiter.c
gp.obj: $(HEADERS) ..\src\language\anal.h ..\src\gp\gp.h .\paricfg.h ..\src\gp\gp.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -I..\src\language $(RLINCLUDE) -Fogp.obj ..\src\gp\gp.c
gp_init.obj: $(HEADERS) ..\src\graph\rect.h ..\src\gp\gp_init.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -I..\src\graph -Fogp_init.obj ..\src\gp\gp_init.c
gp_rl.obj: $(HEADERS) ..\src\language\anal.h ..\src\gp\gp.h .\paricfg.h ..\src\gp\gp_rl.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -I..\src\language $(RLINCLUDE) -Fogp_rl.obj ..\src\gp\gp_rl.c
highlvl.obj: $(HEADERS)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -Fohighlvl.obj ..\src\gp\highlvl.c
plot.obj: $(HEADERS) ..\src\graph\rect.h ..\src\graph\$(PLOTFILE)
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(PLOTCFLAGS) -Foplot.obj ..\src\graph\$(PLOTFILE)
plotport.obj: $(HEADERS) ..\src\graph\rect.h ..\src\graph\plotport.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -I..\src\graph -Foplotport.obj ..\src\graph\plotport.c
