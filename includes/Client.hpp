/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:15 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/24 14:53:28 by jdupuis          ###   ########.fr       */
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
		std::string	_hostname;
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
		std::string	getPrefix() const;
		bool		getRegistered( void ) const;
		bool		getIsAuth( void ) const;
		bool		getIsLog( void ) const;

		std::string extractNextMessage();

		void		setFd( int );
		void		setIpAddress( std::string );
		void		setUsername( std::string );
		void		setNickname( std::string );
		void		setPassword( std::string );
		void		setRegistered( bool );
		void		setLog( bool );
		void		setAuth( bool );
};
