#!/bin/bash
export CC=afl-clang-lto
export CXX=afl-clang-lto++
export LD=$CXX

args=(-DBUILD_EXAMPLES=ON -DENABLE_OPENGL=OFF -DBUILD_SHARED_LIBS=OFF)

unset AFL_USE_ASAN
grep -Fq '/afl-' build2/CMakeCache.txt || rm -rf build2
cmake -S . -B build2 "${args[@]}"
nice make -C build2 clean
nice make -C build2 -j10
for f in build2/bin/Linux/*; do
	ln -sv "../../$f" "bin/Linux/${f##*/}_noasan"
done

export AFL_USE_ASAN=1
grep -Fq '/afl-' CMakeCache.txt || rm -f CMakeCache.txt
cmake . "${args[@]}"
nice make clean
nice make -j10
