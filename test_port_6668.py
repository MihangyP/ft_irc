#!/usr/bin/env python3
import socket, time

def test(nick):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("localhost", 6668))  # Port 6668 comme irssi
    s.send(f"PASS test123\r\nNICK {nick}\r\nUSER user 0 * :user\r\n".encode())
    time.sleep(0.3)
    resp = s.recv(4096).decode(errors='ignore')
    print(f"{nick}: {'433 ERREUR' if '433' in resp else 'ACCEPTÉ ✅' if '001' in resp else '?'}")
    print(resp[:200])
    return s

s1 = test("donto")
s2 = test("DOnto")
s1.close()
s2.close()
