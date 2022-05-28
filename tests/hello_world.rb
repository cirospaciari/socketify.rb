require "socketify"

Socketify::App.new()
.get("/", lambda {|response, request| response.end("Hello World socketify from Ruby!")})
.listen(8082, lambda {|config| puts "Listening on port #{config.port}" })
#or if you like to use full options
#.listen({ port: 8082, host: "0.0.0.0", options: 0 }, lambda {|config| puts "Listening on port #{config.port}" })
.run()