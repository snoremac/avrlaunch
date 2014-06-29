#!/usr/bin/env ruby

require 'serialport'

PROJECT_DIR = File.dirname(__FILE__) + "/.."

# First connection will force reset when DTR is asserted
serial = SerialPort.new("#{PROJECT_DIR}/tty.mcu", 9600, 8, 1, SerialPort::NONE)
sleep 2

serial = SerialPort.new("#{PROJECT_DIR}/tty.mcu", 9600, 8, 1, SerialPort::NONE)
serial.read_timeout = 1000
serial.flush_input
serial.flush_output
serial.write("test\n")

eof = false
while not eof
  line = serial.readline("\\n").chomp
  puts line
  if line =~ /OK/ or line =~ /FAIL/
    eof = true
  end
end

serial.close
