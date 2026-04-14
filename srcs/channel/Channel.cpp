# include "../../includes/Channel.hpp"

Channel::~Channel( void ) {}

Channel::Channel( void ) :
	_inviteOnly( true ),
	_topicRestriction( true ),
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
	_inviteOnly( true ),
	_topicRestriction( true ),
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
		this->_topic = instance._topic;
		this->_clients = instance._clients;
		this->_operators = instance._operators;
		this->_pendingClients = instance._pendingClients;
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

void	Channel::setUserLimitation( int newLimit )
{
	if ( newLimit < 0 )
		newLimit = 0;
	this->_userLimitation = newLimit;
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

void	Channel::addOperator( int newOperator )
{
	this->_operators.insert( newOperator );
}

bool	Channel::hasPendingClient( int clientFd ) const
{
	return ( this->_pendingClients.find( clientFd ) != this->_pendingClients.end() );
}

void	Channel::removeClient( int clientFd )
{
	this->_clients.erase( clientFd );
}

void	Channel::removePendingClient( int clientFd )
{
	this->_pendingClients.erase( clientFd );
}

void	Channel::removeOperator( int operatorFd )
{
	this->_operators.erase( operatorFd );
}