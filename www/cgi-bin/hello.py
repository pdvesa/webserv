# Print the required HTTP headers
print("HTTP/1.1 200 OK")  # The HTTP status line (OK status)
print("Content-Type: text/html")  # The content type of the response (HTML)
print()  # End of the header section (empty line)

# Print the body of the response in HTML
print("<html>")
print("<head><title>Hello</title></head>")
print("<body>")
print("<h1>Hello</h1>")
print("</body>")
print("</html>")
