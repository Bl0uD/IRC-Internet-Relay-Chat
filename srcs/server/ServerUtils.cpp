# include "../../includes/Server.hpp"

void	Server::respond( Client *client, std::string message )
{
	std::string prefix = this->getPrefix();
	if ( prefix.empty() )
		SendToClient( client, message );
	else
		SendToClient( client, ":" + prefix + " " + message );
}

void	Server::Parse( std::string message ) 
{
	//std::string message = data; //jsp si ca marche mais il nous faut une string
	size_t		pos = 0;
	std::string	res;
	Parser		parsed;

	if (!this->_parsedMessages.empty())
		this->_parsedMessages.clear();

	message.erase( std::remove( message.begin(), message.end(), '\r' ), message.end() );
	while ( getStringUntil( message, res, '\n', pos ) )
	{
		if (res != "") {
			parsed.parseMessage( res );
			this->_parsedMessages.push_back( parsed );
		}
		res = "";
	}
}

std::string Server::getPrefix()
{
	return this->_hostName;
	
	struct hostent *hostEntry = gethostbyname( this->_hostName.c_str() );
	if ( !hostEntry )
		throw std::runtime_error( "Error: gethostbyname failed" );
	return hostEntry->h_name;
}

void	Server::SendWelcome( Client *client )
{
	if ( !client || client->getRegistered() )
		return ;
	client->setRegistered( true );
	this->respond( client, RPL_WELCOME( client->getNickname(), client->getPrefix() ) );
	this->respond( client, RPL_YOURHOST( client->getNickname(), this->getPrefix(), "ircserv" ) );
	this->respond( client, RPL_CREATED( client->getNickname(), __DATE__ ) );
	this->respond( client, RPL_MYINFO( client->getNickname(), this->getPrefix(), "ircserv" ) );
}