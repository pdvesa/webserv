import os

post_body = os.environ.get('POST_BODY')

key_value = post_body.split('=')
key, value = key_value

print("Content-Type: text/html")
print()

print(f"""
<html>
<body style="font-family: Arial, sans-serif; text-align: center; padding: 50px; background-color: #f4f4f4;">

    <h1 style="color: #333; font-size: 2em;">Your favorite color is {value}!</h1>

    <!-- Go Home link below the color message -->
    <a href="/" style="background-color: #007bff; color: white; padding: 10px 20px; text-decoration: none; font-size: 1em; border-radius: 5px; margin-top: 20px; display: inline-block;">
        Go Home
    </a>

</body>
</html>
""")