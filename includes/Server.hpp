/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 13:58:44 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/02 18:38:40 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <exception>
#include "Include.hpp"

class Server
{
	private:
		int	_port;
		std::string _password;

	public:
		~Server();
		Server();
		Server( char ** );
		Server( const Server &copy );
		Server &operator=( const Server &instance );

		int			getPort( void ) const;
		std::string	getPassword( void ) const;

		void		setPort( int );
		void		setPassword( std::string );
		
	
};
