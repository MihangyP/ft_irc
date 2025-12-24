ft_irc
Internet Relay Chat
Summary: This project is about creating your own IRC server.
You will use an actual IRC client to connect to your server and test it.
The Internet is governed by solid standard protocols that allow connected computers to
interact with each other.
It’s always beneficial to understand these protocols.

Introduction
Internet Relay Chat or IRC is a text-based communication protocol on the Internet.
It offers real-time messaging that can be either public or private. Users can exchange
direct messages and join group channels.
IRC clients connect to IRC servers in order to join channels. IRC servers are connected
together to form a network.
2Chapter II
General rules
• Your program should not crash in any circumstances (even when it runs out of
memory), and should not quit unexpectedly.
If it happens, your project will be considered non-functional and your grade will be
0.
• You have to turn in a Makefile which will compile your source files. It must not
perform unnecessary relinking.
• Your Makefile must at least contain the rules:
$(NAME), all, clean, fclean and re.
• Compile your code with c++ using the flags -Wall -Wextra -Werror.
• Your code must comply with the C++ 98 standard. Then, it should still compile
if you add the flag -std=c++98.
• Try to always code using C++ features when available (for example, choose <cstring>
over <string.h>). You are allowed to use C functions, but always prefer their C++
versions if possible.
• Any external library and Boost libraries are forbidden.

5Chapter IV
Mandatory Part
Program Name
Files to Submit
Makefile
Arguments
External Function
Libft authorized
Description
ircserv
Makefile, *.{h, hpp}, *.cpp, *.tpp, *.ipp,
an optional configuration file
NAME, all, clean, fclean, re
port: The listening port
password: The connection password
Everything in C++ 98.
socket, close, setsockopt, getsockname,
getprotobyname, gethostbyname, getaddrinfo,
freeaddrinfo, bind, connect, listen, accept,
htons, htonl, ntohs, ntohl, inet_addr, inet_ntoa,
inet_ntop, send, recv, signal, sigaction,
sigemptyset, sigfillset, sigaddset, sigdelset,
sigismember, lseek, fstat, fcntl, poll (or
equivalent)
n/a
An IRC server in C++ 98
You are required to develop an IRC server using the C++ 98 standard.
You must not develop an IRC client.
You must not implement server-to-server communication.
Your executable will be run as follows:
./ircserv <port> <password>
• port: The port number on which your IRC server will be listening for incoming
IRC connections.
• password: The connection password. It will be needed by any IRC client that tries
to connect to your server.
6ft_irc
Internet Relay Chat
Even though poll() is mentioned in the subject and the evaluation
scale, you may use any equivalent such as select(), kqueue(), or
epoll().
7ft_irc
IV.1
Internet Relay Chat
Requirements
• The server must be capable of handling multiple clients simultaneously without
hanging.
• Forking is prohibited. All I/O operations must be non-blocking.
• Only 1 poll() (or equivalent) can be used for handling all these operations (read,
write, but also listen, and so forth).
Because you have to use non-blocking file descriptors, it is
possible to use read/recv or write/send functions with no poll()
(or equivalent), and your server wouldn’t be blocking.
However, it would consume more system resources.
Therefore, if you attempt to read/recv or write/send in any file
descriptor without using poll() (or equivalent), your grade will be
0.
• Several IRC clients exist. You have to choose one of them as a reference. Your
reference client will be used during the evaluation process.
• Your reference client must be able to connect to your server without encountering
any error.
• Communication between client and server has to be done via TCP/IP (v4 or v6).
• Using your reference client with your server must be similar to using it with any
official IRC server. However, you only have to implement the following features:
◦ You must be able to authenticate, set a nickname, a username, join a channel,
send and receive private messages using your reference client.
◦ All the messages sent from one client to a channel have to be forwarded to
every other client that joined the channel.
◦ You must have operators and regular users.
◦ Then, you have to implement the commands that are specific to channel
operators:
∗ KICK - Eject a client from the channel
∗ INVITE - Invite a client to a channel
∗ TOPIC - Change or view the channel topic
∗ MODE - Change the channel’s mode:
· i: Set/remove Invite-only channel
· t: Set/remove the restrictions of the TOPIC command to channel
operators
· k: Set/remove the channel key (password)
8ft_irc
Internet Relay Chat
· o: Give/take channel operator privilege
· l: Set/remove the user limit to channel
• Of course, you are expected to write a clean code.
IV.2
For MacOS only
Since MacOS does not implement write() in the same way as other Unix
OSes, you are permitted to use fcntl().
You must use file descriptors in non-blocking mode in order to get a
behavior similar to the one of other Unix OSes.
However, you are allowed to use fcntl() only as follows:
fcntl(fd, F_SETFL, O_NONBLOCK);
Any other flag is forbidden.
IV.3
Test example
Verify every possible error and issue, such as receiving partial data, low bandwidth, etc.
To ensure that your server correctly processes all data sent to it, the following simple
test using nc can be performed:
\$> nc -C 127.0.0.1 6667
com^Dman^Dd
\$>
Use ctrl+D to send the command in several parts: ’com’, then ’man’, then ’d\n’.
In order to process a command, you have to first aggregate the received packets in
order to rebuild it.
9Chapter V

10Chapter VI
Submission and peer-evaluation
Submit your assignment to your Git repository as usual. Only the work inside your
repository will be evaluated during the defense. Do not hesitate to double-check the
names of your files to ensure they are correct.
You are encouraged to create test programs for your project even though they will
not be submitted or graded. Those tests could be especially useful to test your server
during defense, but also your peer’s if you have to evaluate another ft_irc one day.
Indeed, you are free to use whatever tests you need during the evaluation process.
Your reference client will be used during the evaluation process.
During the evaluation, a brief modification of the project may occasionally be re-
quested. This could involve a minor behavior change, a few lines of code to write or
rewrite, or an easy-to-add feature.
While this step may not be applicable to every project, you must be prepared for it
if it is mentioned in the evaluation guidelines.
This step is meant to verify your actual understanding of a specific part of the project.
The modification can be performed in any development environment you choose (e.g.,
your usual setup), and it should be feasible within a few minutes — unless a specific
timeframe is defined as part of the evaluation.
You can, for example, be asked to make a small update to a function or script, modify a
display, or adjust a data structure to store new information, etc.
The details (scope, target, etc.) will be specified in the evaluation guidelines and may
11ft_irc
Internet Relay Chat
vary from one evaluation to another for the same project.
16D85ACC441674FBA2DF65190663F432222F81AA0248081A7C1C1823F7A96F0B74495
15056E97427E5B22F07132659EC8D88B574BD62C94BB654D5835AAD889B014E078705
709F6E02
12