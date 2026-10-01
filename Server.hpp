#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <unistd.h>
#include <csignal>
#include <vector>
#include <cctype> //for std::isdigit


//for socket
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h> //for sockaddr_in

#include <fcntl.h> 
#include <arpa/inet.h>
#include <poll.h>
#include "Client.hpp"
#include "Channel.hpp"
#include "SystemMsgs.hpp"
#include <map>
#include <set>

//for MODE
#include <sstream>
#include <cstdlib> 

#define RESET "\033[;0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"


class Server{
    public:
        Server();
        ~Server();

        void setPort(int user_port);
        void setPassword(std::string user_pass);

        void ServerInit();
        void CreateSocket();
        void AcceptClient();
        bool ReceiveData(int fd);
        bool SendData(int fd);



        std::vector<std::string> SplitCommand(std::string command);
        void ParseCommand(Client &client, std::vector<std::string> args);

        void handlePass(Client &client, std::vector<std::string> args);
        void handleNick(Client &client, std::vector<std::string> args);
        void handleUser(Client &clients, std::vector<std::string> args);
        void handlePrivmsg( Client &client, std::vector <std::string> args);
        void handleQuit(Client &client, std::vector <std::string> args);
        void handlePing(Client &client, std::vector <std::string> args);
        void handlePong(Client &client, std::vector <std::string> args);
        void handleTopic(Client &client, std::vector <std::string> args);
        void handleWhois(Client &client, std::vector <std::string> args);
        void handleCap(Client &client, std::vector <std::string> args);
        void handleKick(Client &client, std::vector <std::string> args);
        void handleInvite(Client &client, std::vector <std::string> args);
        void handleMode(Client &client, std::vector <std::string> args);

        void handleWho(Client &client, std::vector <std::string> args);

        void sendReplyThree(Client &client, const std::string &code, const std::string &message);
        void sendReplyFour(Client &client, const std::string &code, const std::string &params, const std::string &message);//for JOIN



        Client* findClientByNick(const std::string &nick);


        static void SignalHandler(int sig);

        void ClearClient(int fd);
        void CloseFds();
        void sendDisconnectMessage(int fd);

        //removwas all clients and deletes empty channels
        void removeFromAllChannels(int fd, const std::string &leaveMessage = "");


        //for JOIN
        Client* findClientByFd(int fd);
        Channel* findChannel(const std::string &name);
        void broadcastToChannel(Channel &channel, const std::string &message, int excludeFd = -1);
        void handleJoin(Client &client, std::vector<std::string> args);
        void handlePart(Client &client, std::vector<std::string> args); //PART


    private:
        int Port;
        std::string Password;
        int ServerSocketFd;
        static bool Signal;
        std::vector<Client> clients;
        std::vector<pollfd> fds;

        typedef void (Server::*CommandHandler)(Client &client, std::vector<std::string> args);
        std::map<std::string, CommandHandler> _commandMap;


        //for JOIN
        std::map<std::string, Channel> _channels;
    
};


#endif
