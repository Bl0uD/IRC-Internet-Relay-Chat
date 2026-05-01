/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/01 03:03:27 by jdupuis           #+#    #+#             */
/*   Updated: 2026/05/01 03:09:36 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include "Include.hpp"

class Channel;

class Client
{
	private:
		int				_fd;
		std::string		_ipAddress;
		std::string		_username;
		std::string		_nickname;
		std::string		_hostname;
		std::string		_password;
		bool			_registered;
		bool			_isAuth;
		bool			_isLog;
		std::string		_buffer;

	public:
		Client();
		Client( std::string );
		Client( const Client &copy );
		~Client();
		Client &operator=( const Client &instance );

		int				getFd( void ) const;
		std::string		getIpAddress( void ) const;
		bool			getIsAuth( void ) const;
		bool			getIsLog( void ) const;
		std::string		getNickname( void ) const;
		std::string		getPassword( void ) const;
		std::string		getPrefix() const;
		bool			getRegistered( void ) const;
		std::string		getUsername( void ) const;

		void			setAuth( bool );
		void			setFd( int );
		void			setIpAddress( std::string );
		void			setLog( bool );
		void			setNickname( std::string );
		void			setPassword( std::string );
		void			setRegistered( bool );
		void			setUsername( std::string );

		std::string		extractNextMessage();
		void			appendBuffer( std::string );
};