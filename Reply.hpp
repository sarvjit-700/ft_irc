#ifndef REPLY_HPP
# define REPLY_HPP

# include <string>
# include <vector>

class Client; // Forward declaration

struct Reply
{
    const char* code;
    const char* message;
};

// Core function (handles any number of parameters)
void sendReply(Client& client, const Reply& reply, const std::vector<std::string>& params);

// Convenience overloads
void sendReply(Client& client, const Reply& reply);
void sendReply(Client& client, const Reply& reply, const std::string& param1);
void sendReply(Client& client, const Reply& reply, const std::string& param1, const std::string& param2);

#endif