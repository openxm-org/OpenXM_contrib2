#!/bin/sh
OpenXM_HOME=${OpenXM_HOME:-../../OpenXM}
OpenXM_dist=${OpenXM_HOME}/../OpenXM_dist
GC=gc-7.1
GC_TARGZ=${GC}.tar.gz
GC_PATCH=gc-7.0-risa.diff

tar zxf ${OpenXM_dist}/${GC_TARGZ}

for i in ${GC_PATCH} ; do 
	f=../asir2000/$i
	if [ -f $f ]; then
		(cd ${GC}; patch -p1) < $f 
	fi
done
echo -n > ${GC}/gc_cpp.cc
