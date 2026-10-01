#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <vector>


class Channel
{
    private:
        std::string             _name;  //name of the channel
        std::set<int>           _membersFds;   //container with every client in the channel; int = socket fd
        std::set<int>           _operatorsFds; //clients with rights of operators in the channel; 
        std::set<std::string>   _invitedNicks;
        std::string             _topic;
        bool                    _topicRestricted;   //only t+ mod can change a topic



        //for MODE
        bool _inviteOnly;
        bool _hasKey;
        bool _hasLimit;
        int _userLimit;
        std::string _key;
        

    public:
        Channel(const std::string& name);
        ~Channel();

        //Getters
        const std::string&    getNameChan()  const; //Name of the channel for server
        const std::set<int>&   getMembers() const; //

        
        bool isMember(int fd) const; //checking clients for sending PRIVMSG, 
        bool isOperator(int fd) const; //for commands KICK, INVITE, MODE 

        void addMember(int fd); //
        void removeMember(int fd); //for commands PART, KICK..


        void promoteNewOperator();

        //for JOIN
        const std::string& getTopic() const;
        void setTopic(const std::string &topic);

        //for operators
        void addOperator(int fd);
        //void removeOperator(int fd); //later for MODE, PART, KICK and QUIT command

        //mod + t
        bool isTopicRestricted() const;
        void setTopicRestricted(bool restricted);

        void inviteUser(const std::string &nick);
        bool isInvited(const std::string &nick) const;
        void removeInvited(const std::string &nick);


        // we have to added them later for command MODE,
        // first client who enters the channel will become operator
        //void addOperator(int fd); //
        void removeOperator(int fd);



        //for MODE
        //invite
        bool isInviteOnly() const;
        void setInviteOnly(bool state);

        //pass/key
        bool hasKey() const;
        std::string getKey() const;
        void setKey(const std::string &key);
        void removeKey();

        //user limit
        bool hasLimit() const;
        int getUserLimit() const;
        void setUserLimit(int Limit);
        void removeUserLimit();

};

#endif