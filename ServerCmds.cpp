#include "Server.hpp"

void Server::handlePass(Client &client, std::vector<std::string> args)
{   
    //once client did the registration, he should not try to put password once more time
    if (client.isRegistered())
    {
        sendReply(client, ERR_ALREADYREGISTRED);
        return;
    }

    if (args.size() < 2 || args[1].empty())
    {
        sendReply(client, ERR_NEEDMOREPARAMS);
        return;
    }
    //checking if password of the client is matching with password of the server
    if (args[1] == this->Password)
    {
        client.setPassOK();
        client.appendOutput(":ircserv NOTICE * :Password accepted!\r\n");
        std::cout << "Client[" << client.getFD() << "]: " GREEN 
                << "provided the correct password" RESET << std::endl;
    }
    else
    {
        sendReply(client, ERR_PASSWDMISMATCH);
        std::cout << "Client[" << client.getFD() << "]: " RED 
                << "provided the incorrect password" RESET << std::endl;
    }
}


void Server::handleNick(Client &client, std::vector<std::string> args)
{
    //checking if the client has set up password first
    if (!client.getPassOk())
    {
        sendReply(client, ERR_PASSWDMISMATCH);
        return;
    }
    if (args.size() < 2 || args[1].empty())
    {
        sendReply(client, ERR_NONICKNAMEGIVEN);
        return;
    }
    
    std::string newNick = args[1];
    if (newNick.find_first_of(" *!@?.,") != std::string::npos
        || newNick[0] == '$' || newNick[0] == ':' || newNick[0] == '#')
    {
        sendReply(client, ERR_ERRONEUSNICKNAME); 
        return;
    }
    for (size_t k = 0; k < clients.size(); k++)
    {
        if (clients[k].getFD() != client.getFD() 
            && clients[k].getNick() == newNick)
        {
            sendReply(client, ERR_NICKNAMEINUSE, newNick);
            return;
        }
    }
    bool wasRegistered = client.isRegistered();
    std::string oldNick = client.getNick();

    client.setNick(newNick);
    std::cout << "Client[" << client.getFD() << "]: set / updated NICK to: " << GREEN 
            << client.getNick() << RESET << std::endl;
    
    if (!oldNick.empty())
    {
        client.appendOutput(":" + oldNick + " NICK " + newNick + "\r\n");
    }
    if (client.isRegistered() && !wasRegistered)
    {
        std::string welcome = ":ircserv 001 " + client.getNick() + RPL_WELCOME + client.getHostmask() + "\r\n";
        client.appendOutput(welcome);
        std::cout << "Client[" << client.getFD() << "]: " GREEN << client.getNick() 
                << " is fully registered" RESET << std::endl;
    }
}


//Parameters: <user> <mode> <unused> <realname>
//example:  USER guest 0 * :Ronnie Reagan
//args[0] = USER, args[1] = quest , args[2] = number, args[3] = * , args[4] = :Real name
void Server::handleUser( Client &client, std::vector <std::string> args)
{
    if (!client.getPassOk())
    {
        sendReply(client, ERR_PASSWDMISMATCH);
        return;
    }

    if (!client.getUser().empty()) // changed this, was client.isRegistered() 
    {
        sendReply(client, ERR_ALREADYREGISTRED);
        return;
    }

    if (args.size() < 5 || args[1].empty() || args[2].empty() || args[3].empty() || args[4].empty())
    {
        sendReply(client, ERR_NEEDMOREPARAMS);
        return;
    }

    // for(std::string::size_type i = 0; i < args[2].length(); i++)
    // {
    //     if(!std::isdigit(args[2][i]))
    //     {
    //         sendReplyThree(client, "461", "USER :Second argument must be numeric");
    //         return;
    //     }
    // }

    std::string newUser = "~" + args[1];
    client.setUser(newUser);
    client.setRealName(args[4]);
    std::cout << "Client[" << client.getFD() << "]: set USER to: " << GREEN 
            << client.getRealName() << RESET << std::endl;
    if (client.isRegistered())
    {
        std::string welcome = ":ircserv 001 " + client.getNick() + RPL_WELCOME + client.getHostmask() + "\r\n";
        client.appendOutput(welcome);
        std::cout << "Client[" << client.getFD() << "]: " GREEN << client.getNick() 
                << " is fully registered" RESET << std::endl;
    }
}


