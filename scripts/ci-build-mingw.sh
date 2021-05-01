#!/bin/bash -e

[[ -z "$CC" || -z "$CXX" ]] && exit 255

variant=win32
[[ "$CXX" == "x86_64-"* ]] && variant=win64

libjpeg_version=2.0.6
libpng_version=1.6.37
zlib_version=1.2.11

mkdir -p libs
pushd libs
libs=$PWD
[ -e libjpeg.zip ] || \
	wget "http://minetest.kitsunemimi.pw/libjpeg-$libjpeg_version-$variant.zip" -O libjpeg.zip
[ -e libpng.zip ] || \
	wget "http://minetest.kitsunemimi.pw/libpng-$libpng_version-$variant.zip" -O libpng.zip
[ -e zlib.zip ] || \
	wget "http://minetest.kitsunemimi.pw/zlib-$zlib_version-$variant.zip" -O zlib.zip
[ -d libjpeg ] || unzip -o libjpeg.zip -d libjpeg
[ -d libpng ] || unzip -o libpng.zip -d libpng
[ -d zlib ] || unzip -o zlib.zip -d zlib
popd

cmake . \
	-DCMAKE_SYSTEM_NAME=Windows \
	-DPNG_LIBRARY=$libs/libpng/lib/libpng.dll.a \
	-DPNG_PNG_INCLUDE_DIR=$libs/libpng/include \
	-DJPEG_LIBRARY=$libs/libjpeg/lib/libjpeg.dll.a \
	-DJPEG_INCLUDE_DIR=$libs/libjpeg/include \
	-DZLIB_LIBRARY=$libs/zlib/lib/libz.dll.a \
	-DZLIB_INCLUDE_DIR=$libs/zlib/include

make VERBOSE=1 -j$(nproc)
exit 0
