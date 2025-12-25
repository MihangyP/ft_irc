#!/bin/bash

# Test script to verify server behavior without fcntl/non-blocking sockets
# Tests various blocking scenarios to ensure poll() handles everything correctly

PORT=6669
PASSWORD="pass"

echo "========================================="
echo "IRC Server Blocking Tests"
echo "========================================="

# Test 1: Slow client that sends data byte by byte
echo ""
echo "[TEST 1] Slow client - sending commands byte by byte"
(
    sleep 1
    for char in P A S S ' ' p a s s '\r' '\n' N I C K ' ' s l o w '\r' '\n' U S E R ' ' s l o w ' ' 0 ' ' '*' ' ' ':' r e a l '\r' '\n'; do
        echo -n "$char"
        sleep 0.05
    done
    sleep 1
) | nc localhost $PORT &
CLIENT1=$!

sleep 3
echo "✓ Test 1 complete"

# Test 2: Multiple simultaneous connections
echo ""
echo "[TEST 2] Multiple simultaneous connections (10 clients)"
for i in {1..10}; do
    (
        sleep 0.5
        echo -e "PASS $PASSWORD\r\nNICK client$i\r\nUSER user$i 0 * :Real$i\r\n"
        sleep 2
        echo -e "QUIT :leaving\r\n"
    ) | nc localhost $PORT &
done

sleep 4
echo "✓ Test 2 complete"

# Test 3: Client that connects but never sends data
echo ""
echo "[TEST 3] Idle client (connects but sends nothing)"
(
    sleep 10
) | nc localhost $PORT &
CLIENT3=$!

sleep 2
echo "✓ Test 3 running in background"

# Test 4: Client that sends partial command and waits
echo ""
echo "[TEST 4] Partial command (incomplete line)"
(
    sleep 1
    echo -n "PASS $PASSWORD"
    sleep 5
    echo -e "\r\n"
    echo -e "NICK partial\r\nUSER partial 0 * :real\r\nQUIT\r\n"
) | nc localhost $PORT &

sleep 7
echo "✓ Test 4 complete"

# Test 5: Client sending very long messages
echo ""
echo "[TEST 5] Long message (>512 chars)"
(
    sleep 1
    echo -e "PASS $PASSWORD\r\nNICK longmsg\r\nUSER longmsg 0 * :real\r\n"
    sleep 1
    # Send a message longer than 512 bytes
    LONG_MSG="PRIVMSG #test :"
    for i in {1..100}; do
        LONG_MSG="${LONG_MSG}AAAAAAAAAA"
    done
    echo -e "${LONG_MSG}\r\n"
    sleep 1
    echo -e "QUIT\r\n"
) | nc localhost $PORT &

sleep 4
echo "✓ Test 5 complete"

# Test 6: Rapid fire commands
echo ""
echo "[TEST 6] Rapid fire - many commands quickly"
(
    sleep 1
    for i in {1..50}; do
        echo -e "PING :test$i\r"
    done
    echo -e "PASS $PASSWORD\r\nNICK rapid\r\nUSER rapid 0 * :real\r\nQUIT\r\n"
) | nc localhost $PORT &

sleep 3
echo "✓ Test 6 complete"

# Test 7: Client disconnects abruptly without QUIT
echo ""
echo "[TEST 7] Abrupt disconnect (no QUIT command)"
(
    sleep 1
    echo -e "PASS $PASSWORD\r\nNICK abrupt\r\nUSER abrupt 0 * :real\r\n"
    sleep 1
    # Just close connection without QUIT
) | nc localhost $PORT &

sleep 3
echo "✓ Test 7 complete"

# Test 8: Interleaved operations - JOIN/PART while others are connecting
echo ""
echo "[TEST 8] Interleaved channel operations"
(
    sleep 1
    echo -e "PASS $PASSWORD\r\nNICK user1\r\nUSER user1 0 * :real\r\n"
    sleep 1
    echo -e "JOIN #test\r\n"
    sleep 1
    for i in {1..10}; do
        echo -e "PRIVMSG #test :Message $i\r\n"
        sleep 0.2
    done
    echo -e "PART #test\r\nQUIT\r\n"
) | nc localhost $PORT &

(
    sleep 2
    echo -e "PASS $PASSWORD\r\nNICK user2\r\nUSER user2 0 * :real\r\n"
    sleep 1
    echo -e "JOIN #test\r\n"
    sleep 2
    echo -e "QUIT\r\n"
) | nc localhost $PORT &

sleep 6
echo "✓ Test 8 complete"

# Test 9: Client sends data without newline
echo ""
echo "[TEST 9] No newline terminator"
(
    sleep 1
    echo -n "PASS $PASSWORD"
    sleep 3
) | nc localhost $PORT &

sleep 4
echo "✓ Test 9 complete"

# Test 10: Stress test - many clients joining/leaving same channel
echo ""
echo "[TEST 10] Channel stress test (20 clients)"
for i in {1..20}; do
    (
        sleep $((i % 3))
        echo -e "PASS $PASSWORD\r\nNICK stress$i\r\nUSER stress$i 0 * :real\r\n"
        sleep 1
        echo -e "JOIN #stress\r\n"
        sleep 2
        echo -e "PRIVMSG #stress :Hello from $i\r\n"
        sleep 1
        echo -e "PART #stress\r\nQUIT\r\n"
    ) | nc localhost $PORT &
done

sleep 8
echo "✓ Test 10 complete"

echo ""
echo "========================================="
echo "All tests launched!"
echo "========================================="
echo ""
echo "Monitor your server logs for:"
echo "  - No crashes"
echo "  - No 'Accept failed' errors"
echo "  - No stuck clients"
echo "  - Proper cleanup of disconnected clients"
echo ""
echo "Waiting for all background clients to finish..."
sleep 5

# Kill any remaining idle clients
kill $CLIENT3 2>/dev/null
wait

echo ""
echo "========================================="
echo "Tests Complete!"
echo "========================================="
echo ""
echo "Check server output. If no crashes occurred,"
echo "the server handles blocking sockets correctly!"
