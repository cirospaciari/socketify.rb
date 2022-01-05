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


  #http
  callback :uws_method_handler, [:pointer, :pointer], :void
  callback :uws_listen_handler, [:pointer, UWS::Uws_app_listen_config_t.by_value ], :void
  
  attach_function :uws_create_app, [], :pointer
  attach_function :uws_app_run, [:pointer], :void
  attach_function :uws_app_destroy, [:pointer], :void
  attach_function :uws_socket_close, [:pointer, :int], :void
  attach_function :uws_app_listen, [:pointer, :int, :uws_listen_handler ], :void
  attach_function :uws_app_listen_with_config, [:pointer, UWS::Uws_app_listen_config_t.by_value, :uws_listen_handler ], :void
  
  attach_function :uws_app_get, [ :pointer, :pointer, :uws_method_handler],  :void
  attach_function :uws_app_post, [ :pointer, :pointer, :uws_method_handler],  :void
  attach_function :uws_app_options, [ :pointer, :pointer, :uws_method_handler],  :void
  attach_function :uws_app_delete, [ :pointer, :pointer, :uws_method_handler],  :void
  attach_function :uws_app_patch, [ :pointer, :pointer, :uws_method_handler],  :void
  attach_function :uws_app_put, [ :pointer, :pointer, :uws_method_handler],  :void
  attach_function :uws_app_head, [ :pointer, :pointer, :uws_method_handler],  :void
  attach_function :uws_app_connect, [ :pointer, :pointer, :uws_method_handler],  :void
  attach_function :uws_app_trace, [ :pointer, :pointer, :uws_method_handler],  :void
  attach_function :uws_app_any, [ :pointer, :pointer, :uws_method_handler],  :void

  attach_function :uws_constructor_failed, [:pointer], :int
  attach_function :uws_num_subscribers, [:pointer, :pointer], :uint32
  attach_function :uws_publish, [:pointer, :pointer, :pointer, :int, :int], :int
  attach_function :uws_get_native_handle, [:pointer], :pointer
  attach_function :uws_remove_server_name, [:pointer, :pointer], :void
  attach_function :uws_add_server_name, [:pointer, :pointer], :void


#   bool uws_constructor_failed(uws_app_t *app);
#   unsigned int uws_num_subscribers(uws_app_t *app, const char *topic);
#   bool uws_publish(uws_app_t *app, const char *topic, const char *message, uws_opcode_t opcode, bool compress);
#   void *uws_get_native_handle(uws_app_t *app);
#   void uws_remove_server_name(uws_app_t *app, const char *hostname_pattern);
#   void uws_add_server_name(uws_app_t *app, const char *hostname_pattern);

#   void uws_add_server_name_with_options(uws_app_t *app, const char *hostname_pattern, uws_socket_context_options_t options);
#   void uws_missing_server_name(uws_app_t *app, uws_missing_server_handler handler);
#   void uws_filter(uws_app_t *app, uws_filter_handler handler);


  #requests
  attach_function :uws_req_is_ancient, [:pointer], :int
  attach_function :uws_req_get_yield, [:pointer], :int
  attach_function :uws_req_set_field, [:pointer, :int], :void
  attach_function :uws_req_get_url, [:pointer], :pointer
  attach_function :uws_req_get_method, [:pointer], :pointer
  attach_function :uws_req_get_header, [:pointer, :pointer], :pointer
  attach_function :uws_req_get_query, [:pointer, :pointer], :pointer
  attach_function :uws_req_get_parameter, [:pointer, :uint16], :pointer

  #response
  attach_function :uws_res_end, [:pointer, :pointer, :int], :void
  attach_function :uws_res_pause, [:pointer], :void
  attach_function :uws_res_resume, [:pointer], :void
  attach_function :uws_res_write_continue, [:pointer], :void
  attach_function :uws_res_write_status, [:pointer, :pointer], :void
  attach_function :uws_res_write_header, [:pointer, :pointer, :pointer], :void
  attach_function :uws_res_write_header_int, [:pointer, :pointer, :uint64], :void
  attach_function :uws_res_end_without_body, [:pointer], :void
  attach_function :uws_res_write, [:pointer, :pointer], :int
  attach_function :uws_res_get_write_offset, [:pointer], :uint64
  attach_function :uws_res_has_responded, [:pointer], :int
  attach_function :uws_res_upgrade, [:pointer, :pointer, :pointer, :pointer, :pointer, :pointer], :void

  callback :uws_res_on_writable_handler, [:pointer, :uint64, :pointer], :int
  callback :uws_res_on_aborted_handler, [:pointer, :pointer], :void
  callback :uws_res_on_data_handler, [:pointer, :pointer, :int, :pointer], :void
  
  attach_function :uws_res_on_writable, [:pointer, :uws_res_on_writable_handler, :pointer], :void
  attach_function :uws_res_on_aborted, [:pointer, :uws_res_on_aborted_handler, :pointer], :void
  attach_function :uws_res_on_data, [:pointer, :uws_res_on_data_handler, :pointer], :void
  
