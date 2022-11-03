# frozen_string_literal: true

class Rover
  attr_reader(:server, :yaw, :distance)

  def initialize(port_name: '/dev/ttyACM0', logging: true)
    @yaw = 0.0
    @distance = 0.0
    @server = Serial::Server.new(port_name: port_name, logging: logging)
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
      sleep(0.01)
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
    drive_command(command: 'F', value: speed)
  end

  def reverse(speed)
    drive_command(command: 'B', value: speed)
  end

  def left(speed)
    drive_command(command: 'L', value: speed)
  end

  def right(speed)
    drive_command(command: 'R', value: speed)
  end

  def stop
    drive_command(command: 'S')
  end

  def drive_command(command:, value: nil)
    @server.write("#{command}#{value}")
  end

  def orient
    coord = {}

    target = @yaw + 350
    target -= 360 if target > 360

    sleep(0.5)
    left(75)
    while abs(@yaw - target) > 2
      fetch_sensor_data
      coord[@distance] = @yaw
      sleep(0.01)
    end

    target = coord[coord.keys.max]

    sleep(0.5)
    right(75)
    while abs(@yaw - target) > 2
      fetch_sensor_data
      sleep(0.01)
    end

    stop
    sleep(0.5)
  end
end
