#!/bin/bash -u

PROJECT_DIR=$(dirname $0)/..
cd $PROJECT_DIR

rm -f /tmp/avrlaunch-*.txt
outfile=/tmp/avrlaunch-$RANDOM.txt
$PROJECT_DIR/bin/symlink-arduino.sh

for group in BUFFER SCHEDULER,SHELL EVENT ADC_EVENT,GPIO_EVENT; do
  make -s clean test program TEST=$group
  $PROJECT_DIR/bin/test-serial.rb > $outfile
  if [ $? != 0 ]; then
	  echo "Tests FAILED"
	  exit 1
  fi
	
  cat $outfile
  grep "FAIL" $outfile > /dev/null 2>&1

  if [ $? == 0 ]; then
	  echo "Tests FAILED"
	  exit 1
  else
    echo "Tests PASSED"
  fi
done
    	
echo ""
rm -f $outfile
