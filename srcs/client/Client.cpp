#include "../../includes/Client.hpp"

Client::~Client( void ) {}

Client::Client( void ) {}

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

void Client::setFd( int fd )
{
	this->_fd = fd;
}

void Client::setIpAddress( std::string add )
{
	this->_ipAddress = add;
}

