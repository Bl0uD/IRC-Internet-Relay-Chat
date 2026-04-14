# pragma once

# include <algorithm>
# include <arpa/inet.h> // for inet_ntoa()
# include <csignal> // for signal()
# include <cstring>
# include <errno.h>
# include <exception>
# include <fcntl.h> // for fcntl()
# include <iostream>
# include <netinet/in.h> // for sockaddr_in
# include <poll.h> // for poll()
# include <utility>
# include <unistd.h> // for close()
# include <vector> // for vector
# include <stdexcept>
# include <string>
# include <sys/socket.h> // for socket()
# include <sys/types.h> // for socket()
# include <sstream> // for stringstream
# include <set>

# define	GREEN std::string("\e[1;32m") // for green color
# define	RED std::string("\e[1;31m") // for red color
# define	WHITE std::string("\e[0;37m") // for white color
# define	YELLOW std::string("\e[1;33m") // for yellow color
# define	PURPLE std::string("\e[1;35m")
# define	BLUE std::string("\e[1;34m")

# define	DISPLAY_SERVER		PURPLE << "\n   ☑️\t Server launched !\n\n"

# define	CRLF "\r\n"
# define	CRLFNL "\r\n\n"
# define	WRONG_USAGE	RED << "\n   ⚠️\t WRONG USAGE !\n\n"
# define	ERROR		RED + CRLF + "🚨 Error:\t" + WHITE

# define	ERR_WRONG_USAGE 					WRONG_USAGE \
												<< WHITE << "Try:  ./ircserv " \
												<< GREEN << "<" \
												<< WHITE << "port" \
												<< GREEN << "> <" \
												<< WHITE << "password" \
												<< GREEN << ">" << WHITE << CRLFNL
# define	ERR_INVALID_PORT					std::runtime_error( RED + "Error: Invalid port" \
												+ WHITE + " - expected a value between " \
												+ GREEN + "1024" \
												+ WHITE + " and " \
												+ GREEN + "49151" + WHITE + "." + CRLFNL)
# define	ERR_INVALID_PASSWORD				std::runtime_error( ERROR \
												+ WHITE + "Empty password" + CRLFNL )
# define	ERR_SOCKET_CREATION					std::runtime_error( ERROR \
												+ WHITE + "Server socket creation failed ..." + CRLFNL)
# define	ERR_SO_REUSEADDR					std::runtime_error( ERROR \
												+ WHITE + "Failed to set option (SO_REUSEADDR) on server socket ..." + CRLFNL)
# define	ERR_O_NONBLOCK						std::runtime_error( ERROR \
												+ WHITE + "Failed to set option (O_NONBLOCK) on server socket ..." + CRLFNL)
# define	ERR_BINDING_SOCKET					std::runtime_error( ERROR \
												+ WHITE + "failed to bind socket ...\n" + "Check if server is not already running in another terminal." + CRLFNL)
# define	ERR_LISTEN							std::runtime_error( ERROR \
												+ WHITE + "Listen function failed ..." + CRLFNL)
# define	ERR_POLL							std::runtime_error( ERROR \
												+ WHITE + "Poll failed ..." + CRLFNL)
# define	ERR_FIND_CLIENT						std::runtime_error( ERROR \
												+ WHITE + "Client not found ..." + CRLFNL)
# define 	ERR_ACCEPT_FAILED					RED << "accept() failed" << WHITE << CRLFNL
# define 	ERR_FCNTL_FAILED( ClientFd )		WHITE << "Failed to set option (O_NONBLOCK) for client " \
												<< RED << ClientFd << WHITE << "." << CRLFNL
# define	SERVER_OFF							std::runtime_error( WHITE + "SERVER CLOSED." + CRLFNL )

# define	NEW_CLIENT( ClientFd )				WHITE << "Client < " \
												<< YELLOW << ClientFd \
												<< WHITE << " > " \
												<< GREEN << "CONNECTED." << WHITE << CRLF
# define	CLIENT_DISCONNECTED( ClientFd )		WHITE << "Client < " \
												<< YELLOW << ClientFd \
												<< WHITE << " > " \
												<< RED << "DISCONNECTED." << WHITE << CRLF
