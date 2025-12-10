#!/usr/bin/env python3

import sys
import os
import urllib.parse

# Read POST data from stdin
content_length = int(os.environ.get('CONTENT_LENGTH', 0))
post_data = ""

if content_length > 0:
    post_data = sys.stdin.read(content_length)

# Parse POST data
parsed_data = urllib.parse.parse_qs(post_data)

# Output HTTP headers
print("Content-Type: text/html")
print()  # Blank line separates headers from body

# Output HTML
print("""<!DOCTYPE html>
<html>
<head>
    <title>POST CGI Test</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            max-width: 800px;
            margin: 50px auto;
            padding: 20px;
            background: #f5f5f5;
        }
        .container {
            background: white;
            padding: 30px;
            border-radius: 10px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
        }
        h1 { color: #2c3e50; }
        .data {
            background: #ecf0f1;
            padding: 15px;
            border-radius: 5px;
            margin: 10px 0;
        }
        .key {
            font-weight: bold;
            color: #e74c3c;
        }
        .value {
            color: #27ae60;
        }
        .form {
            margin-top: 30px;
            padding: 20px;
            background: #f8f9fa;
            border-radius: 5px;
        }
        input, textarea {
            width: 100%;
            padding: 10px;
            margin: 5px 0;
            border: 1px solid #ddd;
            border-radius: 4px;
        }
        button {
            background: #3498db;
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 16px;
        }
        button:hover {
            background: #2980b9;
        }
        .back-link {
            text-align: center;
            margin-top: 20px;
        }
        .back-link a {
            color: #667eea;
            text-decoration: none;
            font-weight: bold;
        }
        .back-link a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>POST Data Received</h1>
""")

if parsed_data:
    print("<h2>Received Data:</h2>")
    for key, values in parsed_data.items():
        for value in values:
            print(f"""
            <div class="data">
                <span class="key">{key}:</span>
                <span class="value">{value}</span>
            </div>
            """)
else:
    print("<p><em>No POST data received</em></p>")

print(f"""
        <h3>CGI Environment:</h3>
        <div class="data">
            <span class="key">REQUEST_METHOD:</span>
            <span class="value">{os.environ.get('REQUEST_METHOD', 'N/A')}</span>
        </div>
        <div class="data">
            <span class="key">CONTENT_LENGTH:</span>
            <span class="value">{content_length}</span>
        </div>
        <div class="data">
            <span class="key">CONTENT_TYPE:</span>
            <span class="value">{os.environ.get('CONTENT_TYPE', 'N/A')}</span>
        </div>

        <div class="form">
            <h3>Try Another POST Request:</h3>
            <form method="POST" action="/cgi-bin/test_post.py">
                <label>Name:</label>
                <input type="text" name="name" placeholder="Enter your name" required>

                <label>Email:</label>
                <input type="email" name="email" placeholder="your@email.com" required>

                <label>Message:</label>
                <textarea name="message" rows="4" placeholder="Your message here..."></textarea>

                <button type="submit">Submit POST Request</button>
            </form>
        </div>
            <div class="back-link">
            <a href="/">Back to Main Page</a>
        </div>
    </div>
</body>
</html>
""")
