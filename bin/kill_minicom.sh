#!/bin/bash

ps -ef |egrep "\d+\s+minicom" > /dev/null 2>&1
if [ "$?" == "0" ]; then
  minicom_pid=$(ps -ef |egrep "\d+\s+minicom" |awk '{ print $2 }')
  kill $minicom_pid
  killed=0
  while [ "$killed" == "0" ]; do
    ps -ef |egrep "\d+\s+minicom" > /dev/null 2>&1
    killed=$?
  done
fi
