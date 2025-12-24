#!/usr/bin/env python3
"""Test exact de ce que fait irssi"""
import socket
import time

def test_client(nick, user="srasolom"):
    print(f"\n{'='*70}")
    print(f"TEST CLIENT: {nick}")
    print('='*70)
    
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("localhost", 6668))
    s.settimeout(3)
    
    # Exact sequence from irssi
    commands = [
        "CAP LS 302",
        f"NICK {nick}",
        f"USER {user} localhost localhost :{user}",
        "PASS test123",
        "CAP REQ :multi-prefix",
        "CAP END"
    ]
    
    all_responses = []
    
    for cmd in commands:
        print(f">>> {cmd}")
        s.send((cmd + "\r\n").encode())
        time.sleep(0.05)
        
    # Lire toutes les réponses
    time.sleep(0.5)
    try:
        while True:
            data = s.recv(4096).decode(errors='ignore')
            if not data:
                break
            all_responses.append(data)
            print(f"<<< {data}")
            time.sleep(0.05)
    except socket.timeout:
        pass
    
    full_response = ''.join(all_responses)
    
    # Analyse
    has_433 = "433" in full_response
    has_welcome = "001" in full_response or "Welcome" in full_response
    
    print(f"\n📊 ANALYSE:")
    print(f"   - Erreur 433 (nickname in use): {'OUI ❌' if has_433 else 'NON'}")
    print(f"   - Message Welcome (001): {'OUI ✅' if has_welcome else 'NON'}")
    
    if has_433 and not has_welcome:
        print(f"   ➡️  Résultat: REFUSÉ (comme attendu)")
        return s, False
    elif has_welcome:
        print(f"   ➡️  Résultat: ACCEPTÉ")
        return s, True
    else:
        print(f"   ➡️  Résultat: INCONNU")
        return s, None

print("="*70)
print("SIMULATION EXACTE DU SCÉNARIO IRSSI")
print("="*70)

# Premier client: donto
client1, accepted1 = test_client("donto")
time.sleep(1)

# Deuxième client: DOnto (casse différente)
client2, accepted2 = test_client("DOnto")

print(f"\n{'='*70}")
print("RÉSULTAT FINAL:")
print('='*70)
print(f"1. 'donto' : {'ACCEPTÉ ✅' if accepted1 else 'REFUSÉ ❌'}")
print(f"2. 'DOnto' : {'ACCEPTÉ ✅' if accepted2 else 'REFUSÉ ❌'}")

if accepted1 and accepted2:
    print("\n❌ PROBLÈME: Les deux ont été acceptés! La détection de doublon ne fonctionne pas.")
elif accepted1 and not accepted2:
    print("\n✅ CORRECT: Le serveur a bien rejeté 'DOnto' (doublon de 'donto')")
    print("   Note: irssi ajoute automatiquement '_' après avoir reçu le 433")
else:
    print("\n⚠️  Situation inattendue")

client1.close()
client2.close()
