killall -9 ruby
cd ../ext/uws/
ruby extconf.rb
make
cd ../../
ruby -Ilib:ext -r uws ./tests/hello_world.rb
killall -9 ruby