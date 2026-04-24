/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Include.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:18 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/23 18:23:516 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <algorithm>
# include <arpa/inet.h> // for inet_ntoa()
# include <csignal>
# include <cstring>
# include <errno.h>
# include <exception>
# include <fcntl.h>
# include <iostream>
# include <netdb.h>
# include <netinet/in.h> // for sockaddr_in
# include <poll.h>
# include <utility>
# include <unistd.h> // for close()
# include <vector>
# include <stdexcept>
# include <string>
# include <sys/socket.h>
# include <sys/types.h> // for socket()
# include <sstream>
# include <set>

class Parser;
class Server;
class Client;
class Channel;

typedef			std::vector<Parser>::iterator	parserIt;
typedef void	(Server::*cmdFunc_t)(Client *, Parser);

# define		NB_CMD		13
# define		BUFFER_SIZE	512

# define GREEN		std::string("\e[1;32m")
# define RED		std::string("\e[1;31m")
# define WHITE		std::string("\e[0;37m")
# define YELLOW		std::string("\e[1;33m")
# define PURPLE		std::string("\e[1;35m")
# define BLUE		std::string("\e[1;34m")

/* -------------------------------------------------------------------------- */
/*                             Messages d'erreurs                             */
/* -------------------------------------------------------------------------- */

// Erreurs liées à l'authentification
# define ERR_NONICKNAMEGIVEN( nickname )				"431 " + nickname + " :No nickname given"
# define ERR_NICKNAMEINUSE( nickname, newNickname )		"433 " + nickname + " " + newNickname + " :Nickname is already in use"
# define ERR_NOTREGISTERED( command )					"451 " + command + " :You have not registered"
# define ERR_ALREADYREGISTRED( nickname )				"462 " + nickname + " :You may not reregister"
# define ERR_PASSWDMISMATCH( nickname )					"464 " + nickname + " :Incorrect password (disconecting)"
// Erreurs liées aux commandes
# define ERR_UNKNOWNCOMMAND( nickname, command )		"421 " + nickname + " " + command + " :Unknown command"
# define ERR_NEEDMOREPARAMS( nickname, command )		"461 " + nickname + " " + command + " :Not enough parameters"
# define ERR_NORECIPIENT( nickname, command )			"411 " + nickname + " " + command + " :No recipient given"
# define ERR_NOTEXTTOSEND( nickname, command )			"412 " + nickname + " " + command + " :No text to send"
# define ERR_TOOMANYTARGETS( nickname, command )		"407 " + nickname + " " + command + " :Duplicate recipients. No message delivered"
# define ERR_NOSUCHNICK( nickname, command )			"401 " + nickname + " " + command + " :No such nick"
# define ERR_UNKNOWNMODE( nickname, charac )			"472 " + nickname + " " + charac  + " :is unknown mode char to me"
# define ERR_KEYSET( nickname, channel )				"467 " + nickname + " " + channel + " :Channel key already set"
// Erreurs liées aux channels
# define ERR_NOSUCHCHANNEL( channel )					"403 " + channel + " :No such channel"
# define ERR_NOTONCHANNEL( nickname, channel )			"442 " + nickname + " " + channel + " :You're not on that channel"
# define ERR_USERONCHANNEL( nickname, channel )			"443 " + nickname + " " + channel + " :is already on channel"
# define ERR_CHANNELISFULL( nick, channel )				"471 " + nick + " " + channel + " :Cannot join channel (+l)"
# define ERR_INVITEONLYCHAN( channel )					"473 " + channel + " :Cannot join channel (+i)"
# define ERR_BANNEDFROMCHAN( nick, channel )			"474 " + nick + " " + channel + " :Cannot join channel (+b)"
# define ERR_BADCHANNELKEY( nick, channel )				"475 " + nick + " " + channel + " :Cannot join channel (+k)"
# define ERR_CHANOPRIVSNEEDED( nickname, channel )		"482 " + nickname  + " " + channel + " :You're not channel operator"
# define ERR_USERNOTINCHANNEL( nick, target, chan )		"441 " + nick + " " + target + " " + chan + " :They aren't on that channel"

# define ERR_SHUTDOWN									"ERROR :Server shutting down"

/* -------------------------------------------------------------------------- */
/*                             Messages de reponse                            */
/* -------------------------------------------------------------------------- */

# define RPL_WELCOME( nickname, prefix )				"001 " + nickname + " :Welcome to the Internet Relay Network " + prefix 
# define RPL_YOURHOST( nickname, servName, version )	"002 " + nickname + " :Your host is " + servername + ", running version " + version
# define RPL_CREATED( nickname, date )					"003 " + nickname + " :This server was created " + date
# define RPL_MYINFO( nickname, servName, version )		"004 " + nickname + " " + servername + " " + version + " <available user modes> <available channel modes>"