void Server::handlePrivmsg( Client &client, std::vector <std::string> args){
    // args[0] = "PRIVMSG", args[1] = target, args[2] = message text
    if (!client.isRegistered()) 
    {
        sendReply(client, ERR_NOTREGISTERED);
        return;
    }

    if (args.size() < 2){
        sendReply(client, ERR_NORECIPIENT, "PRIVMSG");
        return;
    }

    if (args.size() < 3){
        sendReply(client, ERR_NOTEXTTOSEND);
        return;
    }

    std::string target = args[1];
    std::string message = args[2];
    if(target[0] == '#'){
        Channel *channel = findChannel(target);
        if(!channel){
            sendReply(client, ERR_NOSUCHCHANNEL, target);
            return;
        }

        if (!channel->isMember(client.getFD())){
            sendReply(client, ERR_CANNOTSENDTOCHAN, target);
            return;
        }


        std::string theMessage = ":" + client.getHostmask() + " PRIVMSG " + target + " :" + message + "\r\n";
        broadcastToChannel(*channel, theMessage, client.getFD());
        return;
    }

    Client *targetClient = findClientByNick(target);
    if (!targetClient){
        sendReply(client, ERR_NOSUCHNICK, target);
        return;
    }
    std::string fullMessage = ":" + client.getHostmask() + " PRIVMSG " + target + " :" + message + "\r\n";
    targetClient->appendOutput(fullMessage);
    std::cout << "Client[" << client.getFD() << "]: " GREEN << client.getNick() 
                << " sent a msg to " << targetClient->getNick() << RESET << std::endl;
}


void Server::handleQuit(Client &client, std::vector <std::string> args){
    std::string reason = "Client Quit";
    if (args.size() > 1 && !args[1].empty())
        reason = args[1];


    std::string quitMsg = ":" + client.getHostmask() + " QUIT :" + reason + "\r\n";
    removeFromAllChannels(client.getFD(), quitMsg);

    //send(client.getFD(), "Goodbye!", 9, 0);
    // std::cout << "Client " << client.getNick() << " left(QUIT)" << std::endl; 
    sendDisconnectMessage(client.getFD());
}


//PING PONG
void Server::handlePing( Client &client, std::vector <std::string> args){
    if (!client.isRegistered())
    {
        sendReply(client, ERR_NOTREGISTERED);
        return;
    }

    if (args.size() < 2 || args[1].empty())
    {
        sendReply(client, ERR_NEEDMOREPARAMS, "PING");
        return;
    }
    
    // std::string token;
    // if (args.size() > 1)
    //     token = args[1];
    // else
    //     token = "";
    std::string reply = "PONG :" + args[1] + "\r\n";
    client.appendOutput(reply);
}

//pong looks like that because by now we need only 
void Server::handlePong( Client &client, std::vector <std::string> args){
    (void) client;
    (void) args;
}



