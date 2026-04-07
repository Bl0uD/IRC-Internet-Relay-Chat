#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <exception>
# include <vector>

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
		void				addClients( int );
		void				addOperators( int );
		void				removeClients( int );
		void				removeOperators( int );
};

#endif