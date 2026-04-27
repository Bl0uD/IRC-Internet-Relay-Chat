# include "../../includes/Server.hpp"

bool	Server::IsRegistered( Client *client )
{
	if ( client->getRegistered() )
		return true ;
	SendToClient( client, GREEN + "Register first.\nSet a username with " \
		+ YELLOW + "USER" \
		+ GREEN + " and a password with " \
		+ YELLOW + "PASS" + GREEN + "." + WHITE );
	return false ;
}

Client	*Server::FindClientWithNickname( std::string Nickname )
{
	for ( std::vector<Client>::iterator it = this->_Clients.begin(); it != this->_Clients.end(); ++it )
	{
		if ( it->getNickname() == Nickname )
			return ( &(*it) );
	}

	return ( NULL );
}

Client	*Server::FindClientWithFd( int fd )
{
	for ( std::vector<Client>::iterator it = this->_Clients.begin(); it != this->_Clients.end(); ++it )
	{
		if ( it->getFd() == fd )
			return ( &(*it) );
	}

	return ( NULL );
}

std::string Client::extractNextMessage()
{
	size_t pos = this->_buffer.find("\n");
	if (pos != std::string::npos)
	{
		std::string message = this->_buffer.substr(0, pos);
		this->_buffer.erase(0, pos + 1);
		return message;
	}
	return "";
}

std::string Client::getPrefix() const
{
	std::string nickname = this->getNickname();
	std::string username = this->_username;
	std::string hostname = this->_hostname;

	if ( nickname.empty() )
		nickname = "*";
	if ( username.empty() )
		username = "unknown";
	if ( hostname.empty() )
		hostname = this->_ipAddress.empty() ? "localhost" : this->_ipAddress;
	return ( nickname + "!" + username + "@" + hostname );
}