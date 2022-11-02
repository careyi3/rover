# frozen_string_literal: true

class Rover
  attr_reader(:server, :yaw, :distance)

  def initialize
    @yaw = 0.0
    @distance = 0.0
    @server = Serial::Server.new(logging: true)
    @server.start
  end

  def run
    loop do
      fetch_sensor_data
      if @distance > 45
        drive
      else
        orient
      end
    end
  end

  private

  def fetch_sensor_data
    @distance = @server.fetch(:distance)
    @yaw = @server.fetch(:yaw)
  end

  def drive
    dist_offset = @distance >= 40 ? @distance - 40 : 0
    throttle = 60 + dist_offset
    forward(throttle)
  end

  def forward(speed)
    drive_command('F', speed)
  end

  def reverse(speed)
    drive_command('B', speed)
  end

  def left(speed)
    drive_command('L', speed)
  end

  def right(speed)
    drive_command('R', speed)
  end

  def stop
    drive_command('S')
  end

  def drive_command(command:, value: nil)
    @server.write("#{command}#{value}")
  end

  def orient
    if [true, false].sample
      left(100)
    else
      right(100)
    end
    sleep(0.5)
  end
end
