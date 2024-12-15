#!/usr/bin/python3

import os
import cgi

# Start the HTML document
print("""
<html>
<head>
    <title>Environmental Variables</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            background-color: #f0f0f0;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            color: #333;
        }
        .container {
            text-align: center;
            background-color: white;
            border-radius: 8px;
            padding: 20px;
            box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
        }
        h1 {
            color: #4CAF50;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin-top: 20px;
        }
        th, td {
            padding: 8px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }
        th {
            background-color: #f2f2f2;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Hello Tester!</h1>
        <p>Here are your system's environment variables:</p>
        <table>
            <tr><th>Variable</th><th>Value</th></tr>
""")

# Fetch and display environment variables
for key, value in os.environ.items():
    print(f"<tr><td>{key}</td><td>{value}</td></tr>")

# End the table and the HTML document
print("""
        </table>
    </div>
</body>
</html>
""")