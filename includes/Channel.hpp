# pragma once

# include "Include.hpp"

class Channel
{
	private:
		bool					_inviteOnly;
		bool					_topicRestriction;
		bool					_passwordRestriction;
		int						_id;
		int						_userLimitation;
		std::string				_password;
		std::string				_name;
		std::string				_topic;
		std::set< int >			_clients;
		std::set< int >			_operators;
		std::set< int >			_pendingClients;
		std::set<char>			_mods;
		std::string				_key;

	public:
		~Channel();
		Channel();
		Channel( std::string topic, std::string paswrd );
		Channel( const Channel &copy );
		Channel &operator=( const Channel &instance );

		std::string				getTopic( void ) const;
		void					setTopic( std::string );
		std::string				getName( void ) const;
		void					setName( std::string );
		std::set<char>			getMods() const;
		void				setMods( Server *server, Client *client, char sign, char mod );
		std::string				getPassword( void ) const;
		void					setPassword( std::string );
		bool					getInviteOnly( void ) const;
		void					setInviteOnly( bool );
		bool					getTopicRestriction( void ) const;
		void					setTopicRestriction( bool );
		bool					getPasswordRestriction( void ) const;
		void					setPasswordRestriction( bool );
		int						getId( void ) const;
		void					setId( int );
		int						getUserLimitation( void ) const;
		void				setKey( Server *server, Client *client,  char sign, std::string _key );
		const std::set< int >	&getClients( void ) const;
		const std::set< int >	&getOperators( void ) const;
		void					addClient( int );
		void					addPendingClient( int );
		void				setOperator( Server *server, Client *, Client *, char );
		bool					hasPendingClient( int ) const;
		bool					removeClient( Client * );
		void				removeOperator( int );
		void					removePendingClient( int );
		bool					isClientInChannel( Client *client );
		bool					isClientAdmin( Client *client );
		std::string				getModsForReply() const;

		void				setUserLimitation( Server *server, Client *client, char sign, std::string limit );
};