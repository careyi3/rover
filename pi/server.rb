require 'pry'
require 'serialport'
require 'readline'

sem = Mutex.new

ser = SerialPort.new("/dev/ttyACM0", 115200, 8, 1, SerialPort::NONE)
# ser = SerialPort.new("/dev/tty.usbmodem1101", 115200, 8, 1, SerialPort::NONE)

data = nil

reader = Thread.new {
  line = nil
  while line == nil
    line = ser.readline
    sem.synchronize {
      puts "<- " + line
      if line[0] == 'D'
        data = line
      end
    }
    line = nil
  end
}

def handleData(sem, data)
  dist = 0
  sem.synchronize {
    if data != nil
      dist = data[1..-1].to_f
    end
  }
  dist
end

def runCommands(ser, command)
  puts "-> " + command
  ser.write("#{command}\n")
end

forward = false
turning = false
while true
  dist = handleData(sem, data)
  if dist <= 40
    unless turning
      turn_command = [true, false].sample ? 'L100' : 'R100'
      runCommands(ser, turn_command)
      forward = false
      turning = true
    end
  else
    unless forward
      runCommands(ser, 'F150')
      forward = true
      turning = false
    end
  end
  sleep(0.05)
end
