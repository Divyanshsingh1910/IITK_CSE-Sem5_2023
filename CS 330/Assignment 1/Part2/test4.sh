#!/bin/bash

actual_size=0
calculated_size=0

find_actual_size()
{
	output=$(du -bsL $1)
	actual_size=$(echo $output | cut -d ' ' -f 1)
}

find_calculated_size()
{
	calculated_size=$(./myDU $1)
}

if [ ! -f myDU.c ]
then
	echo "myDU.c doesn't exist"
	exit -1
fi

gcc -o myDU myDU.c
if [ $? -ne "0" ]
then
	echo "myDU.c: compilation failed"
	exit -1
fi

#Testcase 4
find_actual_size Testcase4/Root
find_calculated_size Testcase4/Root
echo "Expected output: $actual_size"
echo "Your output: $calculated_size"

if [ $actual_size != "$calculated_size" ]
then
	echo "Testcase 4 failed"
else
	echo "Testcase 4 passed"
fi