# define	SERVER_CONNECTED( _SocketFd )		WHITE << "  Server " \
												<< GREEN << "CONNECTED" \
												<< WHITE << " !!\tListening on FD (" \
												<< YELLOW << _SocketFd \
												<< WHITE << ")." << WHITE << CRLF
# define	WAITING_CONNECTION					WHITE << "\tWaiting connection..." << CRLF
# define 	PRINT_DATA( ClientFd, Data )		WHITE << "Client < " \
												<< YELLOW << ClientFd \
												<< WHITE << " > " << Data << CRLF
# define	ERR_CMD_ARGS( token, msg )			BLUE + "Wrong usage of : " + token + "\nUse like this : " + token + " " + msg + WHITE + CRLFNL
# define	ERR_CMD_NOT_FOUND( Cmd )			BLUE + "Unknown command: " + Cmd + "\nType " \
												+ GREEN + "HELP " \
												+ BLUE + "to see the list of available commands." + WHITE + CRLFNL
# define	ERR_INEXISTANT_CHANNEL( topic )		BLUE + "Unknown channel: " + YELLOW + topic + BLUE + "\nType " \
												+ GREEN + "CHANNEL_ON " \
												+ BLUE + "to see the list of available channels." + WHITE + CRLFNL
# define	ERR_INEXISTANT_CLIENT( name )		BLUE + "Unknown client: " + YELLOW +  name + "\nType a valid client." + WHITE + CRLFNL
# define	ERR_NOT_OPERATOR( chanName )		YELLOW + "You are not operator in the channel " + YELLOW + chanName + "." + WHITE + CRLFNL
# define	ERR_NOT_IN_CHANNEL( chanName )		YELLOW + "You are not in " + YELLOW + chanName + " channel." + WHITE + CRLFNL
# define	ERR_CLI_NOT_IN_CHAN( cli, chan )	YELLOW + cli + WHITE + "are not in " + YELLOW + chan + " channel." + WHITE + CRLFNL
# define	ERR_NICKNAME_USED( nickname )		RED + "INFO:" + YELLOW + nickname + " Nickname is already used." + WHITE + CRLFNL
# define	ERR_TOPIC_USED( topic )				RED + "INFO:" + YELLOW + topic + " topic is already used by an other channel." + WHITE + CRLFNL
# define	ERR_NOT_TOPIC_FOUND( topic )		RED + "INFO:" + YELLOW + topic + " no channel match with this topic." + WHITE + CRLFNL
# define	COMMAND_LIST						BLUE + "Available commands:" + CRLFNL \
												+ GREEN + "  - USER" + BLUE + " : set your username\n" \
												+ GREEN + "  - NICK" + BLUE + " : choose your nickname\n" \
												+ GREEN + "  - PASS" + BLUE + " : set your password\n" \
												+ GREEN + "  - TOPIC" + BLUE + " : change the channel topic\n" \
												+ GREEN + "  - KICK" + BLUE + " : remove a user from a channel\n" \
												+ GREEN + "  - INVITE" + BLUE + " : invite a user to a channel\n" \
												+ GREEN + "  - JOIN" + BLUE + " : join a channel\n" \
												+ GREEN + "  - MODE" + BLUE + " : change channel mode\n" \
												+ GREEN + "  - HELP_MODE" + BLUE + " : show MODE flags and usage\n" \
												+ GREEN + "  - CHANNEL" + BLUE + " : send a message to a channel\n" \
												+ GREEN + "  - CHANNEL_ON" + BLUE + " : list active channels\n" \
												+ GREEN + "  - PRIVMSG" + BLUE + " : send a private message\n" + WHITE
