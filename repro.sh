#!/bin/bash
aflout=out/default
myout=out_rep
exe=./bin/Linux/LoadTexture
rm -rf "$myout" && mkdir -p "$myout"
find $aflout/crashes -name 'id:*' -print | \
while read file; do
	echo "➤ Testing $file..."
	short=${file##*/}
	short=${short%%,*}
	fout=$myout/$short.txt
	$exe "$file" >"$fout" 2>&1
	r=$?
	if grep -Fq '==ERROR: AddressSanitizer: SEGV on unknown address' "$fout"; then
		gdb -q --batch -iex 'set confirm off' -ex r -ex bt --args \
			${exe}_noasan "$file" >"$fout" 2>&1
		if grep -q '^Thread.*received signal'; then
			cp "$file" "$myout/$short.bin"
			echo "✔ Reproduced (gdb)"
		else
			echo "✗ Not reproduced (gdb)"
			rm -f "$fout"
		fi
		continue
	fi
	grep -Fq '==ABORTING' "$fout" && r=1
	if [ $r -eq 0 ]; then
		echo "✗ Not reproduced (asan)"
		rm -f "$fout"
		continue
	fi
	cp "$file" "$myout/$short.bin"
	echo "✔ Reproduced (asan)"
done
