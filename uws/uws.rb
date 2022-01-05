require 'ffi'

module UWS

    class Uws_app_listen_config_t < FFI::Struct
        layout :port, :int,
        :host, :pointer,
        :options, :int
    end

end

module UWS::LibC
    extend FFI::Library
    ffi_lib FFI::Library::LIBC
    
    # memory allocators
    attach_function :malloc, [:size_t], :pointer
    attach_function :calloc, [:size_t], :pointer
    attach_function :valloc, [:size_t], :pointer
    attach_function :realloc, [:pointer, :size_t], :pointer
    attach_function :free, [:pointer], :void
    
    # memory movers
    attach_function :memcpy, [:pointer, :pointer, :size_t], :pointer
    attach_function :bcopy, [:pointer, :pointer, :size_t], :void
end # module LibC

module UWS::CAPI
  extend FFI::Library
  ffi_lib 'c'
  ffi_lib './uws/libuwebsockets.so'

  
  attach_function :uws_create_app, [], :pointer
  callback :uws_method_handler, [:pointer, :pointer], :void
  attach_function :uws_app_get, [ :pointer, :pointer, :uws_method_handler],  :void

  callback :uws_listen_handler, [:pointer, UWS::Uws_app_listen_config_t.by_value ], :void
  attach_function :uws_app_listen, [:pointer, :int, :uws_listen_handler ], :void
  attach_function :uws_app_listen_with_config, [:pointer, UWS::Uws_app_listen_config_t.by_value, :uws_listen_handler ], :void
  attach_function :uws_app_run, [:pointer], :void
  attach_function :uws_app_destroy, [:pointer], :void
  attach_function :uws_res_end, [:pointer, :pointer, :int], :void
  attach_function :uws_socket_close, [:pointer, :int], :void

  #requests
  attach_function :uws_req_is_ancient, [:pointer], :int
  attach_function :uws_req_get_yield, [:pointer], :int
  attach_function :uws_req_set_field, [:pointer, :int], :void
  attach_function :uws_req_get_url, [:pointer], :pointer
  attach_function :uws_req_get_method, [:pointer], :pointer
  attach_function :uws_req_get_header, [:pointer, :pointer], :pointer
  attach_function :uws_req_get_query, [:pointer, :pointer], :pointer
  attach_function :uws_req_get_parameter, [:pointer, :uint16], :pointer

end


class UWS::AppRequest
    attr_accessor :native_socket

    def initialize(request)
        @native_request = request
    end

    def is_ancient?()
        return UWS::CAPI.uws_req_is_ancient(@native_request) == 1
    end

    def get_yield?()
        return UWS::CAPI.uws_req_get_yield(@native_request) == 1
    end

    def set_yield(value)
        return UWS::CAPI.uws_req_set_field(@native_request, value ? 1 : 0)
    end

    def get_url()
        pointer = UWS::CAPI.uws_req_get_url(@native_request)
        value = pointer.get_string(0)
        UWS::LibC.free(pointer)
        return value
    end

    def get_method()
        pointer = UWS::CAPI.uws_req_get_method(@native_request)
        value = pointer.get_string(0)
        UWS::LibC.free(pointer)
        return value
    end


    def get_header(lower_case_name)
        native_value = FFI::MemoryPointer.from_string(lower_case_name)
        pointer = UWS::CAPI.uws_req_get_header(@native_request, native_value)
        value = pointer.get_string(0)
        UWS::LibC.free(pointer)
        return value
    end

    def get_query(key)
        native_value = FFI::MemoryPointer.from_string(key)
        pointer = UWS::CAPI.uws_req_get_query(@native_request, native_value)
        value = pointer.get_string(0)
        UWS::LibC.free(pointer)
        return value
    end

    def get_parameter(index)
        pointer = UWS::CAPI.uws_req_get_parameter(@native_request, index)
        value = pointer.get_string(0)
        UWS::LibC.free(pointer)
        return value
    end

end

class UWS::ListenSocket
    attr_accessor :native_socket
    attr_accessor :is_ssl

    def initialize(socket, is_ssl)
        @native_socket = native_socket
        @is_ssl = is_ssl
    end

    def close()
        UWS::CAPI.uws_socket_close(@native_socket, is_ssl ? 1 : 0)
    end
end

class UWS::AppListenConfig
    attr_accessor :native_config

    attr_accessor :port
    attr_accessor :host
    attr_accessor :options
    
    def initialize(config)
        @native_config = config
        @port = config[:port]
        @options = config[:options]
    end

end

class UWS::AppResponse
    attr_accessor :native_reponse

    def initialize(response)
        @native_response = response
    end

    def end(message)
        message = FFI::MemoryPointer.from_string(message)
        UWS::CAPI.uws_res_end(@native_response, message, 1)
    end

end

class UWS::App
    attr_accessor :native_app

   def initialize()
        @native_app = FFI::AutoPointer.new(UWS::CAPI.uws_create_app(), UWS::CAPI.method(:uws_app_destroy))
        @callbacks = {}
   end

   def get(pattern, callback)
        callback_key = "get:%s" % pattern
        native_pattern = FFI::MemoryPointer.from_string(pattern)

        #callback wrapper
        @callbacks[callback_key] = Proc.new do |response, request| 
            callback.call(UWS::AppResponse.new(response), UWS::AppRequest.new(request))
        end

        UWS::CAPI.uws_app_get(@native_app, native_pattern, @callbacks[callback_key])
        return self
   end


   def listen(port_or_config, callback)

        if(port_or_config.kind_of? Integer)
            @port = port_or_config  
            UWS::CAPI.uws_app_listen(@native_app, @port, lambda do | socket, config | 
                callback.call(UWS::ListenSocket.new(socket, false), UWS::AppListenConfig.new(config))
            end)
        else
            config = UWS::Uws_app_listen_config_t.new
            config[:host] = FFI::MemoryPointer.from_string(port_or_config[:host])
            config[:port] = port_or_config[:port]
            config[:options] = port_or_config[:options]

            UWS::CAPI.uws_app_listen_with_config(@native_app, config, lambda do | socket, config | 
                callback.call(UWS::ListenSocket.new(socket, false), UWS::AppListenConfig.new(config))
            end)
        end
        return self
   end

   def run()
        UWS::CAPI.uws_app_run(@native_app)
        return self
   end
end