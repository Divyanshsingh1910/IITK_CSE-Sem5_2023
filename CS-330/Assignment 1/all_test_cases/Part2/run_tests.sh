#!/bin/bash

actual_size=0
calculated_size=0

find_actual_size()
{
	output=$(du -bslL $1)
	actual_size=$(echo $output | cut -d ' ' -f 1)
}

find_calculated_size()
{
	calculated_size=$(timeout 5m strace ./myDU "$1" 2> temp) 
	num_fork=$(cat temp | grep clone | wc -l)
	#echo $num_fork
	

	if [ "$2" == "1" ]
	then
		if [ "$num_fork" != "4" ]
		then
			calculated_size=0
		fi

	elif [ "$2" == "2" ]
	then
		if [ "$num_fork" != "13" ]
		then
			calculated_size=0
		fi
	
	elif [ "$2" == "3" ]
	then
		if [ "$num_fork" != "0" ]
		then
			calculated_size=0
		fi
	
	elif [ "$2" == "4" ]
	then
		if [ "$num_fork" != "0" ]
		then
			calculated_size=0
		fi
	
	elif [ "$2" == "5" ]
	then
		if [ "$num_fork" != "5" ]
		then
			calculated_size=0
		fi
	
	elif [ "$2" == "6" ]
	then
		if [ "$num_fork" != "20" ]
		then
			calculated_size=0
		fi
	
	elif [ "$2" == "7" ]
	then
		if [ "$num_fork" != "1" ]
		then
			calculated_size=0
		fi
	
	elif [ "$2" == "8" ]
	then
		if [ "$num_fork" != "4" ]
		then
			calculated_size=0
		fi
	
	elif [ "$2" == "9" ]
	then
		if [ "$num_fork" != "1" ]
		then
			calculated_size=0
		fi
	
	elif [ "$2" == "10" ]
	then
		if [ "$num_fork" != "1" ]
		then
			calculated_size=0
		fi
	
	elif [ "$2" == "11" ]
	then
		if [ "$num_fork" != "0" ]
		then
			calculated_size=0
		fi
	
	elif [ "$2" == "12" ]
	then
		if [ "$num_fork" != "3" ]
		then
			calculated_size=0
		fi
	
	else
		echo "Incorrect test case number"
		exit -1
	fi

}

print_result()
{
#	echo "Expected output: $2"
#	echo "Your output: $3" 
	if [ "$2" != "$3" ]
	then
		echo "TESTCASE $1 FAILED"
	else
		echo "TESTCASE $1 PASSED"
	fi
	

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

#Testcase 1
find_actual_size Testcase1/Root
find_calculated_size Testcase1/Root 1
print_result 1 $actual_size $calculated_size

#Testcase 2
find_actual_size Testcase2/Root
find_calculated_size Testcase2/Root 2
print_result 2 $actual_size $calculated_size

#Testcase 3
find_actual_size Testcase3/Root
find_calculated_size Testcase3/Root 3
print_result 3 $actual_size $calculated_size

#Testcase 4
find_actual_size Testcase4/Root
find_calculated_size Testcase4/Root 4
print_result 4 $actual_size $calculated_size

#Testcase 5
find_actual_size Testcase5/Root
find_calculated_size Testcase5/Root 5
print_result 5 $actual_size $calculated_size

#Testcase 6
find_actual_size Testcase6/Root
find_calculated_size Testcase6/Root 6
print_result 6 $actual_size $calculated_size

#Testcase 7
find_actual_size Testcase7/Root
find_calculated_size Testcase7/Root 7
print_result 7 $actual_size $calculated_size

#Testcase 8
find_actual_size Testcase8/Root
find_calculated_size Testcase8/Root 8
print_result 8 $actual_size $calculated_size

#Testcase 9
actual_size="Unable to execute"
find_calculated_size Testcase9/Root 9
print_result 9 "$actual_size" "$calculated_size"

#Testcase 10
find_actual_size Testcase10/Root
find_calculated_size Testcase10/Root 10
print_result 10 $actual_size $calculated_size

#Testcase 11
find_actual_size Testcase11/Root
find_calculated_size Testcase11/Root 11
print_result 11 $actual_size $calculated_size

#Testcase 12
find_actual_size Testcase12/Root
find_calculated_size Testcase12/Root 12
print_result 12 $actual_size $calculated_size

echo "Note: 'Your output: 0' indicates your ./myDU utility didn't fork the correct number of child processes"
echo ""
