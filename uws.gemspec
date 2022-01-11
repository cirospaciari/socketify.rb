require 'date'
require File.join(File.dirname(__FILE__), 'lib/uws/version')

Gem::Specification.new do |s|
  s.name = 'uws'
  s.version = UWS::VERSION
  s.authors = 'Ciro Spaciari'
  s.date = Date.today.to_s
  s.email = 'ciro.spaciari@gmail.com'
  s.homepage = 'https://github.com/cirospaciari/uWebSockets.rb'
  s.summary = 'Fast WebSocket and Http/Https server'
  s.description = 'Fast WebSocket and Http/Https server using FFI bindings with C API from uNetworking/uWebSockets.'
  s.licenses = ['MIT']
  s.require_path = 'lib'
  s.files = Dir["{lib,ext}/**/*.{rb,h,c}"] + ['LICENSE', 'README.md']
  s.extensions = ["ext/uws/extconf.rb"]

  s.required_ruby_version = '>= 3.0'
  s.requirements << 'Linux or macOS'
end