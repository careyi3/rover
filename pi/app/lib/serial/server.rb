# frozen_string_literal: true

module Serial
  class Server
    attr_reader(:port, :logging, :running, :data, :sem)

    DATA_TYPES = {
      'D' => :distance,
      'Y' => :yaw
    }.freeze

    def initialize(port_name: '/dev/ttyACM0', baud_rate: 921600, logging: false)
      @port = SerialPort.new(port_name, baud_rate, 8, 1, SerialPort::NONE)
      @logging = logging
      @running = false
      @data = {
        distance: 0.0,
        yaw: 0.0
      }
      @sem = Mutex.new
    end

    def start
      if @running
        puts 'Server already running' if @logging
        return @running
      end
      @running = start_reader
      @running
    end

    def write(command)
      unless running
        puts 'Server not running' if @logging
        return false
      end
      @port.write("#{command}\n")
      puts "-> #{command}" if @logging
      true
    end

    def fetch(data_type)
      unless running
        puts 'Server not running' if @logging
        return 0.0
      end
      data = 0.0
      @sem.synchronize do
        data = @data[data_type]
      end
      data
    end

    private

    def start_reader
      reader =
        Thread.new do
          read_loop
        end
      reader.alive?
    end

    def read_loop
      loop do
        line = @port.readline
        handle_data(line) if data?(line[0])
        log(line) if @logging
      end
    end

    def data?(id)
      !DATA_TYPES[id].nil?
    end

    def handle_data(line)
      @sem.synchronize do
        @data[DATA_TYPES[line[0]]] = line[1..].to_f
      end
    end

    def log(line)
      puts "<- #{line}"
    end
  end
end
