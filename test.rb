require_relative "uws/uws"

app = UWS::App.new()

home_request = lambda do |response, request| 
    response.end("Hello world!")
end
app.get("/", home_request)
# app.get("/", lambda {|response, request| response.end("Hello World uWS from Ruby!")})
# app.listen(8082, lambda {|socket, config| puts "Listening on port %d" % [config.port] })
app.listen({ "port": 8082, "host": "127.0.0.1", "options": 0 }, lambda {|socket, config| puts "Listening on port http://#{config.host}:#{config.port}" })
app.run()