void Server::handleJoin(Client &client, std::vector<std::string> args){



    if (!client.isRegistered()){
        sendReply(client, ERR_NOTREGISTERED);
        return;
    }

    if(args.size() < 2 || args[1].empty()){
        sendReply(client, ERR_NEEDMOREPARAMS);
        return;
    }

    std::vector<std::string> channelNames;
    std::string channelArgs = args[1];
    size_t start = 0;
    size_t comma;
    while ((comma = channelArgs.find(',', start)) != std::string::npos){
        channelNames.push_back(channelArgs.substr(start, comma-start));
        start = comma + 1;
    }
    channelNames.push_back(channelArgs.substr(start));

    std::vector<std::string> keys;
    if (args.size() > 2)
    {
        std::string keyArgs = args[2];
        size_t kstart = 0;
        size_t kcomma;
        while ((kcomma = keyArgs.find(',', kstart)) != std::string::npos){
            keys.push_back(keyArgs.substr(kstart, kcomma - kstart));
            kstart = kcomma + 1;
        }
        keys.push_back(keyArgs.substr(kstart));
    }






    for(size_t i = 0; i < channelNames.size(); i++){
        std::string chanName = channelNames[i];
        if(chanName.empty() || (chanName[0] != '#' && chanName[0] != '&') || chanName.find_first_of(" ,\a") != std::string::npos){
            sendReply(client, ERR_NOSUCHCHANNEL, chanName);
            continue;
        }
    
        Channel *channel = findChannel(chanName);
        bool isNewChannel = false;
        if (!channel){
            _channels.insert(std::make_pair(chanName, Channel(chanName)));
            channel = findChannel(chanName);
            channel->addOperator(client.getFD()); //creator is operator
            isNewChannel = true;
            std::cout << YELLOW "Channel " << chanName << " created by " << client.getNick() << RESET << std::endl;
        }

        if (channel->isMember(client.getFD()))
            continue;


        if (!isNewChannel)
        {
            if (channel->isInviteOnly() && !channel->isInvited(client.getNick()))
            {
                sendReply(client, ERR_INVITEONLYCHAN, chanName);
                continue;
            }
            if (channel->hasKey())
            {
                std::string providedKey = (i < keys.size()) ? keys[i] : "";
                if (providedKey != channel->getKey())
                {
                    sendReply(client, ERR_BADCHANNELKEY, chanName);
                    continue;
                }
            }
            if (channel->hasLimit() && channel->getMembers().size() >= (size_t)channel->getUserLimit())
            {
                sendReply(client, ERR_CHANNELISFULL, chanName);
                continue;
            }
        }




        channel->addMember(client.getFD());

        if (channel->isInvited(client.getNick()))
            channel->removeInvited(client.getNick());

        std::string joinMessage = ":" + client.getHostmask() + " JOIN :" + chanName + "\r\n";
        broadcastToChannel(*channel, joinMessage);

        if(channel->getTopic().empty())
            sendReply(client, RPL_NOTOPIC, chanName);
        else
            sendReply(client, RPL_TOPIC, chanName, channel->getTopic());

        std::string names;
        const std::set<int> &members = channel->getMembers();
        bool first = true;

        for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it){
            Client *member = findClientByFd(*it);
            if(!member)
                continue;
            if (!first)
                names += " ";
            if(channel->isOperator(*it))
                names+= "@";
            names += member->getNick();
            first = false;
        }
        sendReply(client, RPL_NAMREPLY, "= " + chanName, ":" + names);
        sendReply(client, RPL_ENDOFNAMES, chanName);
        std::cout << "Client[" << client.getFD() << "]: " GREEN << client.getNick() 
                << " joined " << chanName << RESET << std::endl;
    }
}


void Server::handlePart(Client &client, std::vector<std::string> args)
{
    if (!client.isRegistered())
    {
        sendReply(client, ERR_NOTREGISTERED);
        return;
    }
    if (args.size() < 2 || args[1].empty())
    {
        sendReply(client, ERR_NEEDMOREPARAMS);
        return;
    }

    std::string reason = "Leaving";
    if (args.size() > 2)
    {
        reason = args[2];
    }

    std::vector<std::string> channelNames;
    std::string channelArgs = args[1];
    size_t start = 0;
    size_t comma;
    while ((comma = channelArgs.find(',', start)) != std::string::npos)
    {
        channelNames.push_back(channelArgs.substr(start, comma - start));
        start = comma + 1;
    }
        channelNames.push_back(channelArgs.substr(start));

    for (size_t i = 0; i < channelNames.size(); i++)
    {
        std::string chanName = channelNames[i];
        Channel *channel = findChannel(chanName);
        
        if (!channel)
        {
            sendReply(client, ERR_NOSUCHCHANNEL, chanName);
            continue;
        } 
        if (!channel->isMember(client.getFD()))
        {
            sendReply(client, ERR_NOTONCHANNEL, chanName);
            continue;
        }

        std::string partMsg = ":" + client.getHostmask() + " PART " + chanName + " :" + reason + "\r\n";
        broadcastToChannel(*channel, partMsg);

        channel->removeMember(client.getFD());
        channel->promoteNewOperator(); 
        std::cout << "Client[" << client.getFD() << "]: " GREEN << client.getNick() 
                << " left " << chanName << RESET << std::endl;

        if (channel->getMembers().empty())
        {
            _channels.erase(chanName);
            std::cout << YELLOW << "Channel " << chanName << " deleted (empty)" RESET << std::endl;
        }
    }
}

