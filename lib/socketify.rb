  
require "socketify/socketify"


class Socketify::ListenConfig

    attr_accessor :port
    attr_accessor :host
    attr_accessor :options
    
    def initialize(port, host, options)
        @port = port
        @host = host
        @options = options
    end
end