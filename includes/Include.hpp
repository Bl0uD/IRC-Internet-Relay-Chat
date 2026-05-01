/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Include.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:18 by jdupuis           #+#    #+#             */
/*   Updated: 2026/05/01 03:11:20 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include	<algorithm>
# include	<arpa/inet.h>	// for inet_ntoa()
# include	<cctype>
# include	<csignal>
# include	<cstring>
# include	<errno.h>
# include	<exception>
# include	<fcntl.h>
# include	<iomanip>
# include	<iostream>
# include	<netdb.h>
# include	<netinet/in.h>	// for sockaddr_in
# include	<poll.h>
# include	<set>
# include	<sstream>
# include	<stdexcept>
# include	<string>
# include	<sys/socket.h>
# include	<sys/types.h>	// for socket()
# include	<unistd.h>		// for close()
# include	<utility>
# include	<vector>

class	Channel;
class	Client;
class	Parser;
class	Server;

typedef				std::vector<Parser>::iterator	parserIt;
typedef void		(Server::*cmdFunc_t)(Client *, Parser);

# define			NB_CMD		13
# define			BUFFER_SIZE	512

# define BLUE		std::string("\e[1;34m")
# define GREEN		std::string("\e[1;32m")
# define PURPLE		std::string("\e[1;35m")
# define RED		std::string("\e[1;31m")
# define WHITE		std::string("\e[0;37m")
# define YELLOW		std::string("\e[1;33m")

/* -------------------------------------------------------------------------- */
/*                               Constantes IRC                               */
/* -------------------------------------------------------------------------- */

# define CRLF				"\r\n"
# define CRLFNL				"\r\n\n"
# define DISPLAY_SERVER		PURPLE << "\n   ☑️\t Server launched !\n\n"
# define ERROR				RED + CRLF + "🚨 Error:\t" + WHITE
# define WRONG_USAGE		RED << "\n   ⚠️\t WRONG USAGE !\n\n"

/* -------------------------------------------------------------------------- */
/*                            Messages d'erreur IRC                           */
/* -------------------------------------------------------------------------- */

/* Authentification */
# define ERR_ALREADYREGISTRED( nickname )				"462 " + nickname + " :You may not reregister"
# define ERR_NICKNAMEINUSE( nickname, newNickname )		"433 " + nickname + " " + newNickname + " :Nickname is already in use"
# define ERR_NOTREGISTERED( command )					"451 " + command + " :You have not registered"
# define ERR_PASSWDMISMATCH( nickname )					"464 " + nickname + " :Incorrect password (disconecting)"

/* Canaux */
# define ERR_BADCHANNELKEY( nick, channel )				"475 " + nick + " " + channel + " :Cannot join channel (+k)"
# define ERR_CHANOPRIVSNEEDED( nickname, channel )		"482 " + nickname  + " " + channel + " :You're not channel operator"
# define ERR_CHANNELISFULL( nick, channel )				"471 " + nick + " " + channel + " :Cannot join channel (+l)"
# define ERR_INVITEONLYCHAN( channel )					"473 " + channel + " :Cannot join channel (+i)"
# define ERR_NOSUCHCHANNEL( channel )					"403 " + channel + " :No such channel"
# define ERR_NOTONCHANNEL( nickname, channel )			"442 " + nickname + " " + channel + " :You're not on that channel"
# define ERR_USERNOTINCHANNEL( nick, target, chan )		"441 " + nick + " " + target + " " + chan + " :They aren't on that channel"
# define ERR_USERONCHANNEL( user, nickname, channel )	"443 " + user + " " + nickname + " " + channel + " :is already on channel"

/* Commandes */
# define ERR_KEYSET( nickname, channel )				"467 " + nickname + " " + channel + " :Channel key already set"
# define ERR_NEEDMOREPARAMS( nickname, command )		"461 " + nickname + " " + command + " :Not enough parameters"
# define ERR_NORECIPIENT( nickname, command )			"411 " + nickname + " " + command + " :No recipient given"
# define ERR_NOSUCHNICK( nickname, command )			"401 " + nickname + " " + command + " :No such nick"
# define ERR_NOTEXTTOSEND( nickname, command )			"412 " + nickname + " " + command + " :No text to send"
# define ERR_TOOMANYTARGETS( nickname, command )		"407 " + nickname + " " + command + " :Duplicate recipients. No message delivered"
# define ERR_UNKNOWNCOMMAND( command )					"421 " + command + " :Unknown command"
# define ERR_UNKNOWNMODE( nickname, charac )			"472 " + nickname + " " + charac  + " :is unknown mode char to me for <channel>"

/* -------------------------------------------------------------------------- */
/*                           Messages de reponse IRC                          */
/* -------------------------------------------------------------------------- */