//might be good to add method getPrefix()
// modes +t(channel is locked/restricted) and -t(channel not restricted, everyone can change topic) 
void Server::handleTopic(Client &client,std::vector<std::string> args)
{
    // 1)checking if there is TOPIC and #channel
    if (args.size() < 2 || args[1].empty())
    {
        sendReply(client, ERR_NEEDMOREPARAMS, "TOPIC");
        return;
    }

    if (!client.isRegistered())
    {
        sendReply(client, ERR_NOTREGISTERED);
        return;
    }

    const std::string &channelName = args[1];


    Channel *channel = findChannel(channelName);
    if (!channel)
    {
        sendReply(client, ERR_NOSUCHCHANNEL, channelName);
        return;
    }

    if (!channel->isMember(client.getFD()))
    {
        sendReply(client, ERR_NOTONCHANNEL, channelName);
        return;
    }
    //2)for reading topic: TOPIC #channel
    if(args.size() == 2)
    {
        if (channel->getTopic().empty())
        {
            sendReply(client, RPL_NOTOPIC, channelName);
        }
        else
        {
            sendReply(client, RPL_TOPIC, channelName, channel->getTopic());
        }
        return;
    }

    //3)TOPIC #channel :text
    if(args.size() >= 3)
    //this one is for changing MODE +t
    {
        //channel is restricted
        if (channel->isTopicRestricted())
        {
            if (channel->isOperator(client.getFD()))
            {
                std::string newTopic = args[2];
                channel->setTopic(newTopic);
                std::string msg = ":" + client.getHostmask() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
                broadcastToChannel(*channel, msg);
                std::cout << "Client[" << client.getFD() << "]: " GREEN << client.getNick() 
                        << " changed " << channelName << " TOPIC to:" << newTopic << RESET << std::endl;
            }
            else
            {
                sendReply(client, ERR_CHANOPRIVSNEEDED, channelName);
                return;
            }
        }
        //channel is not restricted -t
        else
        {
            std::string newTopic = args[2];
            channel->setTopic(newTopic);
            std::string msg = ":" + client.getHostmask() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
            broadcastToChannel(*channel, msg);
            std::cout << "Client[" << client.getFD() << "]: " GREEN << client.getNick() 
                    << " changed " << channelName << " TOPIC to: " << newTopic << RESET << std::endl;
        }
    }
}


void Server::handleWhois(Client &client, std::vector<std::string> args){
    if(args.size()<2)
    {
        sendReply(client, ERR_NONICKNAMEGIVEN);
        return;
    }

    if (!client.isRegistered())
    {
        sendReply(client, ERR_NOTREGISTERED);
        return;
    }

    std::string targetNick = args[1];
    Client *target = findClientByNick(targetNick);

    if(!target){
        sendReply(client, ERR_NOSUCHNICK, targetNick);
        return;
    }

    //format in irc standart:
    //311 <nick> <username> <host> * :<realname>
    std::string params = target->getNick() + " " + target->getUser() + " " + target->getIP() + " *";
    // std::string message = target->getIP() + " *";


    sendReply(client, RPL_WHOISUSER, params, ":" + target->getRealName());
}


void Server::handleCap(Client &client, std::vector<std::string> args)
{
    if (args.size() < 2)
        return;
    

    std::string sub = args[1];
    if (sub=="LS")
        client.appendOutput(":ircserv CAP * LS :\r\n");
    
    else if(sub=="REQ"){
        std::string request = args.size() > 2 ? args[2] : "";
        client.appendOutput(":ircserv CAP * NAK :" + request + "\r\n");
    }
    else if (sub=="END"){
        
    }
    
}

