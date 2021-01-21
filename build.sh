#!/bin/bash
rm -rf target/
mkdir target

cl65 -O -t c64 ./ultimate-dos-lib/src/lib/ultimate_lib.c u-time.c -o ./target/u-time64.prg
cl65 -O -t c128 ./ultimate-dos-lib/src/lib/ultimate_lib.c u-time.c -o ./target/u-time128.prg

rm -f *.o

cd target/

DISK_IMAGE=ntp2Ultimate.d64

c1541 -format "ntp2ultimate,sh" d64 $DISK_IMAGE \
-write u-time64.prg u-time64 \
-write u-time128.prg u-time128 

cd ..