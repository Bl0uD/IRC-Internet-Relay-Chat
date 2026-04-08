# include "../../includes/Server.hpp"

std::vector< std::string >	Server::Parse( char *Data )
{
	std::vector< std::string >	Tokens;
	std::string					line;
	std::string					token;

	if ( Data )
		line = Data;
	std::istringstream ss( line );

	while ( std::getline( ss, token, ' ' ) )
	{
		token.erase( std::remove( token.begin(), token.end(), '\r' ), token.end() );
		token.erase( std::remove( token.begin(), token.end(), '\n' ), token.end() );
		if ( !token.empty() )
			Tokens.push_back( token );
	}
	if ( !Tokens.empty() )
		std::transform( Tokens[0].begin(), Tokens[0].end(), Tokens[0].begin(), ::toupper );
	return ( Tokens );
}

int Server::FindClient( int ClientFd )
{
	int	i = 0;
	while ( this->_Clients[i].getFd() != ClientFd )
		i++;
	return ( i );
}

void	Server::SetUsername( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() != 2 )
	{
		std::cout << ERR_CMD_ARGS( "USER", "<your username>");
		return ;
	}
	int i = FindClient( ClientFd );
	this->_Clients[i].setUsername( Tokens[1] );
	this->SendToClient( ClientFd, ":ircserv NOTICE * :USER command accepted" );
}

void	Server::SetNickname(  std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() != 2 )
	{
		std::cout << ERR_CMD_ARGS( "NICK", "<your nickname>");
		return ;
	}
	int i = FindClient( ClientFd );
	this->_Clients[i].setNickname( Tokens[1] );
	this->SendToClient( ClientFd, ":ircserv NOTICE * :NICK command accepted" );
}

void	Server::ChangeTopic( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() < 3 )
	{
		std::cout << ERR_CMD_ARGS( "TOPIC", "<channel name> <new topic>" );
		return ;
	}

	std::string channelName = Tokens[1];
	std::string newTopic;
	int channelId = -1;

	for ( size_t i = 2; i < Tokens.size(); ++i )
	{
		if ( i > 2 )
			newTopic += " ";
		newTopic += Tokens[i];
	}
	if ( newTopic.size() > 0 && newTopic[0] == ':' )
		newTopic.erase( 0, 1 );

	for ( std::vector< std::pair<int, std::string> >::iterator it = this->_Directory.begin(); it != this->_Directory.end(); ++it )
	{
		if ( it->second == channelName )
		{
			channelId = it->first;
			break ;
		}
	}

	if ( channelId == -1 )
	{
		this->SendToClient( ClientFd, std::string( ":ircserv 403 " ) + channelName + " :No such channel" );
		return ;
	}

	for ( std::vector< Channel >::iterator it = this->_Channels.begin(); it != this->_Channels.end(); ++it )
	{
		if ( it->getId() == channelId )
		{
			it->setTopic( newTopic );
			this->SendToClient( ClientFd, std::string( ":ircserv NOTICE " ) + channelName + " :Topic updated" );
			return ;
		}
	}
}

void	Server::KickClient( std::vector<std::string> Tokens, int ClientFd )
{
	(void)Tokens;
	(void)ClientFd;
	std::cout << "func kick" << std::endl;
}

void	Server::InviteClient( std::vector<std::string> Tokens, int ClientFd )
{
	(void)Tokens;
	(void)ClientFd;
	std::cout << "func invite" << std::endl;
}

void	Server::JoinChannel( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() < 2 )
	{
		std::cout << ERR_CMD_ARGS( "JOIN", "<channel name>" );
		return ;
	}

	std::string channelName = Tokens[1];
	int channelId = -1;

	for ( std::vector< std::pair<int, std::string> >::iterator it = this->_Directory.begin(); it != this->_Directory.end(); ++it )
	{
		if ( it->second == channelName )
		{
			channelId = it->first;
			break ;
		}
	}

	if ( channelId == -1 )
	{
		Channel newChannel;

		channelId = this->_NextChannelId++;
		newChannel.setId( channelId );
		this->_Channels.push_back( newChannel );
		this->_Directory.push_back( std::make_pair( channelId, channelName ) );
	}

	for ( std::vector< Channel >::iterator it = this->_Channels.begin(); it != this->_Channels.end(); ++it )
	{
		if ( it->getId() == channelId )
		{
			if ( it->getClients().empty() )
				it->addOperators( ClientFd );
			it->addClients( ClientFd );
			std::cout << "func Join" << std::endl;
			return ;
		}
	}
}

void	Server::ChangeMode( std::vector<std::string> Tokens, int ClientFd )
{
	(void)Tokens;
	(void)ClientFd;
	std::cout << "func mode" << std::endl;
}

void	Server::SendPrivMsg( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() < 3 )
	{
		std::cout << ERR_CMD_ARGS( "PRIVMSG", "<nickname> <message>" );
		return ;
	}

	std::string targetNickname = Tokens[1];
	std::string message;
	std::string senderNickname;
	int senderIndex = FindClient( ClientFd );
	int targetFd = -1;

	if ( senderIndex >= 0 && senderIndex < static_cast<int>( this->_Clients.size() ) )
	{
		senderNickname = this->_Clients[senderIndex].getNickname();
		if ( senderNickname == "" )
			senderNickname = this->_Clients[senderIndex].getUsername();
	}
	if ( senderNickname == "" )
		senderNickname = "*";

	for ( size_t i = 0; i < this->_Clients.size(); ++i )
	{
		if ( this->_Clients[i].getNickname() == targetNickname )
		{
			targetFd = this->_Clients[i].getFd();
			break ;
		}
	}

	if ( targetFd == -1 )
	{
		this->SendToClient( ClientFd, std::string( ":ircserv 401 " ) + targetNickname + " :No such nick/channel" );
		return ;
	}

	for ( size_t i = 2; i < Tokens.size(); ++i )
	{
		if ( i > 2 )
			message += " ";
		message += Tokens[i];
	}
	if ( message.size() > 0 && message[0] == ':' )
		message.erase( 0, 1 );

	this->SendToClient( targetFd, std::string( ":" ) + senderNickname + " PRIVMSG " + targetNickname + " :" + message );
}

void	Server::ExecCommand( std::vector<std::string> Tokens, int ClientFd )
{
	std::string	cmds[9] = { "USER", "NICK", "TOPIC", "KICK", "INVITE", "JOIN", "MODE", "PRIVMSG", "HELP" };
	int			i = 0;
	if ( Tokens.empty() )
		return ;
	while ( i < 9 && Tokens[0] != cmds[i] )
		i++;
	switch ( i )
	{
		case 0:
			SetUsername( Tokens, ClientFd );
			break ;
		case 1:
			SetNickname( Tokens, ClientFd );
			break ;
		case 2:
			ChangeTopic( Tokens, ClientFd );
			break ;
		case 3:
			KickClient( Tokens, ClientFd );
			break ;
		case 4:
			InviteClient( Tokens, ClientFd );
			break ;
		case 5:
			JoinChannel( Tokens, ClientFd );
			break ;
		case 6:
			ChangeMode( Tokens, ClientFd );
			break ;
		case 7:
			SendPrivMsg( Tokens, ClientFd );
			break ;
		case 8:
			this->SendToClient( ClientFd, COMMAND_LIST );
			break ;
		default:
			this->SendToClient( ClientFd, ERR_CMD_NOT_FOUND( Tokens[0] ) );
			break ;
	}
}