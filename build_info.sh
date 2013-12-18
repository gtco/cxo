#!/bin/sh

###############################################

echo "#define BUILD_DATE \"`/bin/date '+%A %B %d %Y %T'`\"" > build_info.h
echo "#define BUILD_HOST \"`uname -n -m -o -r`\"" >> build_info.h
echo "#define BUILD_NAME \"Nago\"" >> build_info.h
nasm -v > temp.ver
echo "#define NASM_VER \"`/bin/cat temp.ver`\"" >> build_info.h
echo "1" >> build.counter
x=`/bin/cat build.counter |wc -l`
echo "`expr $x`" > temp.typo
echo "#define BUILD_VER \"`/bin/cat temp.typo`\"" >> build_info.h
echo "#define BUILD_VER_NUM `/bin/cat temp.typo`" >> build_info.h




