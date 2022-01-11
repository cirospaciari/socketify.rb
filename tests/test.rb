require "uws"
require "time"
require "json"

# #Graceful Shutdown on SIGINT
$app = nil
Signal.trap("INT") { 
    $app.close()
  exit
}


$threads = []
# $exited = false
#Optimized current_httpdate each second
# $current_httpdate = Time.now.utc.httpdate
# $current_http_date_mutex = Mutex.new
# def timer()
#     while !$exited

#         $current_http_date_mutex.lock
#         $current_httpdate = Time.now.utc.httpdate
#         $current_http_date_mutex.unlock

#         sleep(1)
#     end
# end


# $threads << Thread.new { timer }

$threads << Thread.new {

    $app = UWS::App.new()

    $app.get("/", lambda do |res| 
        # $current_http_date_mutex.lock
        # res.write_header("Date", $current_httpdate)
        # $current_http_date_mutex.unlock
        
        # res.write_header("Server", "uws.rb")
        #    .write_header("Content-Type","text/plain")
        #    .end("Hello, World!")
        
        # res.write_header("Server", "uws.rb")
        #    .write_header("Content-Type","application/json")
        #    .end({"message":"Hello, World!"}.to_json)
        res.end("Hello, World!")
    end)
    .listen(8082, lambda {|config| puts "Listening on port #{config.port}" })
    .run()
    # $exited = true
}

$threads.each(&:join)