void Server::handleKick(Client &client, std::vector <std::string> args)
{
    if (!client.isRegistered())
    {
        sendReply(client, ERR_NOTREGISTERED);
        return;
    }

    if (args.size() < 3)
    {
        sendReply(client, ERR_NEEDMOREPARAMS, "KICK");
        return;
    }

    std::string channelName = args[1];
    std::string targetNick = args[2];
    std::string reason = "Kicked by " +client.getNick();

    // Extract custom reason if provided
    if (args.size() > 3)
    {
        reason = args[3];
    }

    Channel *channel = findChannel(channelName);
    if (!channel)
    {
        sendReply(client, ERR_NOSUCHCHANNEL, channelName);
        return;
    }

    if(!channel->isMember(client.getFD()))
    {
        sendReply(client, ERR_NOTONCHANNEL, channelName);
        return;
    }

    if(!channel->isOperator(client.getFD()))
    {
        sendReply(client, ERR_CHANOPRIVSNEEDED, channelName);
        return;
    }

    Client *targetClient = findClientByNick(targetNick);
    if (!targetClient)
    {
        sendReply(client, ERR_NOSUCHNICK, targetNick);
        return;
    }

    if (!channel->isMember(targetClient->getFD()))
    {
        sendReply(client, ERR_USERNOTINCHANNEL, targetNick, channelName);
        return;
    }

    std::string kickMsg = ":" + client.getHostmask() + " KICK " + channelName + " " 
            + targetNick + " :" + reason + "\r\n";
    
    broadcastToChannel(*channel, kickMsg);
    channel->removeMember(targetClient->getFD());
    channel->promoteNewOperator();
    std::cout << "Client[" << targetClient->getFD() << "]: " YELLOW << targetClient->getNick() 
                    << " was kicked from " << channelName << RESET << std::endl;
    if (channel->getMembers().empty())
    {
        _channels.erase(channelName);
        std::cout << YELLOW << "Channel " << channelName << " deleted (empty)" RESET << std::endl;
    }
}

void Server::handleInvite(Client &client, std::vector <std::string> args)
{
    if (!client.isRegistered())
    {
        sendReply(client, ERR_NOTREGISTERED);
        return;
    }

    if (args.size() < 3)
    {
        sendReply(client, ERR_NEEDMOREPARAMS, "INVITE");
        return;
    }

    std::string targetNick = args[1];
    std::string chanName = args[2];

    Client *targetClient = findClientByNick(targetNick);
    if (!targetClient)
    {
        sendReply(client, ERR_NOSUCHNICK, targetNick);
        return;
    }

    Channel *channel = findChannel(chanName);
    if (!channel)
    {
        sendReply(client, ERR_NOTONCHANNEL, chanName);
        return;
    }

    if (!channel->isMember(client.getFD()))
    {
        sendReply(client, ERR_NOTONCHANNEL, chanName);
        return;
    }

    if (!channel->isOperator(client.getFD()))
    {
        sendReply(client, ERR_CHANOPRIVSNEEDED, chanName);
        return;
    }

    if (channel->isMember(targetClient->getFD()))
    {
        sendReply(client, ERR_USERONCHANNEL, targetNick, chanName);
        return;
    }

    channel->inviteUser(targetNick);
    sendReply(client, RPL_INVITING, targetNick, chanName);
    std::string inviteMsg = ":" + client.getHostmask() + " INVITE " + targetNick + " :" + chanName + "\r\n";
    targetClient->appendOutput(inviteMsg);
    std::cout << "Client[" << client.getFD() << "]: " YELLOW << client.getNick() << " invited " 
            << targetNick << " to " << chanName << RESET << std::endl;
}

//MODE
static std::string toString(int n){
    std::ostringstream strm;
    strm << n;
    return strm.str();
}



