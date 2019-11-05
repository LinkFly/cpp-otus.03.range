#!/bin/bash
cd "$(dirname "$0")"

res_file=results-of-tests.txt
count=5;
test_program=./Debug/tests

cd build
cmake ..
cmake --build .
if test "$?" == "0"
then
	echo "BUILD OK"
else
	echo "BUILD FAIL"
	exit 1
fi

res_file="./$res_file"
echo "============ TESTS (test program: $test_program) ============" |tee > $res_file
echo "---------- Config ----------"|tee >> $res_file
cat ./config.h|tee >> $res_file
echo "---------- end Config ----------"|tee >> $res_file

echo "Running $count tests ..."|tee >> $res_file
for (( i=1; i <= $count; i++ ))
do
	echo "---------------------------"|tee >> $res_file
	echo "----- Running test N $i"|tee >> $res_file
	$test_program|tee >> $res_file
done
cat $res_file

