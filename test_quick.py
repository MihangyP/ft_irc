#!/usr/bin/env python3
import socket
import time

print("Test simple de collision")

# Client 1: donto
s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s1.connect(("localhost", 6667))
s1.send(b"PASS test123\r\nNICK donto\r\nUSER u1 0 * :U1\r\n")
time.sleep(0.3)
r1 = s1.recv(4096).decode(errors='ignore')
print(f"Client 1 (donto):\n{r1}\n")

# Client 2: Donto  
s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s2.connect(("localhost", 6667))
s2.send(b"PASS test123\r\nNICK Donto\r\nUSER u2 0 * :U2\r\n")
time.sleep(0.3)
r2 = s2.recv(4096).decode(errors='ignore')
print(f"Client 2 (Donto):\n{r2}\n")

if "433" in r2:
    print("✅ Collision détectée")
else:
    print("❌ PROBLÈME: Pas de collision détectée!")
    if "001" in r2:
        print("   Le client 2 a été enregistré alors qu'il ne devrait pas!")

s1.close()
s2.close()
