# frozen_string_literal: true

require('bundler')
Bundler.require
require_all('lib')

if ARGV[0].nil?
  Rover.new(logging: true).run
else
  Rover.new(port_name: ARGV[0], logging: true).run
end
