#!/usr/bin/env ruby

require 'serialport'

PROJECT_DIR = File.dirname(__FILE__) + "/.."

serial = SerialPort.open("#{PROJECT_DIR}/tty.mcu", 1200)
serial.dtr=1
serial.dtr=0
serial.close

sleep 2.5