end




class UWS::AppResponse
    attr_accessor :native_reponse
    attr_accessor :callbacks

    def initialize(response)
        @native_response = response
        @callbacks = {}
    end

    def upgrade(data, sec_web_socket_key, sec_web_socket_protocol, sec_web_socket_extensions, socket_context)
    
        data_ptr = nil
        if(optional_data.kind_of? String)
            data_ptr = FFI::MemoryPointer.from_string(optional_data)
        end
        
        if(sec_web_socket_key.kind_of? String)
            sec_web_socket_key_ptr = FFI::MemoryPointer.from_string(sec_web_socket_key)
        else
            sec_web_socket_key_ptr = FFI::MemoryPointer.from_string("")
        end

        if(sec_web_socket_protocol.kind_of? String)
            sec_web_socket_protocol_ptr = FFI::MemoryPointer.from_string(sec_web_socket_protocol)
        else
            sec_web_socket_protocol_ptr = FFI::MemoryPointer.from_string("")
        end

        if(sec_web_socket_extensions.kind_of? String)
            sec_web_socket_extensions_ptr = FFI::MemoryPointer.from_string(sec_web_socket_extensions)
        else
            sec_web_socket_extensions_ptr = FFI::MemoryPointer.from_string("")
        end

        UWS::CAPI.uws_res_upgrade(@native_reponse, data_ptr, sec_web_socket_key_ptr, sec_web_socket_protocol_ptr, sec_web_socket_extensions_ptr, socket_context.native_socket)
        return self
    end

    def on_data(callback, optional_data)
        @callbacks["on_data"] = Proc.new do | response, chunk, is_end, optional_data|
            data = optional_data.null? ? nil : optional_data.get_string(0)
            chunk_data =  nil
            if(!(chunk.null?))
                chunk_data = chunk.get_string(0);
                UWS::LibC.free(chunk)
            end
            callback.call(self, chunk_data, is_end != 0, data)
        end
        data_ptr = nil
        if(optional_data.kind_of? String)
            data_ptr = FFI::MemoryPointer.from_string(optional_data)
        end
        UWS::CAPI.uws_res_on_data(@native_reponse, @callbacks["on_data"], data_ptr)
        return self
    end

    def on_writable(callback, optional_data)
        @callbacks["on_writable"] = Proc.new do |response, offset, optional_data|
            data = optional_data.null? ? nil : optional_data.get_string(0)
            callback.call(self, offset, data)
        end
        data_ptr = nil
        if(optional_data.kind_of? String)
            data_ptr = FFI::MemoryPointer.from_string(optional_data)
        end
        UWS::CAPI.uws_res_on_writable(@native_reponse, @callbacks["v"], data_ptr)
        return self
    end

    def on_aborted(callback, optional_data)
        @callbacks["on_aborted"] = Proc.new do |response, optional_data|
            data = optional_data.null? ? nil : optional_data.get_string(0)
            callback.call(self, data)
        end
        data_ptr = nil
        if(optional_data.kind_of? String)
            data_ptr = FFI::MemoryPointer.from_string(optional_data)
        end
        UWS::CAPI.uws_res_on_aborted(@native_reponse, @callbacks["on_aborted"], data_ptr)
        return self
    end

    def end(message)
        if(!(message.kind_of? String))
            return nil
        end
        message = FFI::MemoryPointer.from_string(message)
        UWS::CAPI.uws_res_end(@native_response, message, 1)
        return self
    end

    def end_without_body()
        UWS::CAPI.uws_res_end_without_body(@native_response)
        return self
    end

    def pause()
        UWS::CAPI.uws_res_pause(@native_response)
        return self
    end

    def resume()
        UWS::CAPI.uws_res_resume(@native_response)
        return self
    end

    def get_write_offset()
        return UWS::CAPI.uws_res_get_write_offset(@native_response)
    end

    def has_write_offset?()
        return UWS::CAPI.uws_res_has_responded(@native_response) != 0
    end

    def write_continue()
        UWS::CAPI.uws_res_write_continue(@native_response)
        return self
    end
    
    def write_status(status)
        status_ptr = FFI::MemoryPointer.from_string(status)
        UWS::CAPI.uws_res_write_status(@native_response, status_ptr)
        return self
    end
    
    def write(data)
        data_ptr = FFI::MemoryPointer.from_string(data)
        return UWS::CAPI.uws_res_write(@native_response, data_ptr) != 0
    end

    def write_header(key, value)
        if(!(key.kind_of? String))
            return nil
        end
        key_ptr = FFI::MemoryPointer.from_string(key)

        if(value.kind_of? Integer)
            UWS::CAPI.uws_res_write_header_int(@native_response, key_ptr, value)
        elsif(value.kind_of? String)
            value_ptr = FFI::MemoryPointer.from_string(value)
            UWS::CAPI.uws_res_write_header(@native_response, key_ptr, value_ptr)
        end
        return self
    end
