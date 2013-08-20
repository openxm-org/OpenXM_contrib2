#!/bin/bash
# $OpenXM: OpenXM_contrib2/windows/extract_gc.sh,v 1.6 2012/05/30 02:24:33 ohara Exp $
(set -o igncr) 2>/dev/null && set -o igncr;#
OpenXM_HOME=${OpenXM_HOME:-../../OpenXM}
OpenXM_dist="$(realpath -m ${OpenXM_HOME}/../OpenXM_dist)"
GC=gc-7.2
GC_TARGZ=gc-7.2b.tar.gz
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
