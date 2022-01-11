require "mkmf"
require 'rbconfig'
require "fileutils"

extension_name = 'uws'
dir_config(extension_name)

module OS
    def OS.windows?
      (/cygwin|mswin|mingw|bccwin|wince|emx/ =~ RUBY_PLATFORM) != nil
    end
  
    def OS.mac?
     (/darwin/ =~ RUBY_PLATFORM) != nil
    end
  
    def OS.unix?
      !OS.windows?
    end
  
    def OS.linux?
      OS.unix? and not OS.mac?
    end
  
    def OS.jruby?
      RUBY_ENGINE == 'jruby'
    end
end


PACKAGE_ROOT_DIR = File.expand_path(File.join(File.dirname(__FILE__), "..", ".."))
UWEBSOCKETS_CAPI_DIR = File.join(PACKAGE_ROOT_DIR, "uWebSockets", "capi")
USOCKETS_INCLUDE_DIR = File.join(PACKAGE_ROOT_DIR, "uWebSockets","uSockets", "src")


$CFLAGS << " -O3 -flto -fPIC "
$INCFLAGS << " -I #{UWEBSOCKETS_CAPI_DIR} -I #{USOCKETS_INCLUDE_DIR}"
$DLDFLAGS << " -lstdc++ "

#build uWebSockets and uSockets static library
if  OS::unix? 
  %x{make -C #{PACKAGE_ROOT_DIR}/uWebSockets/capi capi}
  $LOCAL_LIBS << " #{File.join(UWEBSOCKETS_CAPI_DIR, "libuwebsockets.a")} "    
else
    #not really working just an examples  
    %x{make -C #{PACKAGE_ROOT_DIR}/uWebSockets/capi capi-windows}
    $LOCAL_LIBS << " #{File.join(UWEBSOCKETS_CAPI_DIR, "libuwebsockets.lib")} "
end

create_makefile(File.join(extension_name, extension_name))
