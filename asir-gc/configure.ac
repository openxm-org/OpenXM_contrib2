dnl $OpenXM$

AC_INIT(asir-gc,1.0)
AC_CONFIG_SRCDIR([LICENSE])
AC_PREREQ(2.69)
AC_CANONICAL_HOST

AC_PROG_INSTALL
AC_PROG_MKDIR_P

GC=gc-7.4.2
GC_CONFIGURE_ARGS="--prefix=${prefix} --disable-threads --enable-large-config"
if test -n "${host_alias}" ; then
    GC_CONFIGURE_ARGS="--host=${host_alias} ${GC_CONFIGURE_ARGS}"
fi
AC_ARG_WITH([distdir],
[  --with-distdir          specify the spool for fetching. [[no]]],
[with_distdir=${withval}],[with_distdir=no])
GC_DISTDIR='${prefix}/../OpenXM_dist'
if test ${with_distdir:=no} != no -a ${with_distdir} != yes ; then
    GC_DISTDIR=${with_distdir}
fi   

AC_SUBST([GC])
AC_SUBST([GC_CONFIGURE_ARGS])
AC_SUBST([GC_DISTDIR])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
