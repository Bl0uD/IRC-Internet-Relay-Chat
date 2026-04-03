#pragma once

# include <arpa/inet.h> //-> for inet_ntoa()
# include <csignal> //-> for signal()
# include <exception>
# include <fcntl.h> //-> for fcntl()
# include <iostream>
# include <netinet/in.h> //-> for sockaddr_in
# include <poll.h> //-> for poll()
# include <unistd.h> //-> for close()
# include <vector> //-> for vector
# include <stdexcept>
# include <string>
# include <sys/socket.h> //-> for socket()
# include <sys/types.h> //-> for socket()
# include <sstream> //-> for stringstrea#include <iostream>

# define	GREEN std::string("\e[1;32m") //-> for green color
# define	RED std::string("\e[1;31m") //-> for red color
# define	WHITE std::string("\e[0;37m") //-> for white color
# define	YELLOW std::string("\e[1;33m") //-> for yellow color
# define	PURPLE std::string("\e[1;35m") //-> for purple color

# define	DISPLAY_SERVER		PURPLE << "\n   ☑️\t Server launched !\n\n"

# define	CRLF "\r\n"
# define	WRONG_USAGE	RED << "\n   ⚠️\t WRONG USAGE !\n\n"
# define	ERROR		RED + CRLF + "🚨 Error:\t" + WHITE
			// Used in main.cpp
# define	ERR_WRONG_USAGE 		WRONG_USAGE \
									<< WHITE << "Try:  ./ircserv " \
									<< GREEN << "<" \
									<< WHITE << "port" \
									<< GREEN << "> <" \
									<< WHITE << "password" \
									<< GREEN << ">" \
									<< WHITE << "\n\n"
# define	ERR_INVALID_PORT		std::runtime_error( RED + "Error: Invalid port" \
									+ WHITE + " - expected a value between " \
									+ GREEN + "1024" \
									+ WHITE + " and " \
									+ GREEN + "49151" \
									+ WHITE + "." + CRLF)
# define	ERR_INVALID_PASSWORD	std::runtime_error( ERROR \
									+ WHITE + "Empty password" + CRLF )
# define	ERR_SOCKET_CREATION		std::runtime_error( ERROR \
									+ WHITE + "Socket creation failed ..." + CRLF)
# define	ERR_SO_REUSEADDR		std::runtime_error( ERROR \
									+ WHITE + "Failed to set option (SO_REUSEADDR) on socket ..." + CRLF)
# define	ERR_O_NONBLOCK			std::runtime_error( ERROR \
									+ WHITE + "Failed to set option (O_NONBLOCK) on socket ..." + CRLF)
# define	ERR_BINDING_SOCKET			std::runtime_error( ERROR \
									+ WHITE + "failed to bind socket ..." + CRLF)
# define	ERR_LISTEN				std::runtime_error( ERROR \
									+ WHITE + "Listen function failed ..." + CRLF)
# define	ERR_POLL				std::runtime_error( ERROR \
									+ WHITE + "Poll failed ..." + CRLF)
# define	SERVER_OFF				std::runtime_error( WHITE + "Server closed" + CRLF)


class runtime_error : public std::exception
{
	public:
		explicit runtime_error( const std::string& message );

		explicit runtime_error( const char *message );
};