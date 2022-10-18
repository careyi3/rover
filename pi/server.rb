require 'pry'
require 'serialport'
require 'readline'

sem = Mutex.new

ser = SerialPort.new("/dev/ttyACM0", 921600, 8, 1, SerialPort::NONE)
#ser = SerialPort.new("/dev/tty.usbmodem101", 921600, 8, 1, SerialPort::NONE)

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
dist_old = 0
dt = 0
dist = 0
while true
  t1 = Time.now
  dist_old = dist
  dist = handleData(sem, data)
  if dist <= 45
    unless turning
      turn_command = [true, false].sample ? 'L100' : 'R100'
      runCommands(ser, turn_command)
      forward = false
      turning = true
      sleep(0.5)
    end
  else
    #unless forward
      dist_offset = dist >= 40 ? dist - 40 : 0
      throttle = 60 + dist_offset
      runCommands(ser, "F#{throttle}")
      forward = true
      turning = false
    #end
  end
  sleep(0.01)
  t2 = Time.now
  dt = t2 - t1
end
