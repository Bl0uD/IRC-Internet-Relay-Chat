# include "../../includes/Client.hpp"

Client::~Client( void ) {}

Client::Client( void )
{

}

Client::Client( std::string a )
{
	(void)a;
}

Client::Client( Client const &copy )
{
	*this = copy;
}

Client	&Client::operator=( Client const &instance )
{
	this->_fd = instance._fd;
	this->_ipAddress = instance._ipAddress;
	return ( *this );
}

int	Client::getFd( void ) const
{
	return ( this->_fd );
}

std::string	Client::getIpAddress( void ) const
{
	return ( this->_ipAddress );
}

std::string	Client::getUsername( void ) const
{
	return ( this->_username );
}

std::string	Client::getNickname( void ) const
{
	return ( this->_nickname );
}

std::string	Client::getPassword( void ) const
{
	return ( this->_password );
}

void	Client::setFd( int fd )
{
	this->_fd = fd;
}

void	Client::setIpAddress( std::string add )
{
	this->_ipAddress = add;
}

void	Client::setUsername( std::string username )
{
	this->_username = username;
}

void	Client::setNickname( std::string nickname )
{
	this->_nickname = nickname;
}

void	Client::setPassword( std::string password )
{
	this->_password = password;
}
