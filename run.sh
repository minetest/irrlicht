#!/bin/bash
export AFL_SKIP_CPUFREQ=1
export AFL_TMPDIR=/dev/shm
opts=(
	-t 100
	-i sample_u/
	-o out/
)
exe=./bin/Linux/LoadTexture
if ! [ -d sample_u ]; then
	mkdir sample_u
	afl-cmin.bash -i sample/ -o sample_u/ -- $exe @@
	echo; echo
fi
exec afl-fuzz \
	"${opts[@]}" "$@" \
	-- $exe @@
