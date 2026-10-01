#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client
{
    private:
        int             _fd; // socket number e.g. fd = 4
        std::string     _ipAddress; //store client ipaddress
        std::string     _inputBuffer; // for client txt until \n
        std::string     _outputBuffer; // for server to client txt
        std::string     _nickname; // needed at client registration
        std::string     _username; // needed at client registration
        std::string     _realname; // taken from arg[4] of username
        std::string     _hostmask; // store users full details for msgs
        bool            _hasUser; 
        bool            _hasNick;
        bool            _passOK; // check password matches the one set by server
        bool            _isServerOp; // set if Server Operator
        bool            _pendingDisconnect;//need it for send disconnect message

    public:
        Client();
        Client(int fd); //should always have a fd (socket)
        Client(const Client &copy);
        Client &operator=(const Client &copy);
        ~Client();

        // getters, so the server can get client info
        int         getFD() const;
        std::string getIP() const;
        std::string getUser() const;
        std::string getNick() const;
        std::string getRealName() const; // added
        bool        isRegistered() const; // true if user/nick/password ok
        bool        getPassOk() const; 
        std::string getHostmask() const;

        // input buffer - methods
        void        appendBuffer(const std::string &data);
        std::string extractCommand(); // get string until \n and send to server to parse

        // output buffer - methods
        void        appendOutput(const std::string &data);
        std::string getOutputBuffer();
        void        eraseOutput(size_t bytesSent); // needed because message could be sent in parts to client
        
        // setters
        void        setIP(const std::string &ipadd);
        void        setUser(const std::string &user);
        void        setNick(const std::string &nick);
        void        setRealName(const std::string &realname); // added
        void        setPassOK();


        // set if client/user is an operator
        void        setServerOp(bool status);
        bool        isServerOp() const;


        void markForDisconnect() { _pendingDisconnect = true; }//Set a flag — this client needs to be disconnected
        bool shouldDisconnect() const { return _pendingDisconnect; }//Ask — does this client have a disconnect flag set?
        
};

#endif