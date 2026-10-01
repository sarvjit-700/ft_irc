#include "Server.hpp"

Client* Server::findClientByFd(int fd){
    for (size_t i = 0; i < clients.size(); i++){
        if (clients[i].getFD() == fd)
            return &clients[i];
    }
    return NULL;
}

Channel* Server::findChannel(const std::string &name){
    std::map<std::string, Channel>::iterator it = _channels.find(name);
    if(it == _channels.end())   
        return NULL;
    return &it->second;
}

void Server::broadcastToChannel(Channel &channel, const std::string &message, int excludeFd){
    const std::set<int> &members = channel.getMembers();
    for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it){
        if (*it == excludeFd)
            continue;
        Client *c = findClientByFd(*it);
        if(c)
            c->appendOutput(message);
    }
}