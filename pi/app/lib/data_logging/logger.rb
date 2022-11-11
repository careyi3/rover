# frozen_string_literal: true

module DataLogging
  class Logger
    attr_reader(:path, :log_file)

    def initialize(path: './data')
      @path = path
      start_time = Time.now
      @log_file = "#{path}/log_#{start_time.to_i}.txt"
      write(data: start_time, mode: 'w')
    end

    def log_coords(yaw, distance)
      write(data: "#{yaw},#{distance}")
    end

    def log_orient_start
      write(data: 'O')
    end

    private

    def write(data:, mode: 'a')
      File.write(@log_file, "#{data}\n", mode: mode)
    end
  end
end
