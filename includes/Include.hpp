/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Include.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 13:58:40 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/02 17:42:3 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include <arpa/inet.h> //-> for inet_ntoa()
# include <csignal> //-> for signal()
# include <fcntl.h> //-> for fcntl()
# include <iostream>
# include <netinet/in.h> //-> for sockaddr_in
# include <poll.h> //-> for poll()
# include <unistd.h> //-> for close()
# include <vector> //-> for vector
# include <sys/socket.h> //-> for socket()
# include <sys/types.h> //-> for socket()
# include <sstream> //-> for stringstrea

# define	GREEN "\e[1;32m" //-> for green color
# define	RED "\e[1;31m" //-> for red color
# define	WHITE "\e[0;37m" //-> for white color
# define	YELLOW "\e[1;33m" //-> for yellow color
# define	PURPLE "\e[1;35m" //-> for purple color

# define	DISPLAY_SERVER		PURPLE << "\n   ☑️\t Server launched !\n\n"

# define	WRONG_USAGE	RED << "\n   ⚠️\t WRONG USAGE !\n\n"

			// Used in main.cpp
# define	ERR_WRONG_USAGE 	WRONG_USAGE \
								<< WHITE << "Try:  ./ircserv " \
								<< GREEN << "<" \
								<< WHITE << "port" \
								<< GREEN << "> <" \
								<< WHITE << "password" \
								<< GREEN << ">" \
								<< WHITE << "\n\n"
