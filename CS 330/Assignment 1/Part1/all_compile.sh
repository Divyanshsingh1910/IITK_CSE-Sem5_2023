#!/bin/bash

# Cleanup old executable 
[ -f sqroot ] && rm sqroot
[ -f double ] && rm double
[ -f square ] && rm square

# Compile
gcc -o sqroot sqroot.c -lm
gcc -o double double.c -lm
gcc -o square square.c -lm