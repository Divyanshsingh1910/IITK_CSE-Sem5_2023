#!/bin/bash

# First do make.
make

# Go to test directory.
cd Testcases

# Now ecxecute all test cases with 5m at most timer.
echo test1
timeout 1m ./test1
echo test2
timeout 1m ./test2
echo test3
timeout 1m ./test3
echo test4
timeout 1m ./test4
echo test5
timeout 1m ./test5
echo test6
timeout 1m ./test6
echo test7
timeout 1m ./test7
echo test8
timeout 1m ./test8
echo test9
timeout 1m ./test9
echo test10
timeout 1m ./test10
echo test11
timeout 1m ./test11
echo test12
timeout 1m ./test12
echo test13
timeout 1m ./test13
echo test14
timeout 1m ./test14
echo test15
timeout 1m ./test15

# Finally clean everything.
cd ../
make clean
