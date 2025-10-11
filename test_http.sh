#!/bin/bash

echo "Testing HTTP/1.0 request:"
echo "========================="
curl --http1.0 -v http://localhost:3000/ 2>&1 | head -20