end

class UWS::AppRequest
    attr_accessor :native_socket

    def initialize(request)
        @native_request = request
    end

    def is_ancient?()
        return UWS::CAPI.uws_req_is_ancient(@native_request) != 0
    end

    def get_yield?()
        return UWS::CAPI.uws_req_get_yield(@native_request) != 0
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
        if(!(key.kind_of? String))
            return nil
        end
        
        native_value = FFI::MemoryPointer.from_string(lower_case_name)
        pointer = UWS::CAPI.uws_req_get_header(@native_request, native_value)
        value = pointer.get_string(0)
        UWS::LibC.free(pointer)
        return value
    end

    def get_query(key)
        if(!(key.kind_of? String))
            return nil
        end
        
        native_value = FFI::MemoryPointer.from_string(key)
        pointer = UWS::CAPI.uws_req_get_query(@native_request, native_value)
        value = pointer.get_string(0)
        UWS::LibC.free(pointer)
        return value
    end

    def get_parameter(index)
        if(!(index.kind_of? Integer))
            return nil
        end

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
        host = config[:host]
        @host = host.null? ? nil : host.get_string(0)
        @port = config[:port]
        @options = config[:options]
    end

end


class UWS::App
    attr_accessor :native_app

   def initialize()
        @native_app = FFI::AutoPointer.new(UWS::CAPI.uws_create_app(), UWS::CAPI.method(:uws_app_destroy))
        @callbacks = {}
   end


    def any(pattern, callback)
        if(!(pattern.kind_of? String))
            return self
        end
        
        callback_key = "any:#{pattern}"
        native_pattern = FFI::MemoryPointer.from_string(pattern)

        #callback wrapper
        @callbacks[callback_key] = Proc.new do |response, request| 
            callback.call(UWS::AppResponse.new(response), UWS::AppRequest.new(request))
        end

        UWS::CAPI.uws_app_any(@native_app, native_pattern, @callbacks[callback_key])
        return self
    end

    def trace(pattern, callback)
        if(!(pattern.kind_of? String))
            return self
        end
        
        callback_key = "trace:#{pattern}"
        native_pattern = FFI::MemoryPointer.from_string(pattern)

        #callback wrapper
        @callbacks[callback_key] = Proc.new do |response, request| 
            callback.call(UWS::AppResponse.new(response), UWS::AppRequest.new(request))
        end

        UWS::CAPI.uws_app_trace(@native_app, native_pattern, @callbacks[callback_key])
        return self
    end

    def connect(pattern, callback)
        if(!(pattern.kind_of? String))
            return self
        end
        
        callback_key = "connect:#{pattern}"
        native_pattern = FFI::MemoryPointer.from_string(pattern)

        #callback wrapper
        @callbacks[callback_key] = Proc.new do |response, request| 
            callback.call(UWS::AppResponse.new(response), UWS::AppRequest.new(request))
        end

        UWS::CAPI.uws_app_connect(@native_app, native_pattern, @callbacks[callback_key])
        return self
    end

    def head(pattern, callback)
        if(!(pattern.kind_of? String))
            return self
        end
        
        callback_key = "head:#{pattern}"
        native_pattern = FFI::MemoryPointer.from_string(pattern)

        #callback wrapper
        @callbacks[callback_key] = Proc.new do |response, request| 
            callback.call(UWS::AppResponse.new(response), UWS::AppRequest.new(request))
        end

        UWS::CAPI.uws_app_head(@native_app, native_pattern, @callbacks[callback_key])
        return self
    end

    def put(pattern, callback)
        if(!(pattern.kind_of? String))
            return self
        end
        
        callback_key = "put:#{pattern}"
        native_pattern = FFI::MemoryPointer.from_string(pattern)

        #callback wrapper
        @callbacks[callback_key] = Proc.new do |response, request| 
            callback.call(UWS::AppResponse.new(response), UWS::AppRequest.new(request))
        end

        UWS::CAPI.uws_app_put(@native_app, native_pattern, @callbacks[callback_key])
        return self
    end

    def patch(pattern, callback)
        if(!(pattern.kind_of? String))
            return self
        end
        
        callback_key = "patch:#{pattern}"
        native_pattern = FFI::MemoryPointer.from_string(pattern)

        #callback wrapper
        @callbacks[callback_key] = Proc.new do |response, request| 
            callback.call(UWS::AppResponse.new(response), UWS::AppRequest.new(request))
        end

        UWS::CAPI.uws_app_patch(@native_app, native_pattern, @callbacks[callback_key])
        return self
    end

    def delete(pattern, callback)
        if(!(pattern.kind_of? String))
            return self
        end
        
        callback_key = "delete:#{pattern}"
        native_pattern = FFI::MemoryPointer.from_string(pattern)

        #callback wrapper
        @callbacks[callback_key] = Proc.new do |response, request| 
            callback.call(UWS::AppResponse.new(response), UWS::AppRequest.new(request))
        end

        UWS::CAPI.uws_app_delete(@native_app, native_pattern, @callbacks[callback_key])
        return self
    end

    def options(pattern, callback)
        if(!(pattern.kind_of? String))
            return self
        end
        
        callback_key = "options:#{pattern}"
        native_pattern = FFI::MemoryPointer.from_string(pattern)

        #callback wrapper
        @callbacks[callback_key] = Proc.new do |response, request| 
            callback.call(UWS::AppResponse.new(response), UWS::AppRequest.new(request))
        end

        UWS::CAPI.uws_app_options(@native_app, native_pattern, @callbacks[callback_key])
        return self
    end

   def get(pattern, callback)
        if(!(pattern.kind_of? String))
            return self
        end

        callback_key = "get:#{pattern}"
        native_pattern = FFI::MemoryPointer.from_string(pattern)

        #callback wrapper
        @callbacks[callback_key] = Proc.new do |response, request| 
            callback.call(UWS::AppResponse.new(response), UWS::AppRequest.new(request))
        end

        UWS::CAPI.uws_app_get(@native_app, native_pattern, @callbacks[callback_key])
        return self
   end
   
   def post(pattern, callback)
        if(!(pattern.kind_of? String))
            return self
        end

        callback_key = "post:#{pattern}"
        native_pattern = FFI::MemoryPointer.from_string(pattern)

        #callback wrapper
        @callbacks[callback_key] = Proc.new do |response, request| 
            callback.call(UWS::AppResponse.new(response), UWS::AppRequest.new(request))
        end

        UWS::CAPI.uws_app_post(@native_app, native_pattern, @callbacks[callback_key])
        return self
    end


   def listen(port_or_config, callback)
        
        if(port_or_config.nil?)
            port = 3000  
            UWS::CAPI.uws_app_listen(@native_app, port, lambda do | socket, config | 
                callback.call(UWS::ListenSocket.new(socket, false), UWS::AppListenConfig.new(config))
            end)
        elsif(port_or_config.kind_of? Integer)
            port = port_or_config  
            UWS::CAPI.uws_app_listen(@native_app, port, lambda do | socket, config | 
                callback.call(UWS::ListenSocket.new(socket, false), UWS::AppListenConfig.new(config))
            end)
        else
            config = UWS::Uws_app_listen_config_t.new
            config[:host] = port_or_config[:host] ? FFI::MemoryPointer.from_string(port_or_config[:host]) : FFI::MemoryPointer.from_string("")
            

            port = port_or_config[:port]
            if(port.kind_of? Integer)
                config[:port] = port
            else
                config[:port] = 8080
            end
            
            options = port_or_config[:options]
            if(options.kind_of? Integer)
                config[:options] = options
            else
                config[:options] = 0
            end
            
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