#include "Server.hpp"

bool Server::Signal = false;

Server::Server()
{
    ServerSocketFd = -1;
    _commandMap["PASS"] = &Server::handlePass;
    _commandMap["NICK"] = &Server::handleNick;
    _commandMap["USER"] = &Server::handleUser;
    _commandMap["PRIVMSG"] = &Server::handlePrivmsg;
    _commandMap["QUIT"] = &Server::handleQuit;
    _commandMap["PING"] = &Server::handlePing;
    _commandMap["PONG"] = &Server::handlePong;
    _commandMap["JOIN"] = &Server::handleJoin;
    _commandMap["PART"] = &Server::handlePart;
    _commandMap["TOPIC"] = &Server::handleTopic;
    _commandMap["WHOIS"] = &Server::handleWhois;
    _commandMap["CAP"] = &Server::handleCap;
    _commandMap["KICK"] = &Server::handleKick;
    _commandMap["INVITE"] = &Server::handleInvite;
    _commandMap["MODE"] = &Server::handleMode; // set to stop IRSSI error
    _commandMap["WHO"] = &Server::handleWho;

}

Server::~Server(){
}

void Server::setPort(int user_port){
    this->Port = user_port;
}


void Server::SignalHandler(int sig){
    (void) sig;
    Server::Signal = true;
}

void Server::setPassword(std::string user_pass)
{
    this->Password = user_pass;
}




//CLEAR

//delete all clients if server closes
void Server::CloseFds(){
    for(size_t i = 0; i<clients.size();i++){
        if(clients[i].getFD() != -1)
            close(clients[i].getFD());
    }
        
    
    if(ServerSocketFd != -1){
        close(ServerSocketFd);
        ServerSocketFd = -1;
    }
    
}

//say "bye" to client
void Server::sendDisconnectMessage(int fd)
{
    std::string nick = "unknown";
    for (size_t i = 0; i < clients.size(); i++){
        if (clients[i].getFD() == fd){
            nick = clients[i].getNick().empty() ? "unknown" : clients[i].getNick();
            clients[i].appendOutput("Goodbye, " + nick + "!\r\n");
            clients[i].markForDisconnect();
            std::cout << "Client[" << clients[i].getFD() << "]: " YELLOW << clients[i].getNick() 
                    << " left " << RESET << std::endl;
            break;
        }
    }

    
    //ClearClient(fd);
}

//when client is leaving
void Server::ClearClient(int fd){

    Client *client = findClientByFd(fd);
    std::string quitMsg;
    if (client && !client->getNick().empty())
        quitMsg = ":" + client->getHostmask() + " QUIT :Connection closed\r\n";

    removeFromAllChannels(fd, quitMsg);

    close (fd);
    for(std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it){
        if(it->getFD() == fd){
            clients.erase(it);
            // std::cout << "Client is removed " << fd << std::endl;
            break;
        }
    }
        
    for(std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); ++it){
        if(it->fd == fd){
            fds.erase(it);
            // std::cout << "Poll is removed " << fd << std::endl;
            break;
        }
    }
}





Client* Server::findClientByNick(const std::string &nick){
    for (size_t i = 0; i < clients.size(); i++){
        if(clients[i].getNick() == nick)
            return &clients[i];
    }
    return NULL;
}









//CLIENT'S HANDLING
void Server::AcceptClient(){

    struct sockaddr_in ClientAddress;
    struct pollfd ClientPoll;
    socklen_t len = sizeof(ClientAddress);

    int ClientFd = accept(ServerSocketFd, (struct sockaddr*)&(ClientAddress), &len);
    if(ClientFd == -1){
        (std::cout << "Client can't be accepted");
        return;
    }


    if(fcntl(ClientFd, F_SETFL, O_NONBLOCK) == -1){
        std::cout << "fcnts failed" << std::endl;
        close (ClientFd);
        return;
    }

    Client NewClient(ClientFd);
    std::string ip = inet_ntoa(ClientAddress.sin_addr);
    NewClient.setIP(ip);
    // std::cout << "client : " << NewClient.getFD() << " ip : " << NewClient.getIP() << std::endl;


    
    ClientPoll.events = POLLIN | POLLOUT; // ADDED POLLOUT
    ClientPoll.fd = ClientFd;
    ClientPoll.revents = 0;

    clients.push_back(NewClient);
    fds.push_back(ClientPoll);
    clients.back().appendOutput(":ircserv NOTICE * :Welcome to ft_irc!\r\n");
    clients.back().appendOutput(":ircserv NOTICE * :Enter PASSWORD, NICK and USERNAME\r\n");
    std::cout << "Client[" << ClientFd << "]: " YELLOW << "has connected " << RESET << std::endl;
}



