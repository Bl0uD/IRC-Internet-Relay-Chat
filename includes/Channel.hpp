# pragma once

#include "Include.hpp"

class Channel
{
	private:
		bool				_inviteOnly;
		bool				_topicRestriction;
		bool				_passwordRestriction;
		int					_id;
		int					_userLimitation;
		std::string			_password;
		std::string			_topic;
		std::vector< int >	_clients;
		std::vector< int >	_operators;
		std::vector< int >	_pendingClients;

	public:
		~Channel();
		Channel();
		Channel( std::string topic, std::string paswrd );
		Channel( const Channel &copy );
		Channel &operator=( const Channel &instance );

		std::string			getTopic( void ) const;
		void				setTopic( std::string );
		std::string			getPassword( void ) const;
		void				setPassword( std::string );
		bool				getInviteOnly( void ) const;
		void				setInviteOnly( void );
		bool				getTopicRestriction( void ) const;
		void				setTopicRestriction( void );
		bool				getPasswordRestriction( void ) const;
		void				setPasswordRestriction( void );
		int					getId( void ) const;
		void				setId( int );
		int					getUserLimitation( void ) const;
		void				setUserLimitation( int );
		std::vector< int >	getClients( void ) const;
		std::vector< int >	getOperators( void ) const;
		void				addClient( int );
		void				addPendingClient( int );
		void				addOperator( int );
		void				removeClient( int );
		void				removeOperator( int );
};