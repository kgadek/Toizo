#!/bin/bash

echo

for i in tests/*.in; do
	[ ! -f "$i.desc" -o ! -f "$i.out" ] && continue
	echo "[$i] $(cat $i.desc)"
	[ $("$1" < "$i" | diff - "$i.out" | wc -l) -eq 0 ] && echo "    OK    " || echo "!! FAIL !!"
	echo
done
