#include "Client.hpp"
#include "Server.hpp"

Client::Client() : _fd(-1), _hasUser(false), _hasNick(false), 
                _passOK(false), _isServerOp(false), _pendingDisconnect(false) 
{}

Client::Client(int fd) : _fd(fd), _hasUser(false), _hasNick(false), 
                _passOK(false), _isServerOp(false), _pendingDisconnect(false) //added _pd
{}

Client::Client(const Client &copy)
{
    *this = copy;
}

Client &Client::operator=(const Client &copy)
{
    if (this != &copy)
    {
        this->_fd = copy._fd;
        this->_ipAddress = copy._ipAddress;
        this->_inputBuffer = copy._inputBuffer;
        this->_outputBuffer = copy._outputBuffer;
        this->_nickname = copy._nickname;
        this->_username = copy._username;
        this->_realname = copy._realname; // added
        this->_hostmask = copy._hostmask;
        this->_hasUser = copy._hasUser;
        this->_hasNick = copy._hasNick;
        this->_passOK = copy._passOK;
        this->_isServerOp = copy._isServerOp;
        this->_pendingDisconnect = copy._pendingDisconnect;
    }
    return *this;
}

Client::~Client() {}


// ----- GETTERs ----- //

int Client::getFD() const
{
    return this->_fd;
}

std::string Client::getIP() const
{
    return this->_ipAddress;
}
std::string Client::getUser() const
{
    return this->_username;
}

std::string Client::getNick() const
{
    return this->_nickname;
}

std::string Client::getRealName() const
{
    return this->_realname;
}

std::string Client::getHostmask() const
{
    return this->getNick() + "!" + this->getUser() + "@" + this->getIP();
}

// ---- input Buffer --- //
// limited to 4096 so total buffer size is managable and doesn't crash the server
void    Client::appendBuffer(const std::string &data) //UPDATED
{
    if (_inputBuffer.length() + data.length() > 4096)
    {
        _inputBuffer.clear();
        this->appendOutput(RED "ERROR :Buffer overflow. Disconnecting.\r\n" RESET);
        this->markForDisconnect();
        return;
    }
    _inputBuffer += data;
}

std::string Client::extractCommand()
{
    size_t      pos;
    std::string command;

    pos = _inputBuffer.find('\n');      // find the position of \n

    if (pos == std::string::npos)       // if no \n -> incomplete command
        return "";                      // tell the server to wait

    command = _inputBuffer.substr(0, pos);  // command = everything to \n 
    _inputBuffer.erase(0, pos + 1);         // update buffer, keep everything after \n

    if (!command.empty() && command[command.size() - 1] == '\r')
        command.erase(command.size() - 1);
    if (command.length() > 510)
        command = command.substr(0, 510);   //enforce RFC 2812 length restriction

    return command;
}


//---  output Buffer ---- //
// use appendOutput when server writes to clients
void    Client::appendOutput(const std::string &data)
{
    _outputBuffer += data;
}

//use these 2 in POLLOUT loop
std::string  Client::getOutputBuffer()
{
    return _outputBuffer;
}

void Client::eraseOutput(size_t bytesSent)
{
    if (bytesSent >= _outputBuffer.length())
        _outputBuffer.clear();
    else
        _outputBuffer.erase(0, bytesSent);
}



// --- SETTERS ---- //

// after creating client object in the server 
// std::string ip = inet_ntoa(ClientAddress.sin_addr);
// NewClient.setIP(ip);
void Client::setIP(const std::string &ipadd)
{
    _ipAddress = ipadd;
}

void Client::setNick(const std::string &nick)
{
    _nickname = nick;
    _hasNick = true;
}

void Client::setUser(const std::string &user)
{
    _username = user;
    _hasUser = true;
}

void Client::setRealName(const std::string &real)
{
    _realname = real;
}

void Client::setPassOK()
{
    _passOK = true;
}


// server will constantly need to check if a client is allowed to send msgs
bool Client::isRegistered() const
{
    //once is USER implemented,we should put him back  &&_hasUser 
    return ( _passOK && _hasNick && _hasUser );
}

// server will check if the client has set a password(Commands NICK, USER)
bool Client::getPassOk() const
{
    return _passOK;
}