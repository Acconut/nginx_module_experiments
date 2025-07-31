This folder contains an HTTP response filter based on [Teja's Nginx Module Guide](https://tejgop.github.io/nginx-module-guide). All it does is inject an audio HTML element at the end of each response sent by Nginx.

## Usage

```bash
# Setup Nginx
wget http://nginx.org/download/nginx-1.27.0.tar.gz
tar -xzf nginx-1.27.0.tar.gz
cd nginx-1.27.0
./configure --prefix=../build --add-module=../ngx_http_body_counter_module
make install

# Run Nginx in non-daemon mode
# The corresponding config fule is in build/conf/nginx.conf
../build/sbin/nginx
```

Once Nginx is running, test it via `curl`:

```
$ curl http://localhost:8000/ -i
HTTP/1.1 200 OK
Server: nginx/1.27.0
Date: Fri, 05 Jul 2024 08:21:38 GMT
Content-Type: text/html
Content-Length: 727
Last-Modified: Fri, 05 Jul 2024 07:52:53 GMT
Connection: keep-alive
ETag: "6687a655-267"
Accept-Ranges: bytes

<!DOCTYPE html>
<html>
<head>
<title>Welcome to nginx!</title>
<style>
html { color-scheme: light dark; }
body { width: 35em; margin: 0 auto;
font-family: Tahoma, Verdana, Arial, sans-serif; }
</style>
</head>
<body>
<h1>Welcome to nginx!</h1>
<p>If you see this page, the nginx web server is successfully installed and
working. Further configuration is required.</p>

<p>For online documentation and support please refer to
<a href="http://nginx.org/">nginx.org</a>.<br/>
Commercial support is available at
<a href="http://nginx.com/">nginx.com</a>.</p>

<p><em>Thank you for using nginx.</em></p>
</body>
</html>
<audio controls loop autoplay src="https://upload.wikimedia.org/wikipedia/commons/8/85/Holst-_mars.ogg"></audio>
```

Notice that trailing `<audio>` element.
