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
		std::set< int >		_clients;
		std::set< int >		_operators;
		std::set< int >		_pendingClients;

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
		const std::set< int >	&getClients( void ) const;
		const std::set< int >	&getOperators( void ) const;
		void				addClient( int );
		void				addPendingClient( int );
		void				addOperator( int );
		bool				hasPendingClient( int ) const;
		void				removeClient( int );
		void				removePendingClient( int );
		void				removeOperator( int );
};