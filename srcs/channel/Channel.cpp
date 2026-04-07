# include "../../includes/Channel.hpp"

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
	_operators()
{}

Channel::Channel( std::string topic, std::string paswrd ) :
	_inviteOnly( false ),
	_topicRestriction( false ),
	_passwordRestriction( false ),
	_id( 0 ),
	_userLimitation( 0 ),
	_password( paswrd ),
	_topic( topic ),
	_clients(),
	_operators()
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
}

bool	Channel::getInviteOnly( void ) const
{
	return ( this->_inviteOnly );
}

void	Channel::setInviteOnly( void )
{
	this->_inviteOnly = true;
}

bool	Channel::getTopicRestriction( void ) const
{
	return ( this->_topicRestriction );
}

void	Channel::setTopicRestriction( void )
{
	this->_topicRestriction = true;
}

bool	Channel::getPasswordRestriction( void ) const
{
	return ( this->_passwordRestriction );
}

void	Channel::setPasswordRestriction( void )
{
	this->_passwordRestriction = true;
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

void	Channel::setUserLimitation( int newlimit )
{
	this->_userLimitation = newlimit;
}

std::vector< int >	Channel::getClients( void ) const
{
	return ( this->_clients );
}

std::vector< int >	Channel::getOperators( void ) const
{
	return ( this->_operators );
}

void	Channel::addClients( int newClient )
{
	this->_clients.push_back( newClient );
}

void	Channel::addOperators( int newOperator )
{
	this->_operators.push_back( newOperator );
}

void	Channel::removeClients( int clientFd )
{
	std::vector< int >::iterator it = std::find( this->_clients.begin(), this->_clients.end(), clientFd );
	if ( it != this->_clients.end() )
		this->_clients.erase( it );
}

void	Channel::removeOperators( int operatorFd )
{
	std::vector< int >::iterator it = std::find( this->_operators.begin(), this->_operators.end(), operatorFd );
	if ( it != this->_operators.end() )
		this->_operators.erase( it );
}