//i - invite-only
//t - topic restriction
//k - channel key
//o - operator privilege
//l - user limit
void Server::handleMode(Client &client, std::vector <std::string> args)
{
    if (!client.isRegistered()){
        sendReply(client, ERR_NOTREGISTERED);
        return;
    }

    //at least 2 elements ("MODE #general", "MODE #general +i", for example)
    if (args.size() < 2){
        sendReply(client, ERR_NEEDMOREPARAMS, "MODE");
        return;
    }

    std::string target = args[1];

//clients also has modes. and they don't start from # or &
//without that check server will think that client is a channel
    if(target[0]!= '#' && target[0] != '&'){
        if (target == client.getNick()){
            // std::string line = ":ircserv 221 " + client.getNick() + "\r\n";
            // client.appendOutput(line);
            return;
        }
        return;
    }


    // if(target.empty() || (target[0] != '#' && target[0] != '&')){
    //     sendReply(client, ERR_NOSUCHCHANNEL, target);
    //     return;
    // }

    Channel *channel = findChannel(target);
    if(!channel){
        sendReply(client, ERR_NOSUCHCHANNEL, target);
        return;
    }

    //serching for current channel's modes
    if (args.size() == 2){
        std::string modes = "+";
        std::string params;

        if(channel->isInviteOnly())
            modes += "i";
        if(channel->isTopicRestricted())
            modes += "t";
        if(channel->hasKey()){
            modes += "k";
            params += " " + channel->getKey();
        }
        if (channel->hasLimit()){
            modes+= "l";
            params+= " " + toString(channel->getUserLimit());
        }


        std::string line = ":ircserv 324 " + client.getNick() + " " + target + " " + modes + params + "\r\n";
        client.appendOutput(line);
        return;
    }

//client should be a member of the channel
    if (!channel->isMember(client.getFD()))
    {
        sendReply(client, ERR_NOTONCHANNEL, target);
        return;
    }
//irrsi checks for ban list - ignore
    if (args.size() == 3 && args[2] == "b") 
        return;

//client should also be an operator of the channel
    if (!channel->isOperator(client.getFD()))
    {
        sendReply(client, ERR_CHANOPRIVSNEEDED, target);
        return;
    }

    std::string modeString = args[2]; //string with modes
    size_t paramIndex = 3; //'pointer' to special paranetres for some modes
    bool adding = true; // are we adding or removing mode to the channel?

    std::string appliedModes; //collecting all channel's modes
    std::string appliedParams;//collecting mode's parameters


    for (size_t i = 0; i < modeString.size(); i++){
        char c = modeString[i];

        if (c=='+'){
            adding = true;
            continue;
        }

        if(c=='-'){
            adding = false;
            continue;
        }

        switch(c){
            case 'i':{
                channel->setInviteOnly(adding);
                appliedModes += adding ? "+i" : "-i";
                break;
            }

            case 't':{
                channel->setTopicRestricted(adding);
                appliedModes += adding ? "+t" : "-t";
                break;
            }

            case 'k':{
                if (adding){
                    if(paramIndex>= args.size()){
                        sendReply(client, ERR_NEEDMOREPARAMS, "MODE");
                        break;
                    }
                    std::string key = args[paramIndex++];
                    channel->setKey(key);
                    appliedModes += "+k";
                    appliedParams += " " + key;
                }
                else{
                    channel->removeKey();
                    appliedModes += "-k";
                }
                break;
            }
            
            case 'l':{
                if (adding){
                    if (paramIndex >= args.size()){
                        sendReply(client, ERR_NEEDMOREPARAMS, "MODE");
                        break;
                    }
                    int limit = std::atoi(args[paramIndex].c_str());
                    if (limit <= 0){
                        paramIndex++;
                        break;
                    }
                    channel->setUserLimit(limit);
                    appliedModes += "+l";
                    appliedParams += " " + args[paramIndex];
                    paramIndex ++;
                }

                else{
                    channel->removeUserLimit();
                    appliedModes += "-l";
                }
                break;
            }

            case 'o':{
                if(paramIndex >= args.size()){
                    sendReply(client, ERR_NEEDMOREPARAMS, "MODE");
                    break;
                }

                std::string targetNick = args[paramIndex++];
                Client *targetClient = findClientByNick(targetNick);
                if (!targetClient){
                    sendReply(client, ERR_NOSUCHNICK, targetNick);
                    break;
                }

                if (!channel->isMember(targetClient->getFD())){
                    sendReply(client, ERR_USERNOTINCHANNEL, targetNick, target);
                    break;
                }

                if (adding)
                    channel->addOperator(targetClient->getFD());
                else
                    channel->removeOperator(targetClient->getFD());
                
                appliedModes += adding ? "+o" : "-o";
                appliedParams += " " + targetNick;
                break;
            }

            case 'b': {
                if (paramIndex < args.size()) {
                    paramIndex++;
                }
                break;
            }

            //any other letters will be considered as wrong
            default:{
                std::string line = ":ircserv 472 " + client.getNick() + " " + std::string(1, c) + " :is unknown mode char\r\n";
                client.appendOutput(line);
                break;
            }

        }
    }

    if(!appliedModes.empty()){
        std::string message = ":" + client.getHostmask() + " MODE " + target + " " + appliedModes + appliedParams + "\r\n";
        broadcastToChannel(*channel, message);
        std::cout << "Client[" << client.getFD() << "]:" << client.getNick() << " set mode " << appliedModes << appliedParams << " on " << target << RESET << std::endl; 

    }
    
} 



