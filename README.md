# FT_IRC - Internet Relay Chat Server

Un serveur IRC compatible avec le protocole IRC (RFC 1459/2812) écrit en C++98.

## Compilation

```bash
make
```

Pour recompiler :
```bash
make re
```

Pour nettoyer :
```bash
make clean  # Supprime les .o
make fclean # Supprime tout
```

## Utilisation

```bash
./ircserv <port> <password>
```

- `port` : Port d'écoute (1024-65535 recommandé)
- `password` : Mot de passe du serveur

### Exemple

```bash
./ircserv 6667 mypassword
```

## Connexion avec un client

Vous pouvez utiliser n'importe quel client IRC (irssi, WeeChat, HexChat, etc.) :

```bash
# Avec netcat (pour tester)
nc localhost 6667

# Avec irssi
irssi
/connect localhost 6667 mypassword
```

## Commandes Implémentées

### Authentification et Enregistrement
- **PASS** `<password>` - Authentification avec le mot de passe du serveur
- **NICK** `<nickname>` - Définir/changer le pseudonyme
- **USER** `<username> <hostname> <servername> <realname>` - Enregistrement utilisateur
- **CAP** - Négociation de capacités

### Canaux
- **JOIN** `<channel>[,<channel>] [<key>[,<key>]]` - Rejoindre un ou plusieurs canaux
- **PART** `<channel>[,<channel>] [<message>]` - Quitter un ou plusieurs canaux
- **TOPIC** `<channel> [<topic>]` - Afficher ou modifier le topic d'un canal
- **NAMES** - Liste des utilisateurs (envoyé automatiquement au JOIN)

### Opérateurs de Canal
- **KICK** `<channel> <user> [<comment>]` - Éjecter un utilisateur d'un canal
- **INVITE** `<nickname> <channel>` - Inviter un utilisateur sur un canal
- **MODE** `<channel> <modes> [<params>]` - Modifier les modes du canal

#### Modes de Canal Supportés
- **i** - Invite-only (canal sur invitation uniquement)
- **t** - Topic restriction (seuls les opérateurs peuvent changer le topic)
- **k** - Channel key (mot de passe pour rejoindre le canal)
- **o** - Opérateur de canal (donner/retirer les privilèges d'opérateur)
- **l** - User limit (limite du nombre d'utilisateurs)

### Messages
- **PRIVMSG** `<target> <message>` - Envoyer un message privé ou à un canal
- **QUIT** `[<message>]` - Déconnexion du serveur

### Connexion et Heartbeat
- **PING** `[<token>]` - Tester la connexion (le serveur répond avec PONG)
- **PONG** `<token>` - Réponse à un PING du serveur

## Architecture

```
.
├── include/          # Fichiers d'en-tête (.hpp)
│   ├── Channel.hpp
│   ├── Command.hpp
│   ├── constants.h
│   ├── ft_irc.h
│   ├── IrcClient.hpp
│   ├── IrcException.hpp
│   ├── IrcLog.hpp
│   ├── IrcServer.hpp
│   ├── ParseCommand.hpp
│   └── StringHelper.hpp
├── src/              # Fichiers source (.cpp)
│   ├── Channel.cpp
│   ├── Command.cpp
│   ├── ft_irc.cpp
│   ├── IrcClient.cpp
│   ├── IrcException.cpp
│   ├── IrcLog.cpp
│   ├── IrcServer.cpp
│   ├── ParseCommand.cpp
│   └── StringHelper.cpp
└── Makefile
```

### Classes Principales

- **IrcServer** : Gère le serveur, les connexions et le routage des commandes
- **IrcClient** : Représente un client connecté
- **Channel** : Représente un canal avec ses membres, opérateurs et paramètres
- **Command** : Parse et stocke une commande IRC
- **ParseCommand** : Validation et parsing des commandes
- **StringHelper** : Utilitaires pour manipulation de chaînes

## Fonctionnalités

### Gestion des Canaux
- ✅ Création automatique de canaux
- ✅ Gestion des opérateurs (premier utilisateur = opérateur)
- ✅ Modes de canal (i, t, k, o, l)
- ✅ Protection par mot de passe
- ✅ Limitation du nombre d'utilisateurs
- ✅ Canal sur invitation uniquement
- ✅ Liste des membres avec statut (@operator)

### Sécurité
- ✅ Authentification par mot de passe
- ✅ Validation des pseudonymes (max 9 caractères)
- ✅ Vérification des permissions d'opérateur
- ✅ Gestion des erreurs IRC standards

### Réseau
- ✅ Sockets non-bloquants
- ✅ Poll() pour multiplexage I/O
- ✅ Gestion de multiples clients simultanés
- ✅ Buffer d'entrée par client
- ✅ Gestion propre des déconnexions

## Codes de Réponse IRC

Le serveur implémente les codes de réponse standards IRC :

### Succès
- 001 RPL_WELCOME
- 002 RPL_YOURHOST
- 003 RPL_CREATED
- 004 RPL_MYINFO
- 331 RPL_NOTOPIC
- 332 RPL_TOPIC
- 341 RPL_INVITING
- 353 RPL_NAMREPLY
- 366 RPL_ENDOFNAMES

### Erreurs
- 401 ERR_NOSUCHNICK
- 403 ERR_NOSUCHCHANNEL
- 404 ERR_CANNOTSENDTOCHAN
- 411 ERR_NORECIPIENT
- 412 ERR_NOTEXTTOSEND
- 421 ERR_UNKNOWNCOMMAND
- 431 ERR_NONICKNAMEGIVEN
- 432 ERR_ERRONEUSNICKNAME
- 433 ERR_NICKNAMEINUSE
- 441 ERR_USERNOTINCHANNEL
- 442 ERR_NOTONCHANNEL
- 443 ERR_USERONCHANNEL
- 451 ERR_NOTREGISTERED
- 461 ERR_NEEDMOREPARAMS
- 462 ERR_ALREADYREGISTRED
- 464 ERR_PASSWDMISMATCH
- 471 ERR_CHANNELISFULL
- 473 ERR_INVITEONLYCHAN
- 475 ERR_BADCHANNELKEY
- 482 ERR_CHANOPRIVSNEEDED

## Exemples d'utilisation

### Créer et rejoindre un canal
```
JOIN #test
```

### Définir un mot de passe sur un canal
```
MODE #test +k mypassword
```

### Rejoindre un canal protégé
```
JOIN #test mypassword
```

### Limiter le nombre d'utilisateurs
```
MODE #test +l 10
```

### Activer le mode invitation uniquement
```
MODE #test +i
```

### Donner les privilèges d'opérateur
```
MODE #test +o username
```

### Inviter quelqu'un
```
INVITE username #test
```

### Éjecter un utilisateur
```
KICK #test username :Raison du kick
```

### Changer le topic
```
TOPIC #test :Nouveau sujet du canal
```

## Notes Techniques

- **Standard** : C++98
- **Compilation** : `-Wall -Wextra -std=c++98`
- **I/O** : Non-bloquant avec `poll()`
- **Signaux** : Gestion de SIGINT et SIGQUIT pour arrêt propre

## Auteurs

Projet ft_irc - École 42