# define RPL_NOTOPIC( channel)							"331 " + channel + " :No topic is set"
# define RPL_TOPIC( nickname, channel, topic )			"332 " + nickname + " " + channel + " :" + topic
# define RPL_NAMREPLY( nickname, channel )				"353 " + nickname + " " + channel + " :"
# define RPL_ENDOFNAMES( nickname, channel )			"366 " + nickname + " " + channel + " :End of /NAMES list"

# define RPL_QUIT( nickname, reason )					":" + nickname + " QUIT :" + reason
# define RPL_CAP()										"CAP * LS :"
# define RPL_PART( channel, reason )					"PART " + channel + " " + reason
# define RPL_KICK( channel, nickname, reason )			"KICK " + channel + " " + nickname +  " :" + reason
# define RPL_JOIN( channel )							"JOIN :" + channel
# define RPL_NICK( oldNick, newNick )					":" + oldNick + " NICK " + newNick
# define RPL_PRIVMSG( target, message )					"PRIVMSG " + target + " :" + message
# define RPL_NOTICE( target, message )					"NOTICE " + target + " :" + message
# define RPL_MODE( target, message )					"MODE " + target + " " + message
# define RPL_CHANNELMODEIS( nick, chan, mods )			"324 " + nick + " " + chan + " " + mods
# define RPL_INVITING( nick, invited, chan )			"341 " + nick + " " + invited + " " + chan

# define DISPLAY_SERVER		PURPLE << "\n   ☑️\t Server launched !\n\n"

# define CRLF "\r\n"
# define CRLFNL "\r\n\n"
# define WRONG_USAGE	RED << "\n   ⚠️\t WRONG USAGE !\n\n"
# define ERROR		RED + CRLF + "🚨 Error:\t" + WHITE

# define ERR_WRONG_USAGE 					WRONG_USAGE \
												<< WHITE << "Try:  ./ircserv " \
												<< GREEN << "<" \
												<< WHITE << "port" \
												<< GREEN << "> <" \
												<< WHITE << "password" \
												<< GREEN << ">" << WHITE << CRLFNL
# define ERR_INVALID_PORT					std::runtime_error( RED + "\nError: Invalid port" \
												+ WHITE + " - expected a value between " \
												+ GREEN + "1024" \
												+ WHITE + " and " \
												+ GREEN + "49151" + WHITE + "." + CRLF \
												+ WHITE + "Try:  ./ircserv " \
												+ GREEN + "<" \
												+ WHITE + "port" \
												+ GREEN + "> <" \
												+ WHITE + "password" \
												+ GREEN + ">" + WHITE + CRLF)
# define ERR_INVALID_PASSWORD				std::runtime_error( ERROR \
												+ WHITE + "Empty password" + CRLFNL )
# define ERR_SOCKET_CREATION					std::runtime_error( ERROR \
												+ WHITE + "Server socket creation failed ..." + CRLFNL)
# define ERR_SO_REUSEADDR					std::runtime_error( ERROR \
												+ WHITE + "Failed to set option (SO_REUSEADDR) on server socket ..." + CRLFNL)
# define ERR_O_NONBLOCK						std::runtime_error( ERROR \
												+ WHITE + "Failed to set option (O_NONBLOCK) on server socket ..." + CRLFNL)
# define ERR_BINDING_SOCKET					std::runtime_error( ERROR \
												+ WHITE + "failed to bind socket ...\n" + "Check if server is not already running in another terminal." + CRLFNL)
# define ERR_LISTEN							std::runtime_error( ERROR \
												+ WHITE + "Listen function failed ..." + CRLFNL)
# define ERR_POLL							std::runtime_error( ERROR \
												+ WHITE + "Poll failed ..." + CRLFNL)
# define ERR_FIND_CLIENT						std::runtime_error( ERROR \
												+ WHITE + "Client not found ..." + CRLFNL)
# define ERR_ACCEPT_FAILED					RED << "accept() failed" << WHITE << CRLFNL
# define ERR_FCNTL_FAILED( ClientFd )		WHITE << "Failed to set option (O_NONBLOCK) for client " \
												<< RED << ClientFd << WHITE << "." << CRLFNL
# define SERVER_OFF							std::runtime_error( WHITE + "SERVER CLOSED." + CRLF )

# define NEW_CLIENT( ClientFd )				WHITE << "Client < " \
												<< YELLOW << ClientFd \
												<< WHITE << " > " \
												<< YELLOW << "TRYING TO CONNECT." << WHITE << CRLF
