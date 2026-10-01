<img width="1024" height="572" alt="image" src="https://github.com/user-attachments/assets/89fbe29d-e2cd-40f3-bf99-4a6508e1c9e3" />

*This project has been created as part of the 42 curriculum by mklimuk, msimek, ssukhija.*

# DESCRIPTION
This project involves building a fully functional Internet Relay Chat (IRC) server from scratch using standard C++98. It handles simultaneous multi-client connections over TCP/IP, supporting core IRC features like channel management and private messaging. Instead of relying on traditional multi-threading or process forking, the architecture achieves concurrency through a single-threaded event loop utilizing I/O multiplexing with the poll() system call. This approach allows the server to efficiently monitor all connected sockets at once—dynamically handling new connections, reading incoming data, and dispatching responses exactly when clients are ready.

Avaliable commands:
* PASS - enter the server's password
* NICK - enter you nick
* USER - enter your user's information
* JOIN - join the channel
* TOPIC - set channel's topic
* MODE - set channel's mode
    * i - set/remove invite-only channel
    * t - set/remove restrictions of the topic
    * k - set/remove the channel password (key)
    * o - give/take channel operator privelege
    * l - set/remove the user limit
* KICK - remove a client from a channel
* PART - leave the channel
* QUIT - leave the server

Our team choose Irssi client as a reference. It's a classic client that doesn't require a lot of additional logic. Its behaviour is closer to RFC specification which is what we need to implement according to the subject.

# INSTRUCTIONS
## Build
make

## Run the server
./ircserv 'port' 'server password' 

## Connection 
### Through Irssi
/connect 'ip' 'port' 'server password'
### Through nc
nc 'ip' 'port'

## How to run commands 
* PASS
    * in nc: PASS 'password'
    * in Irssi: password sent during connection 
* USER
    * in nc: USER 'user' 'mode' '*' : 'realname'
    * in Irssi: registration happens automatically after connection
* NICK 
    * in nc: NICK 'your nick'
    * in Irssi: /nick 'your nick'
* PRIVMSG
    * in nc: PRIVMSG 'nick' :'message'
    * in Irssi: /msg 'user' 'message'
* JOIN
    * in nc: JOIN #'channelName'
    * in Irssi: /join #'channelName'
* TOPIC 
    * in nc: TOPIC #'channelName' :'new topic'
    * in Irssi: /topic #'channelName' :'new topic'
* MODE
    * i: Set/remove Invite-only channel
        * in nc: MODE #'channelName' +i or MODE #'channeName' -i
        * in Irssi: /mode #'channelName' +i or /mode #'channelName' -i
    * t: Set/remove the restrictions of the TOPIC command to channel
operators
        * in nc: MODE topic 'new topic'
        * in Irssi: /mode topic 'new topic'
    * k: Set/remove the channel key (password)
        * in nc: MODE #'channelName' +k 'password' or MODE #'channelName' -k
        * in Irssi: /mode #'channelName' +k 'password' or /mode #'channelName' -k
    * o: Give/take channel operator privilege
        * in nc: MODE #'channelName' +o 'nick' or MODE #'channelName' -o 'nick'
        * in Irssi: /mode #'channelName' +o 'nick' or /mode #'channelName' -o 'nick'
    * l: Set/remove the user limit to channel
        * in nc: MODE #'channelName' +l 'number' or MODE #'channelName' -l
        * in Irssi: /mode #'channelName' +l 'number' or /mode #'channelName' -l
* KICK 
    * in nc: KICK #'channelName' 'nick' 
    * in Irssi: /kick #'channelName' 'nick'
* PART 
    * in nc: PART #'channelName'
    * in Irssi: /part #'channelName'
* QUIT
    * in nc: QUIT
    * in Irssi: /quit

# RESOURCES
* [Wikipedia article about IRC](https://cs.wikipedia.org/wiki/Internet_Relay_Chat)
* [Modern IRC Client Protocol](https://modern.ircdocs.horse)
* [Irssi documentation](https://irssi.org/documentation/)
* [Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
* [Example IRC Communication The UChicago χ-Projects](http://chi.cs.uchicago.edu/chirc/irc_examples.html)
* [RFC 2812 - Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
* AI was used to improve our understanding of the project, shaping our ideas and looking for edge cases in our testing.
