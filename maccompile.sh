#!/bin/bash

export QMAKE="/Volumes/SSD/Qt/5.8/clang_64/bin/qmake"
#export QMAKE="qmake"
#export MAKE="make"
export MAKE="make -j 8"
#export TARGET="debug"
#export TARGET="release"
export CLEAN="make clean"
#export SPEC="-spec unsupported/macx-clang"

cd libcds-v1.0.12/qmake
rm -Rf Makefile* object*
${QMAKE} $SPEC libcds.pro
#$CLEAN
$MAKE $TARGET || exit 0
cd ../..

cd hdt-lib/qmake
rm -Rf Makefile* object*
${QMAKE} $SPEC hdt-lib.pro
$CLEAN
$MAKE $TARGET || exit 0
cd ../..

cd hdt-it
rm -Rf Makefile* object*
${QMAKE} $SPEC hdt-it.pro
$CLEAN
$MAKE $TARGET 
cd ..
