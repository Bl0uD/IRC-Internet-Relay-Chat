

# include "../../includes/Server.hpp"

Channel::~Channel( void ) {}

Channel::Channel( void ) :
	_inviteOnly( false ),
	_topicRestriction( false ),
	_passwordRestriction( false ),
	_id( 0 ),
	_userLimitation( 0 ),
	_password( "" ),
	_topic( "" ),
	_clients(),
	_operators(),
	_pendingClients()
{}

Channel::Channel( std::string topic, std::string paswrd ) :
	_inviteOnly( false ),
	_topicRestriction( false ),
	_passwordRestriction( !paswrd.empty() ),
	_id( 0 ),
	_userLimitation( 0 ),
	_password( paswrd ),
	_topic( topic ),
	_clients(),
	_operators(),
	_pendingClients()
{}

Channel::Channel( Channel const &copy )
{
	*this = copy;
}

Channel	&Channel::operator=( Channel const &instance )
{
	if ( this != &instance )
	{
		this->_inviteOnly = instance._inviteOnly;
		this->_topicRestriction = instance._topicRestriction;
		this->_passwordRestriction = instance._passwordRestriction;
		this->_id = instance._id;
		this->_userLimitation = instance._userLimitation;
		this->_password = instance._password;
		this->_name = instance._name;
		this->_topic = instance._topic;
		this->_clients = instance._clients;
		this->_operators = instance._operators;
		this->_pendingClients = instance._pendingClients;
		this->_mods = instance._mods;
		this->_key = instance._key;
	}
	return ( *this );
}

std::string	Channel::getTopic( void ) const
{
	return ( this->_topic );
}

void	Channel::setTopic( std::string newTopic )
{
	this->_topic = newTopic;
}

std::string	Channel::getName( void ) const
{
	return ( this->_name );
}

void	Channel::setName( std::string newName )
{
	this->_name = newName;
}

std::set<char> Channel::getMods() const
{
	return this->_mods;
}

void	Channel::setMods(Server *server, Client *client, char sign, char mod)
{
	if ( sign == '-' )
	{
		if ( mod == 't' )
		{
			this->_topicRestriction = false;
			server->SendToChannel( client, this, RPL_MODE( this->_name, "-t " ), true );
		}
		if ( mod == 'i' )
		{
			this->_inviteOnly = false;
			server->SendToChannel( client, this, RPL_MODE( this->_name, "-i " ), true );
		}
		this->_mods.erase( mod );
	}
	else
	{
		if ( mod == 't' )
		{
			this->_topicRestriction = true;
			server->SendToChannel( client, this, RPL_MODE(this->_name, "+t "), true );
		}
		if ( mod == 'i' )
		{
			this->_inviteOnly = true;
			server->SendToChannel( client, this, RPL_MODE(this->_name, "+i "), true );
		}
		this->_mods.insert( mod );
	}
}

std::string	Channel::getPassword( void ) const
{
	return ( this->_password );
}

void	Channel::setPassword( std::string newPassword )
{
	this->_password = newPassword;
	this->_passwordRestriction = !newPassword.empty();
}

bool	Channel::getInviteOnly( void ) const
{
	return ( this->_inviteOnly );
}

void	Channel::setInviteOnly( bool regis )
{
	this->_inviteOnly = regis;
}

bool	Channel::getTopicRestriction( void ) const
{
	return ( this->_topicRestriction );
}

void	Channel::setTopicRestriction( bool restriction )
{
	this->_topicRestriction = restriction;
}

bool	Channel::getPasswordRestriction( void ) const
{
	return ( this->_passwordRestriction );
}

void	Channel::setPasswordRestriction( bool restriction )
{
	this->_passwordRestriction = restriction;
	if ( !restriction )
		this->_password.clear();
}

int	Channel::getId( void ) const
{
	return ( this->_id );
}

void	Channel::setId( int newId )
{
	this->_id = newId;
}

int	Channel::getUserLimitation( void ) const
{
	return ( this->_userLimitation );
}

const std::set< int >	&Channel::getClients( void ) const
{
	return ( this->_clients );
}

const std::set< int >	&Channel::getOperators( void ) const
{
	return ( this->_operators );
}

void	Channel::addClient( int newClient )
{
	this->_clients.insert( newClient );
}

void	Channel::addPendingClient( int newClient )
{
	this->_pendingClients.insert( newClient );
}


void	Channel::setOperator( Server *server, Client *client, Client *target, char sign )
{
	for ( std::set<int>::iterator it = this->_clients.begin(); it != this->_clients.end(); ++it )
	{
		if ( (*it) == target->getFd() ) 
		{
			if ( sign == '-' && IsOperator( target, this ) )
			{
				this->_operators.erase( target->getFd() );
				server->SendToChannel( client, this, RPL_MODE( this->_name, "-o " + target->getNickname()), true );
				return ;
			}
			else if (sign == '+' && !IsOperator( target, this ) )
			{
				this->_operators.insert( *it );
				server->SendToChannel( client, this, RPL_MODE( this->_name, "+o " + target->getNickname()), true );
				SendToClient( client, RPL_YOUREOPER );
				return ;
			}
		}
	}
}

bool	Channel::hasPendingClient( int clientFd ) const
{
	return ( this->_pendingClients.find( clientFd ) != this->_pendingClients.end() );
}

bool	Channel::removeClient( Client *client )
{ 
	if ( isClientInChannel( client ) == true )
	{
		this->_clients.erase( client->getFd() );
		if (this->_clients.empty())
			return ( false ); // Return false si le client etait le dernier du channel
	}
	return (true);
}

void	Channel::removeOperator( int clientFd )
{
	this->_operators.erase( clientFd );
}

void	Channel::removePendingClient( int clientFd )
{
	this->_pendingClients.erase( clientFd );
}

