#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
make GRAPHICS=NO TERMINAL=NO
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT HUP INT TERM
cc=${CC:-cc}
flags='-DDATADIR=. -Isrc/brogue -Isrc/platform -Isrc/variants -std=c99'
$cc $flags -Dmain=brogue_cli_main -c src/platform/main.c -o "$work/main.o"
objects=$(find src -name '*.o' ! -path '*/Movement.o' ! -path '*/main.o')
$cc $flags test/entranced_web_test.c $objects "$work/main.o" -lm -o "$work/test"
"$work/test"
