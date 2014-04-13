#!/usr/bin/env ruby

require 'serialport'

PROJECT_DIR = File.dirname(__FILE__) + "/.."

# First connection will force reset when DTR is asserted
SerialPort.new("#{PROJECT_DIR}/tty.mcu", 230400, 8, 1, SerialPort::NONE)
sleep 2

serial = SerialPort.new("#{PROJECT_DIR}/tty.mcu", 230400, 8, 1, SerialPort::NONE)
serial.write("test\n")

eof = false
while not eof
  line = serial.readline("\r\n").chomp
  puts line
  eof = ["OK", "FAIL"].member?(line)
end
serial.close
