#!/usr/bin/env python3
"""
CGI Script - Query String Demo
Reads query parameters from the URI and displays them in an HTML page
Example: /cgi-bin/test_need_query.py?name=John&age=25&city=Madrid
"""

import os
import sys
from urllib.parse import parse_qs

# Get query string from environment
query_string = os.environ.get('QUERY_STRING', '')

# Parse query parameters
params = parse_qs(query_string)

# CGI Response Headers
print("Content-Type: text/html")
print("\n\n")  # Blank line separates headers from body

# HTML Response
print("""<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Query String Demo</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }

        .container {
            background: white;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
            padding: 40px;
            max-width: 700px;
            width: 100%;
        }

        h1 {
            color: #667eea;
            text-align: center;
            margin-bottom: 10px;
            font-size: 2em;
        }

        .subtitle {
            text-align: center;
            color: #666;
            margin-bottom: 30px;
        }

        .success {
            background: #e8f5e9;
            border-left: 4px solid #4caf50;
            padding: 15px;
            margin: 20px 0;
            border-radius: 5px;
        }

        .warning {
            background: #fff3cd;
            border-left: 4px solid #ffc107;
            padding: 15px;
            margin: 20px 0;
            border-radius: 5px;
        }

        .param-list {
            background: #f8f9fa;
            border-radius: 10px;
            padding: 20px;
            margin: 20px 0;
        }

        .param-item {
            display: flex;
            justify-content: space-between;
            padding: 10px;
            border-bottom: 1px solid #e0e0e0;
            align-items: center;
        }

        .param-item:last-child {
            border-bottom: none;
        }

        .param-key {
            font-weight: bold;
            color: #667eea;
            font-family: monospace;
        }

        .param-value {
            color: #333;
            background: white;
            padding: 5px 10px;
            border-radius: 5px;
            font-family: monospace;
        }

        .example-box {
            background: #263238;
            color: #aed581;
            padding: 15px;
            border-radius: 10px;
            margin: 20px 0;
            font-family: 'Courier New', monospace;
            overflow-x: auto;
        }

        .btn {
            display: inline-block;
            background: #667eea;
            color: white;
            padding: 10px 20px;
            border-radius: 5px;
            text-decoration: none;
            margin: 5px;
            transition: background 0.3s;
        }

        .btn:hover {
            background: #5568d3;
        }

        footer {
            text-align: center;
            margin-top: 30px;
            color: #999;
            font-size: 0.9em;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1> CGI Query String Demo</h1>
        <p class="subtitle">Python CGI Script Processing URI Parameters</p>
""")

# Display results based on query string
if not query_string:
    print("""
        <div class="warning">
            <p><strong> No query parameters received!</strong></p>
            <p>Try adding parameters to the URL.</p>
        </div>

        <h2 style="color: #667eea; margin-top: 30px;"> How to Use:</h2>
        <p style="margin: 10px 0;">Add query parameters to the URL like this:</p>
        <div class="example-box">
/cgi-bin/test_need_query.py?name=John&age=25&city=Madrid
        </div>

        <h3 style="color: #667eea; margin-top: 20px;">Try These Examples:</h3>
        <div style="margin: 20px 0;">
            <a href="/cgi-bin/test_need_query.py?name=Alice&age=30" class="btn">Example 1: Name & Age</a>
            <a href="/cgi-bin/test_need_query.py?name=Bob&city=Barcelona&job=Developer" class="btn">Example 2: Multiple Params</a>
            <a href="/cgi-bin/test_need_query.py?greeting=Hello%20World&status=active" class="btn">Example 3: URL Encoded</a>
        </div>
    """)
else:
    print(f"""
        <div class="success">
            <p><strong>✅ Query string received successfully!</strong></p>
            <p style="margin-top: 10px;">Raw query: <code style="background: white; padding: 2px 5px; border-radius: 3px;">{query_string}</code></p>
        </div>

        <h2 style="color: #667eea; margin-top: 30px;"> Parsed Parameters:</h2>
        <div class="param-list">
    """)

    if params:
        for key, values in params.items():
            for value in values:
                print(f"""
            <div class="param-item">
                <span class="param-key">{key}</span>
                <span class="param-value">{value}</span>
            </div>
                """)
    else:
        print("""
            <p style="padding: 20px; text-align: center; color: #666;">No valid parameters found</p>
        """)

    print("""
        </div>

        <h3 style="color: #667eea; margin-top: 30px;"> Try Another Query:</h3>
        <div style="margin: 20px 0;">
            <a href="/cgi-bin/test_need_query.py" class="btn">Clear (No Params)</a>
            <a href="/cgi-bin/test_need_query.py?test=123" class="btn">Simple Test</a>
        </div>
    """)

# Footer with environment info
print(f"""
        <div style="background: #f8f9fa; border-radius: 10px; padding: 15px; margin-top: 30px;">
            <h3 style="color: #667eea; margin-bottom: 10px;"> CGI Environment Info:</h3>
            <div style="font-family: monospace; font-size: 0.9em; color: #666;">
                <p><strong>REQUEST_METHOD:</strong> {os.environ.get('REQUEST_METHOD', 'N/A')}</p>
                <p><strong>SCRIPT_NAME:</strong> {os.environ.get('SCRIPT_NAME', 'N/A')}</p>
                <p><strong>SERVER_SOFTWARE:</strong> {os.environ.get('SERVER_SOFTWARE', 'N/A')}</p>
            </div>
        </div>

        <footer>
            <p> Python CGI Script - Webserv</p>
            <p>Query String Demo - 2025</p>
        </footer>
    </div>
</body>
</html>
""")

sys.exit(0)
