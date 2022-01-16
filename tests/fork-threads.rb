require "uws"
require "time"
require "json"


def create_process

    Process.fork do 

        # Graceful Shutdown on SIGINT
        $app = nil
        Signal.trap("INT") { 
            $app.close()
            exit
        }
        $exited = false
        $current_httpdate = Time.now.httpdate
        $mutex = Mutex.new
        timer = Thread.new {
            loop do
                sleep(0.75)
                $mutex.synchronize do
                    current_time = Time.now
                    $current_httpdate = current_time.httpdate
                end
   
            end
        }
        main_thread = Thread.new {

            $app = UWS::App.new()

            plaintext_handler = lambda do |res| 
                $mutex.synchronize do
                    res.write_header("Date",  $current_httpdate)
                       .write_header("Server", "uws.rb")
                       .write_header("Content-Type","text/plain")
                       .end("Hello, World!")
                end
            end

            json_handler = lambda do |res| 
                $mutex.synchronize do
                    res.write_header("Date",  $current_httpdate)
                       .write_header("Server", "uws.rb")
                       .write_header("Content-Type","application/json")
                       .end({"message":"Hello, World!"}.to_json)
                end
            end

            $app.get("/", plaintext_handler)
                .get("/plaintext", plaintext_handler)
                .get("/json", json_handler)
                .listen(3000, lambda {|config| puts "Listening on port #{config.port}" })
                .run()
        }

        main_thread.join()
        timer.exit
    end

end


worker_count = 4
worker_count = ENV['WORKER_COUNT'].to_i if ENV.key?('WORKER_COUNT')

worker_count.times {
    create_process
}
Process.waitall
