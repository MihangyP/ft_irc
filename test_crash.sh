#!/bin/bash

# Test crash scenarios

echo "=== Test Crash Prevention ==="
echo ""

# Start server
./ircserv 6667 test123 > crash_test.log 2>&1 &
SERVER_PID=$!
sleep 1

echo "1. Testing TOPIC on non-existent channel..."
{
    echo "PASS test123"
    sleep 0.2
    echo "NICK crashtest"
    sleep 0.2
    echo "USER crashtest 0 * :Crash Test"
    sleep 0.5
    echo "TOPIC #nonexistent :This should not crash"
    sleep 0.3
    echo "TOPIC #nonexistent +t"
    sleep 0.3
    echo "QUIT :Done"
} | nc localhost 6667 > topic_crash.log 2>&1
sleep 1

echo "2. Testing invalid MODE commands..."
{
    echo "PASS test123"
    sleep 0.2
    echo "NICK crashtest2"
    sleep 0.2
    echo "USER crashtest2 0 * :Crash Test 2"
    sleep 0.5
    echo "JOIN #test"
    sleep 0.3
    echo "TOPIC #test +t"
    sleep 0.3
    echo "TOPIC #test -t"
    sleep 0.3
    echo "QUIT :Done"
} | nc localhost 6667 > mode_crash.log 2>&1
sleep 1

# Check if server is still running
if ps -p $SERVER_PID > /dev/null; then
    echo "✅ Server is still running - no crash!"
    echo ""
    echo "Topic test output:"
    cat topic_crash.log | grep -E "403|TOPIC|No such"
    echo ""
    echo "Mode test output:"
    cat mode_crash.log | grep -E "TOPIC"
else
    echo "❌ Server crashed!"
fi

# Cleanup
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null
rm -f *.log

echo ""
echo "Test completed!"
