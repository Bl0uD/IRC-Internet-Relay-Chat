/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/01 03:03:25 by jdupuis           #+#    #+#             */
/*   Updated: 2026/05/01 03:09:30 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include "Include.hpp"

class Channel
{
	private:
		bool                    _inviteOnly;
		bool                    _topicRestriction;
		bool                    _passwordRestriction;
		int                     _id;
		int                     _userLimitation;
		std::string             _password;
		std::string             _name;
		std::string             _topic;
		std::set< int >         _clients;
		std::set< int >         _operators;
		std::set< int >         _pendingClients;
		std::set<char>          _mods;
		std::string             _key;

	public:
		Channel();
		Channel( std::string topic, std::string paswrd );
		Channel( const Channel &copy );
		~Channel();
		Channel &operator=( const Channel &instance );

		const std::set< int >  &getClients( void ) const;
		std::string             getModsForReply() const;
		std::set<char>          getMods() const;
		int                     getId( void ) const;
		std::string             getName( void ) const;
		std::string             getPassword( void ) const;
		std::string             getTopic( void ) const;
		int                     getUserLimitation( void ) const;
		const std::set< int >  &getOperators( void ) const;

		void                    addClient( int );
		void                    addPendingClient( int );
		void                    removePendingClient( int );
		void                    setId( int );
		void                    setInviteOnly( bool );
		void                    setKey( Server *server, Client *client,  char sign, std::string _key );
		void                    setMods( Server *server, Client *client, char sign, char mod );
		void                    setName( std::string );
		void                    setPassword( std::string );
		void                    setPasswordRestriction( bool );
		void                    setTopic( std::string );
		void                    setTopicRestriction( bool );
		void                    setUserLimitation( Server *server, Client *client, char sign, std::string limit );
        
		bool                    isClientInChannel( Client *client );
		bool                    hasPendingClient( int ) const;
		bool                    removeClient( Client * );
		void                    removeOperator( int );

		void                    setOperator( Server *server, Client *, Client *, char );
		bool                    isClientAdmin( Client *client );

		bool                    getInviteOnly( void ) const;
		bool                    getPasswordRestriction( void ) const;
		bool                    getTopicRestriction( void ) const;
};