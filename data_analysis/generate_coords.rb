require 'csv'
require 'json'
require 'pry'

def polar_to_cart(angle, length)
  x = length * Math.cos((angle)*Math::PI/180)
  y = length * Math.sin((angle)*Math::PI/180)
  return x, y
end

coords = [{x: 0.0, y: 0.0, t: 'o'}]
origin_x, origin_y = 0.0, 0.0
last_x, last_y = 0.0, 0.0
last_yaw, last_dist = 0.0, 0.0

line_num = 0
compute_origin = false
File.readlines('./data/in/test_data.txt').each do |line|
  unless line_num > 0
    line_num += 1
    next
  end
  if line.tr("\n", '') == "O"
    next if line_num < 3
   
    compute_origin = true
  else
    yaw, dist = line.tr("\n", '').split(',').map(&:to_f)
    if compute_origin
      length = last_dist - dist
      angle = yaw + 30
      origin_x, origin_y = polar_to_cart(angle, length)
      compute_origin = false
      coords << {x: origin_x, y: origin_y, t: 'o'}
    end
    outlier = dist < 30 || dist > 80
    last_yaw = yaw
    last_dist = dist

    x, y = polar_to_cart(yaw, dist)
    x = x + origin_x
    y = y + origin_y
    last_x = x
    last_y = y
    
    coords << {x: x, y: y, t: 'p'} unless outlier
  end
  line_num += 1
end

CSV.open('./data/out/data.csv', 'wb') do |csv|
  csv << ['x', 'y', 't']
  coords.each do |coord|
    csv << coord.values
  end
end

File.write('./simulation/data.json', coords.to_json)
