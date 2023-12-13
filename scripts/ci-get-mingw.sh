#!/bin/bash
set -e
topdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
[ -z "$1" ] && exit 255

ver=13.2.0
os=ubuntu22.04
name="mingw-w64-${1}_${ver}_${os}.tar.xz"
wget "http://minetest.kitsunemimi.pw/$name" -O "$name"
sha256sum -w -c <(grep -F "$name" "$topdir/sha256sums.txt")
sudo tar -xaf "$name" -C /usr
rm -f "$name"
