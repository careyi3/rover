require 'pry'
require 'serialport'

sem = Mutex.new

ser = SerialPort.new("/dev/ttyACM0", 9600, 8, 1, SerialPort::NONE)

data = []

reader = Thread.new {
  line = nil
  while line == nil
    line = ser.readline
    sem.synchronize {
      data << line
    }
    line = nil
  end
}

def handleData(sem, data)
  sem.synchronize {
    if data.length > 0
      puts data.shift
    end
  }
end

def runCommands(ser)
  ser.write("F123\n")
end

while true
  handleData(sem, data)
  runCommands(ser)
end
