killall -9 ruby
cd ../ext/uws/
ruby extconf.rb
make
cd ../../
ruby -Ilib:ext -r uws ./tests/graceful-shutdown.rb --jit
killall -9 ruby

#--yjit --jit