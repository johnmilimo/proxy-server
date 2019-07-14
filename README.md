# SIMPLE PROXY SERVER

This is a very minimalistic example of a proxy server. It receives client request and forwards it to the specified upstream server. It then receives upstream server response and sends it over to the client.

## Start the proxy server
	$ bash run_server.sh [UPSTREAM_HOST] [UPSTREAM_PORT]

Example:

	$ bash run_server.sh 127.0.0.1 80

The bash script compiles the C program files and then starts the proxy server on port `8088`. URL: `http://localhost:8088`

NOTE: The [UPSTREAM_HOST] should be unsecured http as the proxy server is not able to perform ssl handshake with the upstream at the moment. 

You can pull the docker image for `httpbin` and run it locally to test with. See below how to:

Pull image:

	$docker pull kennethreitz/httpbin

Run httpbin on port 80:

	docker run -p 80:80 kennethreitz/httpbin
