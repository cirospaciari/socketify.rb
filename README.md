# Socketify.rb
Fast WebSocket and Http/Https server using an native extension with C API from [uNetworking/uWebSockets](https://github.com/uNetworking/uWebSockets)

> This project will adapt the full capi from uNetworking/uWebSockets but for now it's just this.

### Overly simple hello world app
```ruby
require "socketify"

Socketify::App.new()
.get("/", lambda {|response, request| response.end("Hello World socketify from Ruby!")})
.listen(8082, lambda {|config| puts "Listening on port #{config.port}" })
.run()
```

### Gemfile

```gem
gem 'socketify', git: 'https://github.com/cirospaciari/socketify.rb.git', branch: 'main', submodules: true
```

### Run
```bash
bundle exec ruby ./hello_world.rb
```

### SSL version sample
```ruby
require "socketify"

Socketify::SSLApp.new({
    key_file_name: "./misc/key.pem",
    cert_file_name: "./misc/cert.pem", 
    passphrase: "1234"
})
.get("/", lambda {|response, request| response.end("Hello World socketify from Ruby!")})
.listen(8082, lambda {|config| puts "Listening on port #{config.port}" })
.run()
```


## Build and Test Local
```bash
cd ./ext/socketify && ruby extconf.rb && make && cd ../../ && ruby -Ilib:ext -r socketify ./tests/hello_world.rb
```