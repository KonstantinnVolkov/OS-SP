#!/bin/bash

filename=$(basename -s .c "$1")
if gcc $1 -o $filename.exe
    then
        shift 1;
        ./$filename.exe $@;
fi
