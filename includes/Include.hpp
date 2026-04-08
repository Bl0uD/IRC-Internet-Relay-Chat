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

# define	DISPLAY_SERVER		PURPLE << "\n   ☑️\t Server launched !\n\n"

# define	CRLF "\r\n"
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
# define	ERR_CMD_ARGS( token, msg )		WHITE << "Wrong usage of : " << token << "\nUse like this : " << token << " " << msg << CRLF
# define	ERR_CMD_NOT_FOUND( Cmd )		WHITE + "Unknown command: " + Cmd + "\nType HELP to see the list of available commands." + CRLF
# define	COMMAND_LIST					WHITE + "Available commands:" + CRLF \
											+ GREEN + "  - USER" + WHITE + " : set your username" + CRLF \
											+ GREEN + "  - NICK" + WHITE + " : choose your nickname" + CRLF \
											+ GREEN + "  - TOPIC" + WHITE + " : change the channel topic" + CRLF \
											+ GREEN + "  - KICK" + WHITE + " : remove a user from a channel" + CRLF \
											+ GREEN + "  - INVITE" + WHITE + " : invite a user to a channel" + CRLF \
											+ GREEN + "  - JOIN" + WHITE + " : join a channel" + CRLF \
											+ GREEN + "  - MODE" + WHITE + " : change channel mode" + CRLF \
											+ GREEN + "  - PRIVMSG" + WHITE + " : send a private message" + CRLF \
											+ GREEN + "  - HELP" + WHITE + " : show this list" + CRLF

class runtime_error : public std::exception
{
	public:
		explicit runtime_error( const std::string& message );

		explicit runtime_error( const char *message );
};