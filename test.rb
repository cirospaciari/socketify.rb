require_relative "uws/uws"

app = UWS::App.new()

payload =  {
    "_id": "61d62168faa585de13bdd931",
    "index": 0,
    "guid": "acbef340-6605-4a1f-99a2-aa42fab261f5",
    "isActive": true,
    "balance": "$1,823.90",
    "picture": "http://placehold.it/32x32",
    "age": 38,
    "eyeColor": "green",
    "name": "Vinson Kelly",
    "gender": "male",
    "company": "SUREPLEX",
    "email": "vinsonkelly@sureplex.com",
    "phone": "+1 (808) 435-2165",
    "address": "195 Flatbush Avenue, Beyerville, Texas, 8360",
    "about": "Voluptate voluptate eu excepteur magna. Lorem dolore eiusmod sint irure fugiat aliqua officia adipisicing. In voluptate ullamco non deserunt aliquip eu occaecat sit id ut. Consectetur nisi eiusmod velit occaecat occaecat qui reprehenderit ad tempor.\r\n",
    "registered": "2019-03-23T09:12:52 +03:00",
    "latitude": -69.348101,
    "longitude": 76.48184
}

home_request = lambda do |response, request| 
    response.json(payload)
end
app.get("/", home_request)
# app.get("/", lambda {|response, request| response.end("Hello World uWS from Ruby!")})
# app.listen(8082, lambda {|socket, config| puts "Listening on port %d" % [config.port] })
app.listen({ "port": 8082, "host": "127.0.0.1", "options": 0 }, lambda {|socket, config| puts "Listening on port http://#{config.host}:#{config.port}" })
app.run()