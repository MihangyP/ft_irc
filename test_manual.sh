#!/bin/bash
# Manual test - displays server responses

PORT=6667
PASSWORD="test123"

echo "Starting server on port $PORT..."
./ircserv $PORT $PASSWORD &
SERVER_PID=$!
sleep 1

echo ""
echo "=== Test Double USER command ==="
(
    echo "PASS $PASSWORD"
    sleep 0.3
    echo "NICK TestUser"
    sleep 0.3
    echo "USER testuser 0 * :First User"
    sleep 0.3
    echo "USER testuser2 0 * :Second User"
    sleep 1
    echo "QUIT :Goodbye"
) | nc localhost $PORT

echo ""
echo "=== Test Self-KICK ==="
(
    echo "PASS $PASSWORD"
    sleep 0.3
    echo "NICK OpUser"
    sleep 0.3
    echo "USER opuser 0 * :Operator User"
    sleep 0.3
    echo "JOIN #test"
    sleep 0.3
    echo "KICK #test OpUser :Self kick attempt"
    sleep 1
    echo "QUIT :Goodbye"
) | nc localhost $PORT

echo ""
echo "=== Test Self MODE -o ==="
(
    echo "PASS $PASSWORD"
    sleep 0.3
    echo "NICK OpUser2"
    sleep 0.3
    echo "USER opuser2 0 * :Operator User 2"
    sleep 0.3
    echo "JOIN #testmode"
    sleep 0.3
    echo "MODE #testmode -o OpUser2"
    sleep 1
    echo "QUIT :Goodbye"
) | nc localhost $PORT

echo ""
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null
echo "Test completed"
