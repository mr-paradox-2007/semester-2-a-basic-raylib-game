import http.server
import socketserver

PORT = 8080


class MyHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        # This tells the browser to cache files so it doesn't redownload every time
        self.send_header("Cache-Control", "public, max-age=3600")
        super().end_headers()


with socketserver.TCPServer(("", PORT), MyHTTPRequestHandler) as httpd:
    print(f"Serving at http://localhost:{PORT}")
    httpd.serve_forever()


class MyHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        # These headers are like a "VIP Pass" for your game through firewalls
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        super().end_headers()


# Fix for .wasm and .data file types
MyHandler.extensions_map.update({
    '.wasm': 'application/wasm',
    '.data': 'application/octet-stream',
})

PORT = 8000
with socketserver.TCPServer(("", PORT), MyHandler) as httpd:
    print(f"Server started at port {PORT}")
    httpd.serve_forever()
