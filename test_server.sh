#!/bin/bash

# Couleurs pour l'affichage
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}=== Test du serveur IRC ===${NC}\n"

# Vérifier que le serveur existe
if [ ! -f "./ircserv" ]; then
    echo -e "${RED}Erreur: ircserv n'existe pas. Compilez d'abord avec 'make'${NC}"
    exit 1
fi

# Lancer le serveur en arrière-plan
echo -e "${YELLOW}Lancement du serveur sur le port 6667...${NC}"
./ircserv 6667 test123 &
SERVER_PID=$!
sleep 1

# Vérifier que le serveur est lancé
if ! ps -p $SERVER_PID > /dev/null; then
    echo -e "${RED}Erreur: Le serveur n'a pas démarré${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Serveur démarré (PID: $SERVER_PID)${NC}\n"

# Fonction pour envoyer des commandes IRC
send_irc_cmd() {
    local CMD="$1"
    echo -e "${YELLOW}Envoi: ${NC}$CMD"
    echo -e "$CMD\r" | nc -w 1 localhost 6667
    sleep 0.5
}

echo -e "${YELLOW}=== Test de connexion ===${NC}"
{
    echo "PASS test123"
    sleep 0.2
    echo "NICK testuser"
    sleep 0.2
    echo "USER testuser 0 * :Test User"
    sleep 0.5
    echo "PING :test_ping_token"
    sleep 0.3
    echo "JOIN #test"
    sleep 0.5
    echo "PRIVMSG #test :Hello World!"
    sleep 0.5
    echo "QUIT :Bye"
    sleep 0.5
} | nc localhost 6667 &
NC_PID=$!

# Attendre la fin du test
sleep 3

# Tuer le serveur
echo -e "\n${YELLOW}Arrêt du serveur...${NC}"
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo -e "${GREEN}✓ Tests terminés${NC}"
