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
    left_angle = @yaw - 45
    right_angle = @yaw + 45

    coord = {}

    sleep(0.5)
    left(50)
    while @yaw > left_angle
      fetch_sensor_data
      coord[@distance] = @yaw
      sleep(0.01)
    end

    sleep(0.5)
    right(50)
    while @yaw < right_angle
      fetch_sensor_data
      coord[@distance] = @yaw
      sleep(0.01)
    end

    sleep(0.5)
    target = coord[coord.keys.max]

    if target > @yaw
      right(50)
      while @yaw < target
        fetch_sensor_data
        sleep(0.01)
      end
    else
      left(50)
      while @yaw > target
        fetch_sensor_data
        sleep(0.01)
      end
    end
  end
end
