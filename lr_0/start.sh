#!/bin/bash

filename=$(basename -s .c "$1")
if gcc $1 -o $filename.exe;
    then
        shift 1;
	./$filename.exe $@;
    else
	echo "An error occured during compilation. Check if the file name is correct" >&2;
fi
