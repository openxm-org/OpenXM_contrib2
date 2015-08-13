#!/bin/bash
# $OpenXM: OpenXM_contrib2/windows/extract_gc.sh,v 1.7 2013/08/20 14:17:22 ohara Exp $
(set -o igncr) 2>/dev/null && set -o igncr;#
OpenXM_HOME=${OpenXM_HOME:-../../OpenXM}
OpenXM_dist="$(realpath -m ${OpenXM_HOME}/../OpenXM_dist)"
GC=gc-7.4.2
ATOMIC_OPS=libatomic_ops-7.4.0
GC_TARGZ=gc-7.4.2.tar.gz
GC_PATCH=gc-7.0-risa.diff

tar zxf ${OpenXM_dist}/${GC_TARGZ}
tar zxf ${OpenXM_dist}/${ATOMIC_OPS}.tar.gz
mv ${GC} gc
mv ${ATOMIC_OPS} gc/libatomic_ops

for i in ${GC_PATCH} ; do 
	f=../asir2000/$i
	if [ -f $f ]; then
		(cd gc; patch -p1) < $f 
	fi
done
echo -n > gc/gc_cpp.cc
