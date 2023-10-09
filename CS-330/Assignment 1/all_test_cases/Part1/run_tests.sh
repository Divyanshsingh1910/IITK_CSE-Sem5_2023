#!/bin/bash

test () {

    # Execute the given ops pattern.
    RESULT=`$2`

    # If produced result and expected result is equal then test is passed.
    if [ "$RESULT" == "$3" ] 
    then
        echo "TEST $1 PASSED"
    else
        echo "TEST $1 FAILED"
    fi

}


# Cleanup old executable 
[ -f sqroot ] && rm sqroot
[ -f double ] && rm double
[ -f square ] && rm square

# Compile
gcc -o sqroot sqroot.c -lm
gcc -o double double.c -lm
gcc -o square square.c -lm

test 1 "./sqroot 5" 2
test 2 "./double square 2" 16
test 3 "./sqroot square 4" 4
test 4 "./double double double double 1" 16
test 5 "./square square square square square 1" 1
test 6 "./sqroot sqroot sqroot sqroot sqroot sqroot sqroot sqroot sqroot 1" 1
test 7 "./sqroot square sqroot square sqroot square sqroot square sqroot square 1" 1
test 8 "./square double sqroot double sqroot square 3" 9
test 9 "./double sqroot double sqroot double sqroot double sqroot 5" 2
test 10 "./double sqroot double sqroot sqroot sqroot square sqroot square sqroot 5" 1
