#!/bin/bash
# Test netcat validation edge cases

PORT=6667
PASSWORD="test123"

echo "=== TEST: Netcat Input Validation ==="
echo ""

# Start server
./ircserv $PORT $PASSWORD &
SERVER_PID=$!
echo "Server started (PID: $SERVER_PID)"
sleep 1

echo ""
echo "--- Test 1: Double USER command ---"
{
    echo "PASS $PASSWORD"
    sleep 0.2
    echo "NICK TestUser"
    sleep 0.2
    echo "USER testuser 0 * :First User"
    sleep 0.2
    echo "USER testuser2 0 * :Second User"
    sleep 0.5
} | nc localhost $PORT &
NC_PID=$!
sleep 1
echo "Expected: ERR_ALREADYREGISTERED (462) on second USER"
kill $NC_PID 2>/dev/null
echo ""

echo "--- Test 2: Double PASS command ---"
{
    echo "PASS $PASSWORD"
    sleep 0.2
    echo "PASS wrongpass"
    sleep 0.5
} | nc localhost $PORT &
NC_PID=$!
sleep 1
echo "Expected: ERR_ALREADYREGISTERED (462) on second PASS"
kill $NC_PID 2>/dev/null
echo ""

echo "--- Test 3: Invalid NICK characters ---"
{
    echo "PASS $PASSWORD"
    sleep 0.2
    echo "NICK Test@User"
    sleep 0.5
} | nc localhost $PORT &
NC_PID=$!
sleep 1
echo "Expected: ERR_ERRONEUSNICKNAME (432)"
kill $NC_PID 2>/dev/null
echo ""

echo "--- Test 4: Empty NICK ---"
{
    echo "PASS $PASSWORD"
    sleep 0.2
    echo "NICK "
    sleep 0.5
} | nc localhost $PORT &
NC_PID=$!
sleep 1
echo "Expected: ERR_NONICKNAMEGIVEN (431)"
kill $NC_PID 2>/dev/null
echo ""

echo "--- Test 5: Self-KICK prevention ---"
{
    echo "PASS $PASSWORD"
    sleep 0.2
    echo "NICK OpUser"
    sleep 0.2
    echo "USER opuser 0 * :Operator User"
    sleep 0.2
    echo "JOIN #test"
    sleep 0.2
    echo "KICK #test OpUser :Self kick"
    sleep 0.5
} | nc localhost $PORT &
NC_PID=$!
sleep 2
echo "Expected: Error message preventing self-kick"
kill $NC_PID 2>/dev/null
echo ""

echo "--- Test 6: Self MODE -o prevention ---"
{
    echo "PASS $PASSWORD"
    sleep 0.2
    echo "NICK OpUser2"
    sleep 0.2
    echo "USER opuser2 0 * :Operator User 2"
    sleep 0.2
    echo "JOIN #testmode"
    sleep 0.2
    echo "MODE #testmode -o OpUser2"
    sleep 0.5
} | nc localhost $PORT &
NC_PID=$!
sleep 2
echo "Expected: Error message preventing self-demotion"
kill $NC_PID 2>/dev/null
echo ""

echo "--- Test 7: USER with less than 4 params ---"
{
    echo "PASS $PASSWORD"
    sleep 0.2
    echo "NICK TestUser3"
    sleep 0.2
    echo "USER testuser3"
    sleep 0.5
} | nc localhost $PORT &
NC_PID=$!
sleep 1
echo "Expected: ERR_NEEDMOREPARAMS (461)"
kill $NC_PID 2>/dev/null
echo ""

# Clean up
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

echo ""
echo "=== All tests completed ==="
echo "Review the output above to verify proper error handling"
