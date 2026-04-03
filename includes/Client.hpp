/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 13:58:42 by jdupuis           #+#    #+#             */
/*   Updated: 2026/03/31 14:17:42 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include <exception>
# include "Include.hpp"

class Client
{
	private:
		int			_fd;
		std::string	_ipAddress;

	public:
		~Client();
		Client();
		Client(  );
		Client( const Client &copy );
		Client &operator=( const Client &instance );

		int			getFd( void ) const;
		std::string	getIpAddress( void ) const;

		void		setFd( int );
		void		setIpAddress( std::string );
};

#endif