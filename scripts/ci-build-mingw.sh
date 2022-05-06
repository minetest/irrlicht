#!/bin/bash -e

[[ -z "$CC" || -z "$CXX" ]] && exit 255

variant=win32
[[ "$(basename "$CXX")" == "x86_64-"* ]] && variant=win64

libjpeg_version=2.1.2
libpng_version=1.6.37
zlib_version=1.2.11

mkdir -p libs
pushd libs
libs=$PWD
tmp=
[ "$variant" = win32 ] && tmp=dw2/
[ -e libjpeg.zip ] || \
	wget "http://minetest.kitsunemimi.pw/libjpeg-$libjpeg_version-$variant.zip" -O libjpeg.zip
[ -e libpng.zip ] || \
	wget "http://minetest.kitsunemimi.pw/${tmp}libpng-$libpng_version-$variant.zip" -O libpng.zip
[ -e zlib.zip ] || \
	wget "http://minetest.kitsunemimi.pw/${tmp}zlib-$zlib_version-$variant.zip" -O zlib.zip
[ -d libjpeg ] || unzip -o libjpeg.zip -d libjpeg
[ -d libpng ] || unzip -o libpng.zip -d libpng
[ -d zlib ] || unzip -o zlib.zip -d zlib
popd

tmp=(
	-DCMAKE_SYSTEM_NAME=Windows \
	-DPNG_LIBRARY=$libs/libpng/lib/libpng.dll.a \
	-DPNG_PNG_INCLUDE_DIR=$libs/libpng/include \
	-DJPEG_LIBRARY=$libs/libjpeg/lib/libjpeg.dll.a \
	-DJPEG_INCLUDE_DIR=$libs/libjpeg/include \
	-DZLIB_LIBRARY=$libs/zlib/lib/libz.dll.a \
	-DZLIB_INCLUDE_DIR=$libs/zlib/include
)

cmake . "${tmp[@]}"
make -j$(nproc)

if [ "$1" = "package" ]; then
	make DESTDIR=$PWD/_install install
	# strip library
	"${CXX%-*}-strip" --strip-unneeded _install/usr/local/lib/*.dll
	# bundle the DLLs that are specific to Irrlicht (kind of a hack)
	cp -p $libs/*/bin/lib{jpeg,png}*.dll _install/usr/local/lib/
	# create a ZIP
	(cd _install/usr/local; zip -9r "$OLDPWD"/irrlicht-$variant.zip -- *)
fi
exit 0