// void Server::handleWho(Client &client, std::vector<std::string> args){
//     if(!client.isRegistered()){
//         sendReply(client, ERR_NOTREGISTERED);
//         return;
//     }

//     if(args.size() < 2){
//         sendReply(client, RPL_ENDOFMODES, "*");
//         return;
//     }


//     if (args.size)

//     std::string target = args[1];
//     Channel *channel = findChannel(target);

//     if(channel){
//         const std::set<int> &members = channel->getMembers();
//         for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it){
//             Client *member = findClientByFd(*it);
//             if (!member)
//                 continue;
//             std::string flags = channel->isOperator(*it) ? "H@" : "H";
//             std::string params = target + " " + member->getUser() + " " + member->getIP() + " ircserv " + member->getNick() + " " + flags;
//             sendReplyFour(client, "352", params, "0 " + member->getRealName());
//         }
//     }
//     sendReply(client, RPL_ENDOFWHO, target);
// }

void Server::handleWho(Client &client, std::vector<std::string> args){
    if(!client.isRegistered()){
        sendReply(client, ERR_NOTREGISTERED);
        return;
    }

    std::string target = (args.size() > 1) ? args[1] : "";

    // 1. If targeting a specific channel (e.g., "WHO #general")
    if (!target.empty() && (target[0] == '#' || target[0] == '&')) {
        Channel *channel = findChannel(target);
        if (channel) {
            const std::set<int> &members = channel->getMembers();
            for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it){
                Client *member = findClientByFd(*it);
                if (!member || !member->isRegistered()) 
                    continue;
                
                std::string flags = channel->isOperator(*it) ? "H@" : "H";
                std::string params = target + " " + member->getUser() + " " + member->getIP() + " ircserv " + member->getNick() + " " + flags;
                sendReplyFour(client, "352", params, "0 " + member->getRealName());
            }
        }
        sendReply(client, RPL_ENDOFWHO, target);
        return;
    }

    // 2. If no target provided (just "WHO" or "WHO *"), list all users on the server
    for (size_t i = 0; i < clients.size(); i++) {
        Client *member = &clients[i];
        if (!member->isRegistered()) 
            continue;
        
        // Search the _channels map to find which channels this client is in
        std::string userChannels = "";
        for (std::map<std::string, Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
            if (it->second.isMember(member->getFD())) {
                if (!userChannels.empty()) 
                    userChannels += ",";
                userChannels += it->first; // append channel name
            }
        }
        
        // RFC standard dictates using "*" if the user is not in any channel
        if (userChannels.empty()) 
            userChannels = "*"; 

        std::string params = userChannels + " " + member->getUser() + " " + member->getIP() + " ircserv " + member->getNick() + " H";
        sendReplyFour(client, "352", params, "0 " + member->getRealName());
    }
    
    std::string endTarget = target.empty() ? "*" : target;
    sendReply(client, RPL_ENDOFWHO, endTarget);
}