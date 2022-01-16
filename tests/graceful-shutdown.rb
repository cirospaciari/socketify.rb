require "uws"
require "time"
require "json"

# #Graceful Shutdown on SIGINT
$app = nil
Signal.trap("INT") { 
    $app.close()
  exit
}


main_thread = Thread.new {

    $app = UWS::App.new()

    $app.get("/", lambda {|response| response.end("Hello, World!")})
    .listen(8082, lambda {|config| puts "Listening on port #{config.port}" })
    .run()
}

main_thread.join
