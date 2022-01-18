
killall -9 ruby
set -e
#generated self signed cert and key files
mkdir -p ./misc
FILE=./misc/key.pem
if [ -f "$FILE" ]; then
    echo "key and crt already created"
else 
    ../uWebSockets/uSockets/misc/gen_test_certs.sh
    cp /tmp/certs/selfsigned_client_key.pem ./misc/key.pem
    cp /tmp/certs/selfsigned_client_crt.pem ./misc/crt.pem
fi


cd ../ext/uws/
ruby extconf.rb
make clean
make
cd ../../
ruby -Ilib:ext -r uws ./tests/hello_world_async.rb --jit
killall -9 ruby

#--yjit --jit