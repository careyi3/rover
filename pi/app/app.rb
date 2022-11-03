# frozen_string_literal: true

require('bundler')
Bundler.require
require_all('lib')

LOGGING = true

if ARGV[0].nil?
  Rover.new(logging: LOGGING).run
else
  Rover.new(port_name: ARGV[0], logging: LOGGING).run
end
