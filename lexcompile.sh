#!/bin/bash

if [[ -z "$1" ]]; then
    echo "Usage: ./lexcompile.sh file_name.l(ex)"
    exit 1
fi

FNAME="${1}.out"

lex $1
gcc -c -o $FNAME lex.yy.c && rm lex.yy.c &&\
gcc $FNAME -ll -o $FNAME
