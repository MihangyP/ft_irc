#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <cstring>
#include <sys/socket.h>
#include <poll.h>

#define PORT 6667
#define MAX_CLIENTS 10
#define SERVER_PASSWORD "secret123"

struct Client {
    int fd;
    std::string input_buffer;
    std::string nickname;
    std::string username;
    bool authenticated;
    bool registered;

    Client(int fd_) : fd(fd_), authenticated(false), registered(false) {}
};

std::map<int, Client*> clients;

// Utilitaire : rendre un fd non bloquant
void set_nonblocking(int fd) {
    fcntl(fd, F_SETFL, O_NONBLOCK);
}

// Envoi simple
void send_msg(Client* c, const std::string& msg) {
    write(c->fd, msg.c_str(), msg.size());
}

// Déconnecte et supprime un client
void disconnect_client(int fd) {
    close(fd);
    delete clients[fd];
    clients.erase(fd);
}

// Tentative d'enregistrement
void try_register(Client* c) {
    if (c->authenticated && !c->nickname.empty() && !c->username.empty()) {
        c->registered = true;
        send_msg(c, "001 Welcome to ft_irc, " + c->nickname + "\r\n");
        std::cout << "Client " << c->fd << " registered as " << c->nickname << "\n";
    }
}

// Gestion des commandes PASS, NICK, USER
void parse_command(Client* c, const std::string& line) {
    std::istringstream iss(line);
    std::string cmd;
    iss >> cmd;

    if (cmd == "PASS") {
        std::string pwd;
        iss >> pwd;
        if (pwd == SERVER_PASSWORD) c->authenticated = true;
        else {
            send_msg(c, "464 Invalid password\r\n");
            disconnect_client(c->fd);
        }
    } else if (cmd == "NICK") {
        std::string nick;
        iss >> nick;
        c->nickname = nick;
    } else if (cmd == "USER") {
        std::string user;
        iss >> user;
        c->username = user;
    }

    try_register(c);
}

// Lecture non bloquante et reconstruction de lignes
void handle_input(Client* c) {
    char buf[512];
    int bytes = read(c->fd, buf, sizeof(buf));
    if (bytes <= 0) {
        disconnect_client(c->fd);
        return;
    }

    c->input_buffer.append(buf, bytes);

    size_t pos;
    while ((pos = c->input_buffer.find('\n')) != std::string::npos) {
        std::string line = c->input_buffer.substr(0, pos);
        // supprime \r si présent
        if (!line.empty() && line.back() == '\r') line.pop_back();

        c->input_buffer.erase(0, pos + 1);
        parse_command(c, line);
    }
}

int main() {
    int listener_fd = socket(AF_INET, SOCK_STREAM, 0);
    set_nonblocking(listener_fd);

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(listener_fd, (sockaddr*)&addr, sizeof(addr));
    listen(listener_fd, 5);

    struct pollfd fds[MAX_CLIENTS + 1];
    fds[0].fd = listener_fd;
    fds[0].events = POLLIN;
    int nfds = 1;

    std::cout << "Server listening on port " << PORT << "\n";

    while (true) {
        int ret = poll(fds, nfds, -1);
        if (ret < 0) continue;

        // Nouvelle connexion
        if (fds[0].revents & POLLIN) {
            int client_fd = accept(listener_fd, NULL, NULL);
            if (client_fd >= 0) {
                set_nonblocking(client_fd);
                Client* c = new Client(client_fd);
                clients[client_fd] = c;

                fds[nfds].fd = client_fd;
                fds[nfds].events = POLLIN;
                nfds++;

                std::cout << "New client connected: fd=" << client_fd << "\n";
            }
        }

        // Lecture clients existants
        for (int i = 1; i < nfds; ++i) {
            Client* c = clients[fds[i].fd];
            if (fds[i].revents & POLLIN) handle_input(c);
        }
    }
}

