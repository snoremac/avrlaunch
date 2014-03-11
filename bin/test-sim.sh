#!/bin/bash -eu

PROJECT_DIR=$(dirname $0)/..
cd $PROJECT_DIR

make clean
rm -f /tmp/avrlaunch-*.txt

outfile=/tmp/avrlaunch-$RANDOM.txt
make test-sim BOARD=SIMAVR > $outfile
	
cat $outfile
grep "FAIL" $outfile > /dev/null 2>&1

if [ $? == 0 ]; then
	echo "Tests FAILED"
	exit 1
else
	echo "Tests PASSED"
fi
	
echo ""
rm -f $outfile
