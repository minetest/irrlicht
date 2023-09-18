#!/bin/bash -e
args=(-DBUILD_EXAMPLES=ON -DENABLE_OPENGL=OFF -DBUILD_SHARED_LIBS=OFF)

export CC=afl-clang-fast
export CXX=afl-clang-fast++
export LD=$CXX
unset AFL_USE_ASAN
if ! grep -Fq '/afl-' build2/CMakeCache.txt; then
	rm -rf build2
	cmake -S . -B build2 "${args[@]}"
fi
nice make -C build2 clean
nice make -C build2 -j10
for f in build2/bin/Linux/*; do
	ln -snfv "../../$f" "bin/Linux/${f##*/}_noasan"
done

export CC=afl-clang-lto
export CXX=afl-clang-lto++
export LD=$CXX
export AFL_USE_ASAN=1
if ! grep -Fq '/afl-' CMakeCache.txt; then
	rm -f CMakeCache.txt
	cmake . "${args[@]}"
fi
nice make clean
nice make -j10
