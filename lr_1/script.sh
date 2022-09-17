#!/bin/bash

ERR=$"/tmp/errFile"
file=$(basename $0)
pids=$(pgrep -U $1) 2>> $ERR
if [ $? -eq 0 ] 	
then
    for pid in $pids 
    do	
        echo "$pid $(ls /proc/$pid/fd/ | wc -l)"
    done
else
    echo "$file: $(cat $ERR)" >&2;
fi