# define RPL_QUIT( nickname, reason )		":" + nickname + " QUIT :" + reason
# define CLIENT_DISCONNECTED( ClientFd )		WHITE << "Client < " \
												<< YELLOW << ClientFd \
												<< WHITE << " > " \
												<< RED << "DISCONNECTED." << WHITE << CRLF
# define SERVER_CONNECTED( fd, port )		WHITE << "  Server " \
												<< GREEN << "CONNECTED" \
												<< WHITE << " !!\tListening on FD " \
												<< GREEN << "< " \
												<< YELLOW << fd \
												<< GREEN << " >" \
												<< WHITE << " and port " \
												<< GREEN << "< " \
												<< YELLOW << port \
												<< GREEN << " >" \
												<< WHITE << " )." << WHITE << CRLF
# define WAITING_CONNECTION					WHITE << "\tWaiting connection..." << CRLF
# define PRINT_DATA( ClientFd, Data )		WHITE << "Client < " \
												<< YELLOW << ClientFd \
												<< WHITE << " > " << Data << CRLF
# define ERR_CMD_ARGS( token, msg )			PURPLE + "Wrong usage of : " + YELLOW + token + PURPLE + "\nUse like this : " + YELLOW + token + " " + msg + WHITE + CRLFNL
# define ERR_CMD_NOT_FOUND( Cmd )			PURPLE + "Unknown command: " + Cmd + "\nType " \
												+ GREEN + "HELP " \
												+ PURPLE + "to see the list of available commands." + WHITE + CRLFNL
# define ERR_INEXISTANT_CHANNEL( topic )		PURPLE + "Unknown channel: " + YELLOW + topic + PURPLE + "\nType " \
												+ GREEN + "CHANNEL_ON " \
												+ PURPLE + "to see the list of available channels." + WHITE + CRLFNL
# define ERR_INEXISTANT_CLIENT( name )		PURPLE + "Unknown client: " + YELLOW + name + PURPLE + "\nType a valid client." + WHITE + CRLFNL
# define ERR_NOT_OPERATOR( topic )		PURPLE + "You are not operator in the channel " + YELLOW + topic + PURPLE + "." + WHITE + CRLFNL
# define ERR_NOT_IN_CHANNEL( topic )		PURPLE + "You are not in " + YELLOW + topic + PURPLE + " channel." + WHITE + CRLFNL
# define ERR_CLI_NOT_IN_CHAN( cli, chan )	YELLOW + cli + WHITE + "are not in " + YELLOW + chan + " channel." + WHITE + CRLFNL
# define ERR_NICKNAME_USED( nickname )		RED + "INFO: " + YELLOW + nickname + WHITE + " Nickname is already used." + CRLFNL
# define ERR_USERNAME_USED( nickname )		RED + "INFO: " + YELLOW + nickname + WHITE + " Username is already used." + CRLFNL
# define ERR_TOPIC_USED( topic )				RED + "INFO: " + YELLOW + topic + WHITE + " topic is already used by an other channel." + CRLFNL
# define ERR_NOT_TOPIC_FOUND( topic )		RED + "INFO: " + YELLOW + topic + WHITE + " no channel match with this topic." + CRLFNL
# define COMMAND_LIST						PURPLE + "Available commands:" + CRLFNL \
												+ GREEN + "  - USER" + WHITE + " : set your username\n" \
												+ GREEN + "  - NICK" + WHITE + " : choose your nickname\n" \
												+ GREEN + "  - PASS" + WHITE + " : set your password\n" \
												+ GREEN + "  - TOPIC" + WHITE + " : change the channel topic\n" \
												+ GREEN + "  - KICK" + WHITE + " : remove a user from a channel\n" \
												+ GREEN + "  - INVITE" + WHITE + " : invite a user to a channel\n" \
												+ GREEN + "  - JOIN" + WHITE + " : join/create a channel\n" \
												+ GREEN + "  - MODE" + WHITE + " : change channel mode\n" \
												+ GREEN + "  - HELP_MODE" + WHITE + " : show MODE flags and usage\n" \
												+ GREEN + "  - CHANNEL" + WHITE + " : send a message to a channel\n" \
												+ GREEN + "  - CHANNEL_ON" + WHITE + " : list active channels\n" \
												+ GREEN + "  - PRIVMSG" + WHITE + " : send a private message\n" + WHITE
