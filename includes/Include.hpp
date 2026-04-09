#pragma once

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

# define	GREEN std::string("\e[1;32m") // for green color
# define	RED std::string("\e[1;31m") // for red color
# define	WHITE std::string("\e[0;37m") // for white color
# define	YELLOW std::string("\e[1;33m") // for yellow color
# define	PURPLE std::string("\e[1;35m") // for purple color
# define	BLUE std::string("\e[1;34m") // for blue color

# define	DISPLAY_SERVER		PURPLE << "\n   ☑️\t Server launched !\n\n"

# define	CRLF "\r\n\n"
# define	WRONG_USAGE	RED << "\n   ⚠️\t WRONG USAGE !\n\n"
# define	ERROR		RED + CRLF + "🚨 Error:\t" + WHITE

# define	ERR_WRONG_USAGE 				WRONG_USAGE \
											<< WHITE << "Try:  ./ircserv " \
											<< GREEN << "<" \
											<< WHITE << "port" \
											<< GREEN << "> <" \
											<< WHITE << "password" \
											<< GREEN << ">" \
											<< WHITE << "\n\n"
# define	ERR_INVALID_PORT				std::runtime_error( RED + "Error: Invalid port" \
											+ WHITE + " - expected a value between " \
											+ GREEN + "1024" \
											+ WHITE + " and " \
											+ GREEN + "49151" \
											+ WHITE + "." + CRLF)
# define	ERR_INVALID_PASSWORD			std::runtime_error( ERROR \
											+ WHITE + "Empty password" + CRLF )
# define	ERR_SOCKET_CREATION				std::runtime_error( ERROR \
											+ WHITE + "Server socket creation failed ..." + CRLF)
# define	ERR_SO_REUSEADDR				std::runtime_error( ERROR \
											+ WHITE + "Failed to set option (SO_REUSEADDR) on server socket ..." + CRLF)
# define	ERR_O_NONBLOCK					std::runtime_error( ERROR \
											+ WHITE + "Failed to set option (O_NONBLOCK) on server socket ..." + CRLF)
# define	ERR_BINDING_SOCKET				std::runtime_error( ERROR \
											+ WHITE + "failed to bind socket ..." + CRLF)
# define	ERR_LISTEN						std::runtime_error( ERROR \
											+ WHITE + "Listen function failed ..." + CRLF)
# define	ERR_POLL						std::runtime_error( ERROR \
											+ WHITE + "Poll failed ..." + CRLF)
# define 	ERR_ACCEPT_FAILED				RED << "accept() failed" \
											<< WHITE << CRLF
# define 	ERR_FCNTL_FAILED( ClientFd )	WHITE << "Failed to set option (O_NONBLOCK) for client " \
											<< RED << ClientFd \
											<< WHITE << "." << CRLF
# define	SERVER_OFF						std::runtime_error( WHITE + "SERVER CLOSED." + CRLF)

# define	NEW_CLIENT( ClientFd )			WHITE << "Client < " \
											<< YELLOW << ClientFd \
											<< WHITE << " > " \
											<< GREEN << "CONNECTED.\n" \
											<< WHITE << CRLF
# define	CLIENT_DISCONNECTED( ClientFd )	WHITE << "Client < " \
											<< YELLOW << ClientFd \
											<< WHITE << " > " \
											<< RED << "DISCONNECTED.\n" \
											<< WHITE << CRLF
# define	SERVER_CONNECTED( _SocketFd )	WHITE << "  Server " \
											<< GREEN << "CONNECTED" \
											<< WHITE << " !!\tListening on FD (" \
											<< YELLOW << _SocketFd \
											<< WHITE << ").\n" \
											<< WHITE << CRLF
# define	WAITING_CONNECTION				WHITE << "\tWaiting connection...\n" << CRLF
# define 	PRINT_DATA( ClientFd, Data )	WHITE << "Client < " \
											<< YELLOW << ClientFd \
											<< WHITE << " > " << Data << CRLF
# define	ERR_CMD_ARGS( token, msg )		BLUE + "Wrong usage of : " + token + "\nUse like this : " + token + " " + msg + WHITE + CRLF
# define	ERR_CMD_NOT_FOUND( Cmd )		BLUE + "Unknown command: " + Cmd + "\nType HELP to see the list of available commands." + WHITE + CRLF
# define	COMMAND_LIST					BLUE + "Available commands:" + CRLF \
											+ GREEN + "  - USER" + BLUE + " : set your username\n" \
											+ GREEN + "  - NICK" + BLUE + " : choose your nickname\n" \
											+ GREEN + "  - TOPIC" + BLUE + " : change the channel topic\n" \
											+ GREEN + "  - KICK" + BLUE + " : remove a user from a channel\n" \
											+ GREEN + "  - INVITE" + BLUE + " : invite a user to a channel\n" \
											+ GREEN + "  - JOIN" + BLUE + " : join a channel\n" \
											+ GREEN + "  - MODE" + BLUE + " : change channel mode\n" \
											+ GREEN + "  - PRIVMSG" + BLUE + " : send a private message\n" \
											+ GREEN + "  - HELP" + BLUE + " : show this list" + WHITE + CRLF
# define	MSG_NEW_CLIENT( NewFd )			PURPLE + "\n\tWelcome to IRC\n" \
											+ BLUE + "First steps to use IRC. Authentificate with :\n" \
											+ GREEN + "USER <your username>\n" \
											+ GREEN + "PASS <your password>" + WHITE + CRLF
# define	UPDATE_USERNAME( username )		GREEN + "Your username has been successfully processed.\n" \
											+ BLUE + "New username : " + username + WHITE + CRLF
# define	UPDATE_NICKNAME( nickname )		GREEN + "Your nickname has been successfully processed.\n" \
											+ BLUE + "New nickname : " + nickname + WHITE + CRLF
# define	UPDATE_PASSWORD					GREEN + "Your password has been successfully processed." + WHITE + CRLF

class runtime_error : public std::exception
{
	public:
		explicit runtime_error( const std::string& message );

		explicit runtime_error( const char *message );
};