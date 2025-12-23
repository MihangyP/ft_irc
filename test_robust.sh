#!/bin/bash

# Comprehensive crash test

echo "=== IRC Server Robustness Test ==="
echo ""

# Start server
./ircserv 6667 test123 > server_robust.log 2>&1 &
SERVER_PID=$!
sleep 1

echo "Test 1: TOPIC with invalid channel name (+t instead of #channel)"
{
    echo "PASS test123"
    sleep 0.2
    echo "NICK testuser1"
    sleep 0.2
    echo "USER testuser1 0 * :Test User 1"
    sleep 0.5
    echo "JOIN #validchan"
    sleep 0.3
    echo "TOPIC +t :This should not crash"
    sleep 0.3
    echo "TOPIC #validchan +t"
    sleep 0.3
    echo "QUIT"
} | nc localhost 6667 > test1.log 2>&1
sleep 1

echo "Test 2: Commands on non-existent channels"
{
    echo "PASS test123"
    sleep 0.2
    echo "NICK testuser2"
    sleep 0.2
    echo "USER testuser2 0 * :Test User 2"
    sleep 0.5
    echo "TOPIC #nonexistent :test"
    sleep 0.2
    echo "MODE #nonexistent +i"
    sleep 0.2
    echo "PART #nonexistent"
    sleep 0.2
    echo "KICK #nonexistent someone :test"
    sleep 0.2
    echo "INVITE someone #nonexistent"
    sleep 0.2
    echo "QUIT"
} | nc localhost 6667 > test2.log 2>&1
sleep 1

echo "Test 3: Empty or malformed channel names"
{
    echo "PASS test123"
    sleep 0.2
    echo "NICK testuser3"
    sleep 0.2
    echo "USER testuser3 0 * :Test User 3"
    sleep 0.5
    echo "JOIN"
    sleep 0.2
    echo "JOIN  "
    sleep 0.2
    echo "TOPIC  :test"
    sleep 0.2
    echo "QUIT"
} | nc localhost 6667 > test3.log 2>&1
sleep 1

# Check if server is still alive
if ps -p $SERVER_PID > /dev/null; then
    echo "✅ SERVER SURVIVED ALL TESTS!"
    echo ""
    echo "Sample responses:"
    echo "Test 1 (invalid channel +t):"
    grep -E "403|No such" test1.log | head -2
    echo ""
    echo "Test 2 (nonexistent channel):"
    grep -E "403|442|No such|not on" test2.log | head -3
else
    echo "❌ SERVER CRASHED!"
    echo ""
    echo "Server log:"
    tail -20 server_robust.log
fi

# Cleanup
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null
rm -f *.log

echo ""
echo "Robustness test completed!"
