#!/bin/bash

# Test user limit functionality

echo "=== Test MODE +l (User Limit) ==="
echo ""

# Start server
./ircserv 6667 test123 > server.log 2>&1 &
SERVER_PID=$!
sleep 1

echo "1. Creating channel and setting limit to 2 users..."
{
    echo "PASS test123"
    sleep 0.2
    echo "NICK user1"
    sleep 0.2
    echo "USER user1 0 * :User One"
    sleep 0.5
    echo "JOIN #testlimit"
    sleep 0.3
    echo "MODE #testlimit +l 2"
    sleep 0.5
} | nc localhost 6667 > user1.log 2>&1 &
sleep 2

echo "2. Second user joining (should succeed)..."
{
    echo "PASS test123"
    sleep 0.2
    echo "NICK user2"
    sleep 0.2
    echo "USER user2 0 * :User Two"
    sleep 0.5
    echo "JOIN #testlimit"
    sleep 0.5
} | nc localhost 6667 > user2.log 2>&1 &
sleep 2

echo "3. Third user trying to join (should FAIL - channel full)..."
{
    echo "PASS test123"
    sleep 0.2
    echo "NICK user3"
    sleep 0.2
    echo "USER user3 0 * :User Three"
    sleep 0.5
    echo "JOIN #testlimit"
    sleep 0.5
} | nc localhost 6667 > user3.log 2>&1 &
sleep 2

echo ""
echo "=== Results ==="
echo "User 3 should have received ERR_CHANNELISFULL (471)"
echo ""
echo "User 3 output:"
grep -E "471|Cannot join|full" user3.log || echo "  (checking log...)"
cat user3.log | grep -A 2 "JOIN"

echo ""
echo "4. Removing limit with MODE -l..."
{
    echo "PASS test123"
    sleep 0.2
    echo "NICK user1_ctrl"
    sleep 0.2
    echo "USER user1 0 * :User One Control"
    sleep 0.5
    echo "JOIN #testlimit"
    sleep 0.3
    echo "MODE #testlimit -l"
    sleep 0.5
} | nc localhost 6667 > user1_ctrl.log 2>&1 &
sleep 2

echo "5. Now user4 should be able to join..."
{
    echo "PASS test123"
    sleep 0.2
    echo "NICK user4"
    sleep 0.2
    echo "USER user4 0 * :User Four"
    sleep 0.5
    echo "JOIN #testlimit"
    sleep 0.5
} | nc localhost 6667 > user4.log 2>&1 &
sleep 2

echo ""
echo "User 4 output:"
grep -E "JOIN|471|Welcome" user4.log || cat user4.log

# Cleanup
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null
rm -f *.log

echo ""
echo "Test completed!"
