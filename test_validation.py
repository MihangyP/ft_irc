#!/usr/bin/env python3
# Test IRC server validation

import socket
import time

def send_cmd(sock, cmd):
    """Send command and wait for response"""
    print(f">> {cmd}")
    sock.send((cmd + '\r\n').encode())
    time.sleep(0.3)
    try:
        sock.settimeout(0.5)
        response = sock.recv(4096).decode('utf-8', errors='ignore')
        print(f"<< {response}")
        return response
    except socket.timeout:
        return ""

def test_double_user():
    print("\n=== Test 1: Double USER command ===")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('localhost', 6667))
    
    send_cmd(sock, "PASS test123")
    send_cmd(sock, "NICK TestUser")
    send_cmd(sock, "USER testuser 0 * :First User")
    resp = send_cmd(sock, "USER testuser2 0 * :Second User")
    
    if "462" in resp or "ALREADYREGISTERED" in resp:
        print("✓ PASS: Second USER rejected")
    else:
        print("✗ FAIL: Second USER should be rejected")
    
    sock.close()

def test_self_kick():
    print("\n=== Test 2: Self-KICK prevention ===")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('localhost', 6667))
    
    send_cmd(sock, "PASS test123")
    send_cmd(sock, "NICK OpUser")
    send_cmd(sock, "USER opuser 0 * :Operator User")
    time.sleep(0.5)  # Wait for registration
    send_cmd(sock, "JOIN #test")
    resp = send_cmd(sock, "KICK #test OpUser :Self kick")
    
    if "cannot kick yourself" in resp.lower() or "441" in resp:
        print("✓ PASS: Self-kick prevented")
    else:
        print("✗ FAIL: Self-kick should be prevented")
        print(f"   Response: {resp}")
    
    sock.close()

def test_self_deop():
    print("\n=== Test 3: Self MODE -o prevention ===")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('localhost', 6667))
    
    send_cmd(sock, "PASS test123")
    send_cmd(sock, "NICK OpUser2")
    send_cmd(sock, "USER opuser2 0 * :Operator User 2")
    time.sleep(0.5)
    send_cmd(sock, "JOIN #testmode")
    resp = send_cmd(sock, "MODE #testmode -o OpUser2")
    
    if "cannot remove your own" in resp.lower() or "482" in resp:
        print("✓ PASS: Self-demotion prevented")
    else:
        print("✗ FAIL: Self-demotion should be prevented")
        print(f"   Response: {resp}")
    
    sock.close()

def test_invalid_nick():
    print("\n=== Test 4: Invalid NICK characters ===")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('localhost', 6667))
    
    send_cmd(sock, "PASS test123")
    resp = send_cmd(sock, "NICK Test@User#")
    
    if "432" in resp or "ERRONEUSNICKNAME" in resp:
        print("✓ PASS: Invalid NICK rejected")
    else:
        print("✗ FAIL: Invalid NICK should be rejected")
    
    sock.close()

def test_double_pass():
    print("\n=== Test 5: Double PASS command ===")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(('localhost', 6667))
    
    send_cmd(sock, "PASS test123")
    resp = send_cmd(sock, "PASS wrongpass")
    
    if "462" in resp or "ALREADYREGISTERED" in resp:
        print("✓ PASS: Second PASS rejected")
    else:
        print("✗ FAIL: Second PASS should be rejected")
    
    sock.close()

if __name__ == "__main__":
    print("Starting IRC Server Validation Tests")
    print("Make sure server is running on port 6667 with password 'test123'")
    
    time.sleep(1)
    
    try:
        test_double_user()
        time.sleep(0.5)
        test_self_kick()
        time.sleep(0.5)
        test_self_deop()
        time.sleep(0.5)
        test_invalid_nick()
        time.sleep(0.5)
        test_double_pass()
        
        print("\n=== All tests completed ===")
    except Exception as e:
        print(f"\nError during testing: {e}")
        print("Make sure the server is running!")