# define	HELP_MODE							BLUE + "MODE - Change the channel’s mode:\n" \
												+ BLUE + "Usage: " + YELLOW + "MODE <channel> <flag> [value]\n\n" \
												+ "· " + GREEN + "i: " + BLUE + "Set/remove Invite-only channel\n" \
												+ "· " + GREEN + "t: " + BLUE + "Set/remove the restrictions of the TOPIC command to channel operators\n" \
												+ "· " + GREEN + "k: " + BLUE + "Set/remove the channel key (password)\n" \
												+ "· " + GREEN + "o: " + BLUE + "Give/take channel operator privilege\n" \
												+ "· " + GREEN + "l: " + BLUE + "Set/remove the user limit to channel. 0 to none." + WHITE + CRLFNL
# define	MSG_NEW_CLIENT( NewFd )				PURPLE + "\n\tWelcome to IRC\n" \
												+ BLUE + "First steps to use IRC. Register with :\n" \
												+ GREEN + " - USER <your username>\n" \
												+ GREEN + " - PASS <your password>" + WHITE + CRLFNL
# define	UPDATE_USERNAME( username )			GREEN + "Your username has been successfully processed.\n" \
												+ BLUE + "New username : " + YELLOW + username + WHITE + CRLFNL
# define	UPDATE_NICKNAME( nickname )			GREEN + "Your nickname has been successfully processed.\n" \
												+ BLUE + "New nickname : " + YELLOW + nickname + WHITE + CRLFNL
# define	UPDATE_PASSWORD						GREEN + "Your password has been successfully processed." + WHITE + CRLFNL
# define	SERVER_CLOSED						RED + "\tINFO: " \
												+ BLUE + "IRC server has been closed. Thank you." + WHITE + CRLFNL
# define	REGISTERED( status )				GREEN + "You have been correctly " + YELLOW + status + GREEN + "." + WHITE + CRLFNL
# define	CHANNEL_MESSAGE(top, nick, msg)		YELLOW + top + GREEN + " : " + YELLOW + nick + GREEN + " : " + WHITE + msg + CRLFNL
# define	RECEIVED_INVITE( topic )			YELLOW + topic + WHITE + " send you an invite." + CRLFNL
# define	RECEIVED_KICK( topic )				YELLOW + "You have been " + RED + "kicked " + BLUE + "from " + RED + topic + WHITE + CRLFNL
# define	CHANNEL_OPERATOR( topic )			GREEN + "Channel " + YELLOW + topic + GREEN + " mode updated." + WHITE + CRLFNL
# define	CREATE_CHANNEL( topic )				BLUE + "Channel " + YELLOW + topic + BLUE + " has been successfully created." + WHITE + CRLFNL
# define	ALREADY_IN_CHANNEL( Topic )			BLUE + "You are already in channel " + YELLOW + Topic + BLUE + "." + WHITE + CRLFNL
# define	NEW_TOPIC_SET( old, nTopic, name )	BLUE + "Channel topic has been set to " + YELLOW + nTopic + BLUE + " by " + YELLOW + name + BLUE + ". (OLD: " + YELLOW + old + BLUE + ")" + WHITE + CRLFNL
# define	CLIENT_KICKED( topic, name )		BLUE + "Client named " \
												+ YELLOW + name \
												+ BLUE + " has been kicked from the " \
												+ YELLOW + topic + BLUE + " channel." + WHITE + CRLFNL
# define	CLIENT_INVITED( topic, name )		GREEN + "Client named " \
												+ YELLOW + name \
												+ GREEN + " has been invite to the " \
												+ YELLOW + topic + GREEN + " channel." + WHITE + CRLFNL
# define	NEW_CLIENT_JOIN( nick, topic )		YELLOW + nick + GREEN + " has joined channel " + YELLOW + topic + GREEN + "." + WHITE + CRLFNL
# define	PRIV_MSG( sender, message )			YELLOW + sender + GREEN + " : " + WHITE + message + CRLFNL
# define	WRONG_PASSWORD( channel )			RED + "Error:" + GREEN + "Wrong password to access to " + YELLOW + channel + GREEN + " channel" + WHITE + CRLFNL
# define	SET_PSWD_RESTRICTION( channel )		YELLOW + channel + GREEN + " channel password restriction updated." + WHITE + CRLFNL


class runtime_error : public std::exception
{
	public:
		explicit runtime_error( const std::string& message );

		explicit runtime_error( const char *message );
};