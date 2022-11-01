require 'pry'
require 'serialport'
require 'readline'

sem = Mutex.new

ser = SerialPort.new("/dev/ttyACM0", 921600, 8, 1, SerialPort::NONE)
#ser = SerialPort.new("/dev/tty.usbmodem101", 921600, 8, 1, SerialPort::NONE)

yawRaw = nil
distanceRaw = nil

reader = Thread.new {
  line = nil
  while line == nil
    line = ser.readline
    sem.synchronize {
      puts "<- " + line
      if line[0] == 'D'
        distanceRaw = line
      end
      if line[0] == 'Y'
        yawRaw = line
      end
    }
    line = nil
  end
}

def handleData(sem, data)
  val = 0
  sem.synchronize {
    if data != nil
      val = data[1..-1].to_f
    end
  }
  val
end

def runCommands(ser, command)
  puts "-> #{command}"
  ser.write("#{command}\n")
end

forward = false
turning = false
yaw = 0.0
distance = 0.0
while true
  yaw = handleData(sem, yawRaw)
  distance = handleData(sem, distanceRaw)
  puts "<- yaw: #{yaw}, distance: #{distance}"
  if distance <= 45
    unless turning
      turn_command = [true, false].sample ? 'L100' : 'R100'
      runCommands(ser, turn_command)
      forward = false
      turning = true
      sleep(0.5)
    end
  else
    dist_offset = dist >= 40 ? dist - 40 : 0
    throttle = 60 + dist_offset
    runCommands(ser, "F#{throttle}")
    forward = true
    turning = false
  end
  sleep(0.01)
end
