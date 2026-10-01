#include "Reply.hpp"
#include "Client.hpp"

void sendReply(Client& client, const Reply& reply, const std::vector<std::string>& params)
{
    std::string response;
    std::string targetNick;

    response += ":ircserv ";
    response += reply.code; 
    targetNick = client.getNick();
    if (targetNick.empty())
        targetNick = "*";
    response += " " + targetNick;    

    for (std::vector<std::string>::const_iterator it = params.begin(); it != params.end(); ++it)
    {    
        response += " " + *it;
    }

    if (reply.message[0] != '\0')
        response += " " + std::string(reply.message);
    response += "\r\n";
    client.appendOutput(response);
}

void sendReply(Client& client, const Reply& reply)
{
    std::vector<std::string> emptyParams;
    sendReply(client, reply, emptyParams);
}

// Wrapper for 1 parameter (e.g., 461 ERR_NEEDMOREPARAMS, 403 ERR_NOSUCHCHANNEL)
void sendReply(Client& client, const Reply& reply, const std::string& param1)
{
    std::vector<std::string> params;
    params.push_back(param1);
    sendReply(client, reply, params);
}

// Wrapper for 2 parameters (e.g., 441 ERR_USERNOTINCHANNEL)
void sendReply(Client& client, const Reply& reply, const std::string& param1, const std::string& param2)
{
    std::vector<std::string> params;
    params.push_back(param1);
    params.push_back(param2);
    sendReply(client, reply, params);
}