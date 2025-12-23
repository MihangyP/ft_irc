#!/bin/bash

# Test PING/PONG functionality

echo "=== Test PING/PONG ==="
echo ""

# Start server in background
./ircserv 6667 test123 > /dev/null 2>&1 &
SERVER_PID=$!
sleep 1

echo "Testing PING before authentication..."
{
    echo "PING :before_auth"
    sleep 0.5
} | nc -w 2 localhost 6667

echo ""
echo "Testing PING after authentication..."
{
    echo "PASS test123"
    sleep 0.2
    echo "NICK testuser"
    sleep 0.2
    echo "USER testuser 0 * :Test User"
    sleep 0.5
    echo "PING :after_auth"
    sleep 0.5
    echo "PING localhost"
    sleep 0.5
    echo "QUIT :Bye"
} | nc -w 3 localhost 6667

# Stop server
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo ""
echo "Test completed!"
