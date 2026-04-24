/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:15 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/23 18:39:32 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Include.hpp"

class Channel;

class Client
{
	private:
		int			_fd;
		std::string	_ipAddress;
		std::string _username;
		std::string _nickname;
		std::string _password;
		bool		_registered;

		bool		_isAuth;
		bool		_isLog;
		std::string _buffer;


	public:
		~Client();
		Client();
		Client( std::string );
		Client( const Client &copy );
		Client &operator=( const Client &instance );

		int			getFd( void ) const;
		std::string	getIpAddress( void ) const;
		std::string	getUsername( void ) const;
		std::string	getNickname( void ) const;
		std::string getPassword( void ) const;
		std::string extractNextMessage();

		bool		getRegistered( void ) const;

		void		setFd( int );
		void		setIpAddress( std::string );
		void		setUsername( std::string );
		void		setNickname( std::string );
		void		setPassword( std::string );
		void		setRegistered( bool );
};
