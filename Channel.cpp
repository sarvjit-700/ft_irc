#include "Channel.hpp"


Channel::Channel(const std::string& name):_name(name), _topic(""), _topicRestricted(true), _inviteOnly(false), _hasKey(false), _hasLimit(false), _userLimit(0), _key("")
{}


Channel::~Channel()
{}


const std::string& Channel::getNameChan()  const
{
    return _name;
}


const std::set<int>&   Channel::getMembers() const
{
    return _membersFds;
}


bool Channel::isMember(int fd) const
{
    return (_membersFds.find(fd) != _membersFds.end());
}


bool Channel::isOperator(int fd) const
{
    return (_operatorsFds.find(fd) != _operatorsFds.end());
}

void Channel::addOperator(int fd)
{
    _operatorsFds.insert(fd);
}

void Channel::removeOperator(int fd)
{
    _operatorsFds.erase(fd);
}

void Channel::addMember(int fd)
{
    _membersFds.insert(fd);
    
    if(_membersFds.size() == 1)
        _operatorsFds.insert(fd);
}


void Channel::removeMember(int fd)
{
    _membersFds.erase(fd);
    _operatorsFds.erase(fd);
}


//for JOIN
const std::string& Channel::getTopic() const{
    return _topic;
}

void Channel::setTopic(const std::string &topic){
    _topic = topic;
}

//for TOPIC
bool Channel::isTopicRestricted() const
{
    return _topicRestricted;
}

void Channel::setTopicRestricted(bool restricted)
{
    _topicRestricted = restricted;
}

// for Invite
void Channel::inviteUser(const std::string &nick)
{
    _invitedNicks.insert(nick);
}

bool Channel::isInvited(const std::string &nick) const
{
    return (_invitedNicks.find(nick) != _invitedNicks.end());
}

void Channel::removeInvited(const std::string &nick)
{
    _invitedNicks.erase(nick);
}


//for MODE
bool Channel::isInviteOnly()const{
    return _inviteOnly;
}

void Channel::setInviteOnly(bool state){
    _inviteOnly = state;
}

bool Channel::hasKey() const{
    return _hasKey;
}

std::string Channel::getKey() const{
    return _key;
}

bool Channel::hasLimit() const{
    return _hasLimit;
}

int Channel::getUserLimit() const{
    return _userLimit;
}

void Channel::setUserLimit(int limit){
    _userLimit = limit;
    _hasLimit = true;
}

void Channel::removeUserLimit(){
    _userLimit = 0;
    _hasLimit = false;
}


void Channel::setKey(const std::string &key)
{
    _key = key;
    _hasKey = true;
}

void Channel::removeKey()
{
    _key.clear();
    _hasKey = false;
}

void Channel::promoteNewOperator()
{
    if (_operatorsFds.empty() && !_membersFds.empty())
    {
        int newOp = *_membersFds.begin();
        _operatorsFds.insert(newOp);
    }
}