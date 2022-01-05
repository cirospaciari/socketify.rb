require_relative "uws/uws"

UWS::App.new()
.get("/", lambda {|response, request| response.end("Hello World uWS from Ruby!")})
.listen(8082, lambda {|socket, config| puts "Listening on port #{config.port}" })
.run()

# or you can pass full options in .listen
#.listen({ "port": 8082, "host": "127.0.0.1", "options": 0 }, lambda {|socket, config| puts "Listening on port #{config.port}" })