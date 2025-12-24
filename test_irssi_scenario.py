#!/usr/bin/env python3
"""Simule exactement ce que fait irssi lors de la connexion"""
import socket
import time

def irssi_connect(nickname, username="user"):
    """Simule la séquence de connexion irssi"""
    print(f"\n{'='*60}")
    print(f"CONNEXION CLIENT IRSSI - Nickname: {nickname}")
    print('='*60)
    
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("localhost", 6667))
    sock.settimeout(2)
    
    # Séquence exacte irssi
    commands = [
        "CAP LS 302",
        f"NICK {nickname}",
        f"USER {username} localhost localhost :{username}",
        "PASS test123"
    ]
    
    for cmd in commands:
        print(f">>> {cmd}")
        sock.send((cmd + "\r\n").encode())
        time.sleep(0.1)
        try:
            resp = sock.recv(4096).decode(errors='ignore')
            if resp:
                print(f"<<< {resp}")
        except socket.timeout:
            pass
    
    # Attendre réponse finale
    time.sleep(0.3)
    try:
        final = sock.recv(4096).decode(errors='ignore')
        if final:
            print(f"<<< {final}")
            
        # Vérifier si enregistré
        if "001" in final or "Welcome" in final:
            print(f"✅ CLIENT ENREGISTRÉ: {nickname}")
            return sock, True
        elif "433" in final:
            print(f"❌ ERREUR 433: Nickname {nickname} déjà utilisé")
            return sock, False
        else:
            print(f"⚠️  État inconnu pour {nickname}")
            return sock, None
    except socket.timeout:
        print(f"⚠️  Timeout - état inconnu pour {nickname}")
        return sock, None

# Test exact du scénario
print("TEST: Créer 'donto' puis 'Donto' comme avec irssi")

client1, success1 = irssi_connect("donto", "user1")
time.sleep(0.5)

client2, success2 = irssi_connect("Donto", "user2")
time.sleep(0.5)

print(f"\n{'='*60}")
print("RÉSULTATS:")
print('='*60)
print(f"Client 1 'donto': {'ENREGISTRÉ ✅' if success1 else 'REFUSÉ ❌'}")
print(f"Client 2 'Donto': {'ENREGISTRÉ ✅' if success2 else 'REFUSÉ ❌'}")

if success1 and success2:
    print("\n❌ PROBLÈME: Les deux clients sont enregistrés avec le même nom!")
elif success1 and not success2:
    print("\n✅ CORRECT: Le deuxième client a été refusé (ERR_NICKNAMEINUSE)")
else:
    print("\n⚠️  Résultat inattendu")

client1.close()
client2.close()
