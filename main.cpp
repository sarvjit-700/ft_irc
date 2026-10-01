#include "Server.hpp"
#include <cstdlib> //atoi
#include <stdexcept> //exceptions


int check_port(const char* port_str)
{
    if (port_str == NULL || port_str[0] == '\0')
        throw std::invalid_argument("Port is empty.");

    for(int i = 0; port_str[i]; i++)
    {
        if(!isdigit(port_str[i]))
            throw std::invalid_argument("Port must include only numbers.");
    }

    int port = atoi(port_str);
    if (port < 1024 || port > 49151)
        throw std::out_of_range("Port must be between 1024 and 49151.");
    return port;
}

std::string check_pass(const char* pass_str)
{
    if (pass_str == NULL || pass_str[0] == '\0')
        throw std::invalid_argument("Password is empty.");
    for(int i = 0; pass_str[i]; i++)
    {
        if (isspace(pass_str[i]))
            throw std::invalid_argument("Password can not contain whitespaces");
    }
    return std::string(pass_str);
}

int main(int argc, char **argv){
    Server Irc_Server;

    if(argc != 3)
    {
        std::cerr << "Usage: program <port> <password>" << std::endl;
        return 1;
    }

    try{
        int user_port = check_port(argv[1]);
        std::string user_pass = check_pass(argv[2]);

        struct sigaction sa;
        sa.sa_handler = Server::SignalHandler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGQUIT, &sa, NULL);

        signal(SIGPIPE, SIG_IGN);


        Irc_Server.setPort(user_port);
        Irc_Server.setPassword(user_pass); //setting password for the server, we have to compare server password and user password(handlePass)

        Irc_Server.ServerInit();
    }

    catch(const std::exception& e){
        // Irc_Server.CloseFds();
        std::cerr << e.what() << std::endl;
    }

    std::cout << "Goodbye!" << std::endl;
}

