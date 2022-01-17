# uWebSockets.rb
Fast WebSocket and Http/Https server using an native extension with C API from [uNetworking/uWebSockets](https://github.com/uNetworking/uWebSockets)

> This project will adapt the full capi from uNetworking/uWebSockets but for now it's just this.

### Overly simple hello world app
```ruby
require "uws"

UWS::App.new()
.get("/", lambda {|response| response.end("Hello World uWS from Ruby!")})
.listen(8082, lambda {|config| puts "Listening on port #{config.port}" })
.run()
```

### Gemfile

```gem
gem 'uws', git: 'https://github.com/cirospaciari/uWebSockets.rb.git', branch: 'main', submodules: true
```

### Run
```bash
bundle exec ruby ./hello_world.rb
```

### SSL version sample
```ruby
require "uws"

UWS::SSLApp.new({
    key_file_name: "./misc/key.pem",
    cert_file_name: "./misc/cert.pem", 
    passphrase: "1234"
})
.get("/", lambda {|response| response.end("Hello World uWS from Ruby!")})
.listen(8082, lambda {|config| puts "Listening on port #{config.port}" })
.run()
```