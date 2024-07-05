This folder contains a simple module based on [Teja's Nginx Module Guide](https://tejgop.github.io/nginx-module-guide) with some minor adjustments. All it does is define a content handler that generates a hello world message with the request arguments.

## Usage

```bash
# Setup Nginx
wget http://nginx.org/download/nginx-1.27.0.tar.gz
tar -xzf nginx-1.27.0.tar.gz
cd nginx-1.27.0
./configure --prefix=../build --add-module=../ngx_http_hello_world_module 
make install

# Run Nginx in non-daemon mode
# The corresponding config fule is in build/conf/nginx.conf
../build/sbin/nginx
```

Once Nginx is running, test it via `curl`:

```
$ curl http://localhost:8000/test?hello=world -i
HTTP/1.1 200 OK
Server: nginx/1.27.0
Date: Fri, 05 Jul 2024 07:46:04 GMT
Content-Type: text/plain
Content-Length: 45
Connection: keep-alive

Hello! The request arguments are:
hello=world
```
