#!/bin/sh
# $OpenXM: OpenXM_contrib2/windows/extract_gc.sh,v 1.2 2009/03/11 11:32:07 ohara Exp $
(shopt -s igncr) 2>/dev/null && eval 'shopt -s igncr';#
OpenXM_HOME=${OpenXM_HOME:-../../OpenXM}
OpenXM_dist=${OpenXM_HOME}/../OpenXM_dist
GC=gc-7.1
GC_TARGZ=${GC}.tar.gz
GC_PATCH=gc-7.0-risa.diff
PARI_TARGZ=pari-2.0.17.beta.tgz

cat ${OpenXM_dist}/${PARI_TARGZ} | ( cd pari20; tar zxf -)
tar zxf ${OpenXM_dist}/${GC_TARGZ}

for i in ${GC_PATCH} ; do 
	f=../asir2000/$i
	if [ -f $f ]; then
		(cd ${GC}; patch -p1) < $f 
	fi
done
echo -n > ${GC}/gc_cpp.cc