std::vector<std::string>    Server::SplitCommand(std::string command)
{
    std::vector<std::string> args;

    for (size_t i = 0; i < command.length(); i++)
    {
        if (command[i] == ' ')
            continue;
        if (command[i] == ':')
        {
            args.push_back(command.substr(i + 1));
            break;
        }
        size_t end = command.find(' ', i);
        if (end == std::string::npos)
        {
            args.push_back(command.substr(i));
            break;
        }
        else
        {
            args.push_back(command.substr(i, end - i));
            i = end;
        }
    }
    std::cout << BLUE << "Received => ";
    for (size_t i = 0; i < args.size(); i++)
    {
        std::cout << args[i] << " ";
    }
    std::cout << RESET << std::endl;
    return args;
}


void Server::ParseCommand(Client &client, std::vector<std::string> args)
{

    if (args.empty())
        return;
    std::string command = args[0];
    if (_commandMap.find(command) != _commandMap.end())
        (this->*_commandMap[command])(client, args);
    else{
        std::string nick = client.getNick().empty() ? "*" : client.getNick();
        client.appendOutput(":ircserv 421 " + nick + " " + command + " :Unknown command\r\n");
    }
}

bool Server::ReceiveData(int fd){
    char buffer[1024];
    int bytesNumber = recv(fd, buffer, sizeof(buffer) - 1, 0);

    if (bytesNumber <= 0){
        std::cout << "Client[" << fd << "]: " YELLOW << "has disconnected " << RESET << std::endl;
        ClearClient(fd);
        return false;
    }

    buffer[bytesNumber] = '\0';
    for (size_t i = 0; i < clients.size(); i++){
        if(clients[i].getFD() == fd){
            
            clients[i].appendBuffer(buffer);

            std::string command;
            while ((command = clients[i].extractCommand()) != ""){
                std::vector<std::string> args = SplitCommand(command);
                if (args.empty()){
                    std::cout << "empty args!" << std::endl;
                    continue;
                }
                ParseCommand(clients[i], args);
            }
            break;
        }
    }
    return true;
}

bool Server::SendData(int fd)
{
// Find the correct client
    for (size_t j = 0; j < clients.size(); j++) {
        if (clients[j].getFD() == fd) {
            
            std::string outData = clients[j].getOutputBuffer();
            if (!outData.empty()) {
                // Attempt to send the queued data
                int bytesSent = send(fd, outData.c_str(), outData.length(), 0);
                
                if (bytesSent > 0) {
                    // Safely remove only what was actually transmitted
                    clients[j].eraseOutput(bytesSent);
                }
            }
            if (clients[j].getOutputBuffer().empty() && clients[j].shouldDisconnect()) {
                ClearClient(fd);
                return false;
            }
            break;
        }
    }
    return true;
}




//SERVER HANDLING
void Server::ServerInit() {

    CreateSocket();

    std::cout << YELLOW "Server is connected" RESET << std::endl;

    while (Server::Signal == false) {

        for (size_t i = 0; i < clients.size(); i++) {
            for (size_t j = 0; j < fds.size(); j++) {
                if (fds[j].fd == clients[i].getFD()) {
                    fds[j].events = POLLIN;
                    if (!clients[i].getOutputBuffer().empty())
                        fds[j].events |= POLLOUT;
                    break;
                }
            }
        }        



        int pollResult = poll(&fds[0], fds.size(), -1);
        if (pollResult == -1) {
            if (Server::Signal == true)
                break;
            throw (std::runtime_error("Poll failed"));
        }

        for (size_t i = 0; i < fds.size(); i++) {
            
            // 1. Error handling
            if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                sendDisconnectMessage(fds[i].fd);
                ClearClient(fds[i].fd);
                i--;
                continue;
            }

            // 2. Incoming data
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == ServerSocketFd) {
                    AcceptClient();
                } else {
                    if (ReceiveData(fds[i].fd) == false) {
                        i--;
                        continue;
                    }
                }
            }
            // 3. Outgoing data
            if (fds[i].revents & POLLOUT) {
                if (fds[i].fd != ServerSocketFd) {
                    if (SendData(fds[i].fd) == false) {
                    i--;
                    continue;
                    }
                }
            }
        }
    }
    CloseFds();
}