# define RPL_CAP()										"CAP * LS :"
# define RPL_CHANNELMODEIS( nick, chan, mods )			"324 " + nick + " " + chan + " " + mods
# define RPL_CREATED( nickname, date )					"003 " + nickname + " :This server was created " + date
# define RPL_ENDOFNAMES( nickname, channel )			"366 " + nickname + " = " + channel + " :End of /NAMES list"
# define RPL_INVITING( nick, invited, chan )			"341 " + nick + " " + invited + " " + chan
# define RPL_JOIN( channel )							"JOIN " + channel
# define RPL_KICK( channel, nickname, reason )			"KICK " + channel + " " + nickname +  " :" + reason
# define RPL_MODE( target, message )					"MODE " + target + " " + message
# define RPL_MYINFO( nickname, servName, version )		"004 " + nickname + " " + servName + " " + version + " iov itkol"
# define RPL_NAMREPLY( nickname, channel )				"353 " + nickname + " = " + channel + " :"
# define RPL_NICK( oldNick, newNick )					":" + oldNick + " NICK :" + newNick
# define RPL_NOTOPIC( channel)							"331 " + channel + " :No topic is set"
# define RPL_PART( channel, reason )					"PART " + channel + " :" + reason
# define RPL_PRIVMSG( target, message )					"PRIVMSG " + target + " :" + message
# define RPL_QUIT( nickname, reason )					":" + nickname + " QUIT :" + reason
# define RPL_TOPIC( nickname, channel, topic )			"332 " + nickname + " " + channel + " :" + topic
# define RPL_WELCOME( nickname, prefix )				"001 " + nickname + " :Welcome to the Internet Relay Network " + prefix
# define RPL_YOURHOST( nickname, servName, version )	"002 " + nickname + " :Your host is " + servName + ", running version " + version
# define RPL_YOUREOPER									"381 :You are now an IRC operator"

/* -------------------------------------------------------------------------- */
/*                 Messages d'etat et d'interface server                      */
/* -------------------------------------------------------------------------- */

# define ERR_WRONG_USAGE 								WRONG_USAGE << WHITE << "Try:  ./ircserv " << GREEN << "<" << WHITE << "port" << GREEN << "> <" << WHITE << "password" << GREEN << ">" << WHITE << CRLFNL

# define SERVER_CONNECTED( fd, port )					WHITE << "  Server " << GREEN << "CONNECTED" << WHITE << " !!\tListening on FD " << GREEN << "< " << YELLOW << fd << GREEN << " >" << WHITE << " and port " << GREEN << "< " << YELLOW << port << GREEN << " >" << WHITE << "." << CRLF
# define WAITING_CONNECTION								WHITE << "\tWaiting connection..." << CRLF
# define NEW_CLIENT( ClientFd )							WHITE << "Client < " << YELLOW << ClientFd << WHITE << " > " << YELLOW << "TRYING TO CONNECT." << WHITE << CRLF
# define CLIENT_DISCONNECTED( ClientFd )				WHITE << "Client < " << YELLOW << ClientFd << WHITE << " > " << RED << "DISCONNECTED." << WHITE << CRLF

# define ERR_ACCEPT_FAILED								RED << "accept() failed" << WHITE << CRLFNL 
# define ERR_BINDING_SOCKET								std::runtime_error( ERROR + WHITE + "failed to bind socket ...\n" + "Check if server is not already running in another terminal." + CRLFNL)
# define ERR_FCNTL_FAILED( ClientFd )					WHITE << "Failed to set option (O_NONBLOCK) for client " << RED << ClientFd << WHITE << "." << CRLFNL 
# define ERR_INVALID_PASSWORD							std::runtime_error( ERROR + WHITE + "Empty password" + CRLFNL )
# define ERR_INVALID_PORT								std::runtime_error( RED + "\nError: Invalid port" + WHITE + " - expected a value between " + GREEN + "1024" + WHITE + " and " + GREEN + "49151" + WHITE + "." + CRLF + WHITE + "Try:  ./ircserv " + GREEN + "<" + WHITE + "port" + GREEN + "> <" + WHITE + "password" + GREEN + ">" + WHITE + CRLF)
# define ERR_LISTEN										std::runtime_error( ERROR + WHITE + "Listen function failed ..." + CRLFNL)
# define ERR_O_NONBLOCK									std::runtime_error( ERROR + WHITE + "Failed to set option (O_NONBLOCK) on server socket ..." + CRLFNL)
# define ERR_POLL										std::runtime_error( ERROR + WHITE + "Poll failed ..." + CRLFNL)
# define ERR_SO_REUSEADDR								std::runtime_error( ERROR + WHITE + "Failed to set option (SO_REUSEADDR) on server socket ..." + CRLFNL)
# define ERR_SOCKET_CREATION							std::runtime_error( ERROR + WHITE + "Server socket creation failed ..." + CRLFNL)
# define DISPLAY_CMD( cFD, cNickname, message )			"Command tried by" << PURPLE << "  < " << YELLOW << cFD << PURPLE << " >  " << YELLOW << std::left << std::setw(16) << cNickname << WHITE << ": " << message << std::endl;

# define SERVER_OFF										std::runtime_error( WHITE + "SERVER CLOSED." + CRLF )

bool	getStringUntil(const std::string& input, std::string& result, char delimiter, size_t& startPos);
std::vector<std::string>	split(std::string toSplit, char delim);

class runtime_error : public std::exception
{
	public:
		explicit	runtime_error( const std::string &message );

		explicit	runtime_error( const char *message );
};