# define HELP_MODE							PURPLE + "MODE - Change the channel’s mode:\nUsage: "  \
												+ PURPLE + "MODE " + YELLOW + "<" + GREEN + "channel" + YELLOW + "> <" + GREEN + "flag" + YELLOW + "> [" + GREEN + "value" + YELLOW + "]" + WHITE + CRLFNL \
												+ "· " + GREEN + "i: " + WHITE + "Set/remove Invite-only channel.\n" \
												+ "· " + GREEN + "t: " + WHITE + "Set/remove the restrictions of the TOPIC command to channel operators.\n" \
												+ "· " + GREEN + "k: " + WHITE + "Set/remove the channel key (password).\n" \
												+ "· " + GREEN + "o: " + WHITE + "Give/take channel operator privilege.\n" \
												+ "· " + GREEN + "l: " + WHITE + "Set/remove the user limit to channel (0 to none)." + WHITE + CRLFNL
# define MSG_NEW_CLIENT( NewFd )				PURPLE + "\n\tWelcome to IRC\n" \
												+ BLUE + "First steps to use IRC. Register with :\n" \
												+ GREEN + " - USER <your username>\n" \
												+ GREEN + " - PASS <your password>" + WHITE + CRLFNL
# define UPDATE_USERNAME( username )			GREEN + "Your username has been successfully processed.\n" \
												+ BLUE + "New username : " + YELLOW + username + WHITE + CRLFNL
# define UPDATE_NICKNAME( nickname )			GREEN + "Your nickname has been successfully processed.\n" \
												+ BLUE + "New nickname : " + YELLOW + nickname + WHITE + CRLFNL
# define UPDATE_PASSWORD						GREEN + "Your password has been successfully processed." + WHITE + CRLF
# define SERVER_CLOSED						RED + "\tINFO: " \
												+ WHITE + "IRC server has been closed. Thank you." + WHITE + CRLFNL
# define REGISTERED( status )				GREEN + "You have been correctly " + YELLOW + status + GREEN + "." + WHITE + CRLFNL
# define CHANNEL_MESSAGE(top, nick, msg)		YELLOW + top + GREEN + " : " + YELLOW + nick + GREEN + " : " + WHITE + msg + CRLFNL
# define RECEIVED_INVITE( topic )			YELLOW + topic + WHITE + " send you an invite." + CRLFNL
# define RECEIVED_KICK( topic )				YELLOW + "You have been " + RED + "kicked " + YELLOW + "from " + RED + topic + WHITE + CRLFNL
# define CHANNEL_OPERATOR( topic )			GREEN + "Channel " + YELLOW + topic + GREEN + " mode updated." + WHITE + CRLFNL
# define CHANNEL_LIMIT( topic, limit )		YELLOW + topic + GREEN + " channel user limitation has been set to " + limit + WHITE + CRLFNL
# define CHANNEL_LIMIT_R( topic )			YELLOW + topic + GREEN + " channel user limitation has been removed." + WHITE + CRLFNL
# define CHANNEL_FULL( topic )				RED + "Channel " + YELLOW + topic + " is full." + WHITE + CRLFNL
# define CREATE_CHANNEL( topic )				BLUE + "Channel " + YELLOW + topic + BLUE + " has been successfully created." + WHITE + CRLFNL
# define ALREADY_IN_CHANNEL( Topic )			BLUE + "You are already in channel " + YELLOW + Topic + BLUE + "." + WHITE + CRLFNL
# define NEW_TOPIC_SET( old, nTopic, name )	GREEN + "Channel topic has been set to " + YELLOW + nTopic + GREEN + " by " + YELLOW + name + GREEN + ". (OLD: " + YELLOW + old + GREEN + ")" + WHITE + CRLFNL
# define CLIENT_KICKED( topic, name )		GREEN + "Client named " \
												+ YELLOW + name \
												+ GREEN + " has been kicked from the " \
												+ YELLOW + topic + GREEN + " channel." + WHITE + CRLFNL
# define CLIENT_INVITED( topic, name )		GREEN + "Client named " \
												+ YELLOW + name \
												+ GREEN + " has been invite to the " \
												+ YELLOW + topic + GREEN + " channel." + WHITE + CRLFNL
# define NEW_CLIENT_JOIN( nick, topic )		YELLOW + nick + GREEN + " has joined channel " + YELLOW + topic + GREEN + "." + WHITE + CRLFNL
# define PRIV_MSG( sender, message )			YELLOW + sender + GREEN + " : " + WHITE + message + CRLFNL
# define WRONG_PASSWORD( channel )			RED + "Error: " + GREEN + "Wrong password to access to " + YELLOW + channel + GREEN + " channel" + WHITE + CRLFNL
# define SET_PSWD_RESTRICTION( channel )		YELLOW + channel + GREEN + " channel password restriction updated." + WHITE + CRLFNL

bool getStringUntil(const std::string& input, std::string& result, char delimiter, size_t& startPos);
std::vector<std::string>	split(std::string toSplit, char delim);

class runtime_error : public std::exception
{
	public:
		explicit runtime_error( const std::string& message );

		explicit runtime_error( const char *message );
};