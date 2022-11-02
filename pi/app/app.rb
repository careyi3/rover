# frozen_string_literal: true

require('bundler')
Bundler.require
require_all('lib')

Rover.new.run
