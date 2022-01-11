require "uws"

UWS::App.new()
.get("/", lambda {|response| response.end("Hello World uWS from Ruby!")})
.listen(8082, lambda {|config| puts "Listening on port #{config.port}" })
.run()