void Server::CreateSocket(){

    //create socket file descriptor
    //int socket(int domain, int type, int protocol);
    //SOCK_STREAM type for TCP sockets
    // 0 means that system will decide which protocol to use by itself
    ServerSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if(ServerSocketFd<=-1)
        throw(std::runtime_error ("Socket can't be created"));


    struct sockaddr_in SocketAddress;
    SocketAddress.sin_family = AF_INET;
    SocketAddress.sin_port = htons(this->Port);
    SocketAddress.sin_addr.s_addr = INADDR_ANY;

    int enable = 1;
    //setting options to a socket - ALLOW REUSE right after restart server
    //int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
    //SOL_SOCKET level means that the option is aplied to the socket itself
    //SO_REUSEADDR - allows socket for immediate reuse of local addresses and ports
    if(setsockopt(ServerSocketFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
        throw(std::runtime_error("Socket can't be setted(SO_REUSEADDR option failed)"));

    //sets the socket to non-blocking mode
    //it means that server will run through all clients and check if they want to share some data
    if(fcntl(ServerSocketFd, F_SETFL, O_NONBLOCK) == -1)
        throw(std::runtime_error("Socket can't be setted(O_NONBLOCK option failed)"));

    //assigning socket to address and port
    if(bind(ServerSocketFd, (struct sockaddr*)&SocketAddress, sizeof(SocketAddress))== -1)
        throw(std::runtime_error("Socket can't be binded"));

    //int listen(int sockfd, int backlog);
    //backlog: The maximum number of connection request
    if(listen(ServerSocketFd, SOMAXCONN) == -1)
        throw (std::runtime_error("Socket can't listen"));


    struct pollfd IrcPoll;
    //IrcPoll - is an object, which system will follow
    IrcPoll.fd = ServerSocketFd; // "system, follow that socket!""
    IrcPoll.events = POLLIN; // "tell me when you'll get some data to read"
    IrcPoll.revents = 0;// "for now, nothing has happend"
    fds.push_back(IrcPoll);//add the folowing object to fds list, which I'll give to poll
}


void Server::sendReplyThree(Client &client, const std::string &code, const std::string &message)
{
    std::string nick = client.getNick().empty() ? "*" : client.getNick();
    // client.appendOutput(":ircserv " RED + code + RESET + " " + nick + " :" + message + "\r\n");
    client.appendOutput(":ircserv "+ code + " " + nick + " :" + message + "\r\n");
}


void Server::sendReplyFour(Client &client, const std::string &code, const std::string &params, const std::string &message)
{
    std::string nick = client.getNick().empty() ? "*" : client.getNick();
    std::string line = ":ircserv " + code + " " + nick;
    if (!params.empty())
        line += " " + params;
    line += " :" + message + "\r\n";
    // client.appendOutput(":ircserv " RED + code + RESET + " " + nick + " :" + message + "\r\n");
    client.appendOutput(line);
}



void Server::removeFromAllChannels(int fd, const std::string &leaveMessage)
{
    for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); )
    {
        Channel &channel = it->second;

        if (channel.isMember(fd))
        {
            if (!leaveMessage.empty())
                broadcastToChannel(channel, leaveMessage, fd);
            channel.removeMember(fd);
            channel.promoteNewOperator();      
        }

        if (channel.getMembers().empty())
        {
            std::string chanName = it->first;
            std::map<std::string, Channel>::iterator toErase = it;
            ++it;

            _channels.erase(toErase);
            std::cout << YELLOW << "Channel " << chanName << " deleted (empty)" RESET << std::endl;

            continue;
        }
        ++it;
    }
}