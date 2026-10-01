#ifndef SYSTEMMSGS_HPP
# define SYSTEMMSGS_HPP

# include "Reply.hpp"

#define RPL_WELCOME     " :Welcome to the IRC Network "       // <nick>!<user>@<host>

// -- ERRORS MSGS -- //
static const Reply ERR_NOSUCHNICK           = {"401", ":No such nick/channel"};                       // <nick>
static const Reply ERR_NOSUCHSERVER         = {"402", ":No such server"};                              // <server>
static const Reply ERR_NOSUCHCHANNEL        = {"403", ":No such channel"};                             // <channel>
static const Reply ERR_CANNOTSENDTOCHAN     = {"404", ":Cannot send to channel"};                      // <channel>
static const Reply ERR_TOOMANYCHANNELS      = {"405", ":You have joined too many channels"};           // <channel>
static const Reply ERR_WASNOSUCHNICK        = {"406", ":There was no such nickname"};                  // <nick>
static const Reply ERR_TOOMANYTARGETS       = {"407", ":Duplicate recipients. No message delivered"}; // <target>
static const Reply ERR_NORECIPIENT          = {"411", ":No recipient given"};                         //<command>
static const Reply ERR_NOTEXTTOSEND         = {"412", ":No recipient given"};
static const Reply ERR_UNKNOWNCOMMAND       = {"421", ":Unknown command"};                              // <command>
static const Reply ERR_NONICKNAMEGIVEN      = {"431", ":No nickname given"};                            // none
static const Reply ERR_ERRONEUSNICKNAME     = {"432", ":Erroneus nickname"};                           // <nick>
static const Reply ERR_NICKNAMEINUSE        = {"433", ":Nickname is already in use"};                  // <nick>
static const Reply ERR_USERNOTINCHANNEL     = {"441", ":They aren't on that channel"};                 // <nick> <channel>
static const Reply ERR_NOTONCHANNEL         = {"442", ":You're not on that channel"};                  // <channel>
static const Reply ERR_USERONCHANNEL        = {"443", ":is already on channel"};                       // <nick> <channel>
static const Reply ERR_NOTREGISTERED        = {"451", ":You have not registered"};                      // none
static const Reply ERR_NEEDMOREPARAMS       = {"461", ":Not enough parameters"};                       // <command>
static const Reply ERR_ALREADYREGISTRED     = {"462", ":You may not reregister"};                      // none
static const Reply ERR_PASSWDMISMATCH       = {"464", ":Password incorrect"};                          // none
static const Reply ERR_CHANNELISFULL        = {"471", ":Cannot join channel (+l)"};                    // <channel>
static const Reply ERR_UNKNOWNMODE          = {"472", ":Unknown mode char"};                           // <char>
static const Reply ERR_INVITEONLYCHAN       = {"473", ":Cannot join channel (+i)"};                   // <channel>
static const Reply ERR_BADCHANNELKEY        = {"475", ":Cannot join channel (+k)"};                   // <channel>
static const Reply ERR_NOPRIVILEGES         = {"481", ":Permission Denied"};                           // none
static const Reply ERR_CHANOPRIVSNEEDED     = {"482", ":You're not channel operator"};                // <channel>
static const Reply ERR_ALREADYOPER          = {"485", ":You're already an IRC operator"};              // none

// -- WELCOME MSGS -- //

static const Reply RPL_YOURHOST             = {"002", ":Your host is"};                                // <servername> <version>
static const Reply RPL_CREATED              = {"003", ":This server was created"};                    // <date>
static const Reply RPL_MYINFO               = {"004", ""};                                            // <servername> <version> <usermodes> <chanmodes>
static const Reply RPL_ISUPPORT             = {"005", ""};                                            // <tokens> :are supported by this server

// -- COMMAND -- //
static const Reply RPL_USERHOST             = {"302", ""};                                            // <nick> <userhost>
static const Reply RPL_LISTSTART            = {"321", "Channel :Users  Name"};                        // none
static const Reply RPL_LIST                 = {"322", ""};                                            // <channel> <#visible> :<topic>
static const Reply RPL_LISTEND              = {"323", ":End of /LIST"};                               // none

// -- JOIN -- //
static const Reply RPL_NOTOPIC              = {"331", ":No topic is set"};                            // <channel>
static const Reply RPL_TOPIC                = {"332", ""};                                            // <channel> :<topic>
static const Reply RPL_NAMREPLY             = {"353", ""};                                            // <symbol> <channel> :<nick list>
static const Reply RPL_ENDOFNAMES           = {"366", ":End of /NAMES list"};                         // <channel>
static const Reply RPL_YOUREOPER            = {"381", ":You are now an IRC operator"};                // none

// -- MODE -- //
static const Reply RPL_UMODEIS              = {"221", ""};                                            // <mode>
static const Reply RPL_ENDOFWHO             = {"315", ":End of WHO list"};                            // <mask>
static const Reply RPL_CHANNELMODEIS        = {"324", ""};                                            // <channel> <mode> [<mode params>]
static const Reply RPL_WHOREPLY             = {"352", ""};                                            // <channel> <username> <hostname> <servername> <nick> <flags> :<hopcount> <realname>
static const Reply RPL_ENDOFMODES           = {"368", ":End of channel mode list"};                   // <channel>

// -- WHOIS -- //
static const Reply RPL_WHOISUSER            = {"311", ""};                                            // <nick> <username> <hostname> * :<realname>
static const Reply RPL_WHOISSERVER          = {"312", ""};                                            // <nick> <server> :<server info>
static const Reply RPL_WHOISOPERATOR        = {"313", ":is an IRC operator"};                         // <nick>
static const Reply RPL_WHOISIDLE            = {"317", ""};                                            // <nick> <seconds> <signon> :seconds idle, signon time
static const Reply RPL_ENDOFWHOIS           = {"318", ":End of WHOIS list"};                          // <nick>
static const Reply RPL_WHOISCHANNELS        = {"319", ""};                                            // <nick> :<channels>

// -- MISC -- //
static const Reply RPL_INVITING             = {"341", ""};                                            // <nick> <channel>

#endif