# uWebSockets.rb
Fast WebSocket and Http/Https server using FFI bindings with C API from [uNetworking/uWebSockets](https://github.com/uNetworking/uWebSockets)

> This project will adapt the full capi from uNetworking/uWebSockets but for now it's just this.

## Overly simple hello world app
```ruby
require_relative "uws/uws"

UWS::App.new()
.get("/", lambda {|response, request| response.end("Hello World uWS from Ruby!")})
.listen(8082, lambda {|socket, config| puts "Listening on port %d" % [config.port] })
.run()
```
