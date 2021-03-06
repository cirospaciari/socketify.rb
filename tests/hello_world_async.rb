require "socketify"
require "time"
require "json"
# #Graceful Shutdown on SIGINT
$app = nil
Signal.trap("INT") { 
  $app.close()
  exit
}

main_thread = Thread.new {
    #if you dont want SSL just use $app = Socketify::App.new()
    $app = Socketify::SSLApp.new({
      key_file_name: "./misc/key.pem", #generated by ./test.sh
      cert_file_name: "./misc/cert.pem", #generated by ./test.sh
      passphrase: "1234"
    })
    home_handler = lambda do |response, request| 
      #We have to attach an abort handler for us to be aware
      #of disconnections while we perform async tasks
      aborted = false
      response.on_aborted(lambda do 
        #We don't implement any kind of cancellation here,
        #so simply flag us as aborted
        aborted = true
      end)
      #Simulate checking auth for 5 seconds. This looks like crap, never write
      #code that utilize Thread like this
      #Either way, here we go!
      Thread.new {
        sleep 5
        response.end("Hello, World!") unless aborted
      }
    end
    $app.get("/", home_handler)
    .listen(8082, lambda {|config| puts "Listening on port #{config.port}" })
    .run()
}

main_thread.join
