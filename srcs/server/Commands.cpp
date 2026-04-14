# include "../../includes/Server.hpp"

void	Server::SetUsername( std::vector<std::string> Tokens, Client *client )
{
	if ( Tokens.size() != 2 )
	{
		SendToClient( client, ERR_CMD_ARGS( "USER", "<your username>" ) );
		return ;
	}
	client->setUsername( Tokens[1] );
	SendToClient( client , UPDATE_USERNAME( client->getUsername() ) );
	if ( !client->getPassword().empty() )
		client->setRegistered( true );
}

void	Server::SetNickname( std::vector<std::string> Tokens, Client *client )
{
	if ( Tokens.size() != 2 )
	{
		SendToClient( client, ERR_CMD_ARGS( "NICK", "<your nickname>" ) );
		return ;
	}

	std::string newNickname = Tokens[1];
	std::string oldNickname = client->getNickname();

	if ( newNickname != oldNickname
		&& this->_Nicknames.find( newNickname ) != this->_Nicknames.end() )
	{
		SendToClient( client, ERR_NICKNAME_USED( newNickname ) );
		return ;
	}

	if ( oldNickname != "" )
		this->_Nicknames.erase( oldNickname );
	client->setNickname( newNickname );
	this->_Nicknames.insert( newNickname );
	SendToClient( client, UPDATE_NICKNAME( client->getNickname() ) );
}

void	Server::SetPassword( std::vector<std::string> Tokens, Client *client )
{
	if ( Tokens.size() != 2 )
	{
		SendToClient( client, ERR_CMD_ARGS( "PASS", "<your password>" ) );
		return ;
	}
	client->setPassword( Tokens[1] );
	SendToClient( client, UPDATE_PASSWORD );
	if ( !client->getUsername().empty() )
		client->setRegistered( true );
}


void	Server::ChangeTopic( std::vector<std::string> Tokens, Client *client )
{
	if ( Tokens.size() < 3 )
	{
		SendToClient( client, ERR_CMD_ARGS( "TOPIC", "<old topic> <new topic>" ) );
		return ;
	}

	std::string oldTopic = Tokens[1];
	Channel	*channel = FindChannelWithTopic( oldTopic );
	if ( channel == NULL )
	{
		SendToClient( client, ERR_NOT_TOPIC_FOUND( oldTopic ) );
		return ;
	}

	if ( !InChannel( client, channel ) )
	{
		SendToClient( client, ERR_NOT_IN_CHANNEL( oldTopic ) );
		return ;
	}

	if ( !IsOperator( client, channel ) && channel->getTopicRestriction() )
	{
		SendToClient( client, ERR_NOT_OPERATOR( channel->getTopic() ) );
		return ;
	}

	std::string newTopic = Tokens[2];
	if ( FindDuplicateTopic( oldTopic, newTopic ) )
	{
		SendToClient( client, ERR_TOPIC_USED( newTopic ) );
		return ;
	}

	SendToAllMembers( channel, NEW_TOPIC_SET( channel->getTopic(), newTopic, client->getNickname() ) );
	channel->setTopic( newTopic );
	return ;
}

void	Server::KickClient( std::vector<std::string> Tokens, Client *client )
{
	if ( Tokens.size() != 3 )
	{
		SendToClient( client, ERR_CMD_ARGS( "KICK", "<channel topic> <username>" ) );
		return ;
	}

	Channel  *channel = FindChannelWithTopic(Tokens[1]);
	if ( channel == NULL )
	{
		SendToClient( client, ERR_INEXISTANT_CHANNEL( Tokens[1] ) );
		return ;
	}

	Client *TargetClient = FindClientWithNickname( Tokens[2] );
	if ( TargetClient == NULL )
	{
		SendToClient( client, ERR_INEXISTANT_CLIENT( Tokens[2] ) );
		return ;
	}
	
	if ( !IsOperator( client, channel ))
	{
		SendToClient( client, ERR_NOT_OPERATOR( channel->getTopic() ) );
		return ;
	}

	if ( !InChannel( client, channel ) )
	{
		SendToClient( client, ERR_NOT_IN_CHANNEL( channel->getTopic() ) );
		return ;
	}

	if ( InChannel( TargetClient, channel ))
	{
		channel->removeClient( TargetClient->getFd() );
		channel->removeOperator( TargetClient->getFd() );
		channel->removePendingClient( TargetClient->getFd() );
		SendToClient( TargetClient, RECEIVED_KICK( channel->getTopic() ) );
		SendToAllMembers( channel, CLIENT_KICKED( channel->getTopic(), TargetClient->getNickname() ) );
	}
	else
		SendToClient( client, ERR_CLI_NOT_IN_CHAN( TargetClient->getNickname(), channel->getTopic() ) );
}

void	Server::InviteClient( std::vector<std::string> Tokens, Client *client )
{
	if ( Tokens.size() < 3 )
	{
		SendToClient( client, ERR_CMD_ARGS( "INVITE", "<channel name, client nickname>" ) );
		return ;
	}

	Channel *channel = FindChannelWithTopic( Tokens[1] );
	if ( !channel )
	{
		SendToClient( client, ERR_INEXISTANT_CHANNEL( Tokens[1] ) );
		return ;
	}

	if ( !IsOperator( client, channel ) )
	{
		SendToClient( client, ERR_NOT_OPERATOR( Tokens[1] ) );
		return ;
	}

	if ( !InChannel( client, channel ) )
	{
		SendToClient( client, ERR_NOT_IN_CHANNEL( Tokens[1] ) );
		return ;
	}

	Client *TargetClient = FindClientWithNickname( Tokens[2] );
	if ( !TargetClient )
	{
		SendToClient( client, ERR_INEXISTANT_CLIENT( Tokens[2] ) );
		return ;
	}

	if ( InChannel( TargetClient, channel ) )
	{
		SendToClient( client, BLUE + TargetClient->getNickname() + " is already in " + channel->getTopic() + "." + WHITE + CRLF );
		return ;
	}

	channel->addPendingClient( TargetClient->getFd() );
	SendToClient( TargetClient, RECEIVED_INVITE( channel->getTopic() ) );

	SendToAllMembers( channel, CLIENT_INVITED( channel->getTopic(), TargetClient->getNickname() ) );
}
bool	Server::IsChannelFull( Channel *channel )
{
	if ( channel->getUserLimitation() > 0
		&& channel->getClients().size() >= static_cast<size_t>( channel->getUserLimitation() ) )
		return true;
	return false;
}

void	Server::JoinChannel( std::vector<std::string> Tokens, Client *client )
{
	if ( Tokens.size() < 2 )
	{
		SendToClient( client, ERR_CMD_ARGS( "JOIN", "<channel name>" ) );
		return ;
	}

	std::string Topic = Tokens[1];
	Channel *channel = FindChannelWithTopic( Topic );
	if ( !channel )
	{
		Channel newChannel;
		newChannel.setId( ++this->_NextChannelId );
		newChannel.setTopic( Topic );
		this->_Channels.push_back( newChannel );
		channel = &this->_Channels.back();
		SendToClient( client, CREATE_CHANNEL( Topic ) );
	}
	if ( channel->getClients().empty() )
	{
		channel->addOperator( client->getFd() );
		channel->addClient( client->getFd() );
		return ;
	}
	if ( InChannel( client, channel ))
	{
		SendToClient( client, ALREADY_IN_CHANNEL( Topic ));
		return ;
	}

	bool	isInvited = channel->hasPendingClient( client->getFd() );
	bool	hasValidPassword = ( channel->getPasswordRestriction()
		&& Tokens.size() >= 3
		&& channel->getPassword() == Tokens[2] );

	if ( !hasValidPassword && Tokens.size() == 3 ) 
		SendToClient( client, WRONG_PASSWORD( channel->getTopic() ));

	if ( ( channel->getInviteOnly() || channel->getPasswordRestriction() )
		&& !isInvited && !hasValidPassword )
	{
		if ( channel->getPasswordRestriction() )
			SendToClient( client, GREEN + "Channel needs a password or an invite.\nTry " + YELLOW + "JOIN" + GREEN + " <" + YELLOW + channel->getTopic() + GREEN + "> <" + YELLOW + "password" + GREEN + ">" + WHITE );
		else
			SendToClient( client, GREEN + "Channel is in invite-only mode." + WHITE + CRLFNL );
		return ;
	}

	if ( !IsChannelFull( channel ))
	{
		channel->addClient( client->getFd() );
		SendToAllMembers( channel, NEW_CLIENT_JOIN( client->getNickname(), channel->getTopic() ));
		if ( isInvited )
			channel->removePendingClient( client->getFd() );
	}
	else
		SendToClient( client, GREEN + "Channel is full." + WHITE );
}

void	SetRemoveInviteOnly( Channel *channel )
{
	if ( channel->getInviteOnly() )
		channel->setInviteOnly( false );
	else
		channel->setInviteOnly( true );
}

void	ChangeTopicRestriction( Channel *channel )
{
	if ( channel->getTopicRestriction() )
		channel->setTopicRestriction( false ); 
	else
		channel->setTopicRestriction( true );
}

void	Server::SetRemovePassword( Channel *channel, std::string password )
{
	if ( password == "0" )
	{
		channel->setPassword( "" );
		channel->setPasswordRestriction( false );
	}
	else
	{
		channel->setPassword( password );
		channel->setPasswordRestriction( true );
	}
}

bool	Server::GiveTakeOperatorGrade( Channel *channel, Client *target )
{
	if ( channel->getOperators().find( target->getFd() ) != channel->getOperators().end() )
	{
		channel->removeOperator( target->getFd() );
		return false;
	}
	else
	{
		channel->addOperator( target->getFd() );
		return true;
	}
}

void	Server::SetRemoveUserLimitation( Channel *channel, std::string	Limitation )
{
	int					newUserLimitation = 0;
	std::stringstream	ss( Limitation );

	ss >> newUserLimitation;
	if ( newUserLimitation > 0 )
		channel->setUserLimitation( newUserLimitation );
	else
		channel->setUserLimitation( 0 );
}

void	Server::ChangeMode( std::vector<std::string> Tokens, Client *client )
{
	if ( Tokens.size() < 3 || Tokens.size() > 4 || Tokens[2].size() != 1 )
	{
		SendToClient( client, ERR_CMD_ARGS( "MODE", "<channel topic> <FLAG>" ) ); //flags: i t k o l
		return ;
	}
	Channel *channel = FindChannelWithTopic( Tokens[1] );
	if ( !channel )
	{
		SendToClient( client, ERR_INEXISTANT_CHANNEL( Tokens[1] ) );
		return ;
	}
	if ( !IsOperator( client, channel ) )
	{
		SendToClient( client, ERR_NOT_OPERATOR( Tokens[1] ) );
		return ;
	}
	
	char	flag = Tokens[2][0];
	switch( flag )
	{
		case 'i':
		{
			SetRemoveInviteOnly( channel );
			if ( channel->getInviteOnly() )
				SendToAllMembers( channel, channel->getTopic() + " channel Invite has been set to " + GREEN + "true." + WHITE + CRLFNL );
			else
				SendToAllMembers( channel, channel->getTopic() + " channel Invite has been set to " + RED + "false." + WHITE + CRLFNL );
			break;
		}
		case 't':
		{
			ChangeTopicRestriction( channel );
			if ( channel->getTopicRestriction() )
				SendToClient( client, channel->getTopic() + " channel Topic restriction has been set to " + GREEN + "true." + WHITE + CRLFNL );
			else
				SendToClient( client, channel->getTopic() + " channel Topic restriction has been set to " + RED + "false." + WHITE + CRLFNL );
			break;
		}
		case 'k':
		{
			if ( Tokens.size() != 4 )
			{
				SendToClient( client, ERR_CMD_ARGS( "MODE", "<channel topic> k <new password> - password to 0 to remove it" ) );
				return ;
			}
			SetRemovePassword( channel, Tokens[3] );
			SendToClient( client, SET_PSWD_RESTRICTION( channel->getTopic() ));
			break;
		}
		case 'o':
		{
			if ( Tokens.size() != 4 )
			{
				SendToClient( client, ERR_CMD_ARGS( "MODE", "<channel topic> o <nickname>" ) );
				return ;
			}
			Client *TargetClient = FindClientWithNickname( Tokens[3] );
			if ( !TargetClient )
			{
				SendToClient( client, ERR_INEXISTANT_CLIENT( Tokens[3] ) );
				return ;
			}
			if ( !InChannel( TargetClient, channel ) )
			{
				SendToClient( client, ERR_CLI_NOT_IN_CHAN( TargetClient->getNickname(), channel->getTopic() ) );
				return ;
			}
			if ( GiveTakeOperatorGrade( channel, TargetClient ) )
				SendToClient( client, YELLOW + TargetClient->getNickname() + " has been promoted to Operator grade." + WHITE + CRLFNL );
			else
				SendToClient( client, YELLOW + TargetClient->getNickname() + " was relieved of his duties as an operator." + WHITE + CRLFNL );
			break;
		}
		case 'l':
		{
			if ( Tokens.size() != 4 )
			{
				SendToClient( client, ERR_CMD_ARGS( "MODE", "<" + channel->getTopic() + "> l <user limitation> - 0 to remove limitation" ) );
				return ;
			}
			SetRemoveUserLimitation( channel, Tokens[3] );
			std::stringstream	ss;
			ss << channel->getUserLimitation();
			std::string	UserLimitation = ss.str();
			if ( channel->getUserLimitation() )
				SendToClient( client, channel->getTopic() + " channel UserLimitation has been set to " + GREEN + UserLimitation + WHITE + CRLFNL );
			else
				SendToClient( client, channel->getTopic() + " channel UserLimitation has been removed." + WHITE + CRLFNL );
			break;
		}
		default:
			SendToClient( client, HELP_MODE );
			break ;
	}
}

void	Server::SendPrivMsg( std::vector<std::string> Tokens, Client *client )
{
	if ( Tokens.size() < 3 )
	{
		SendToClient( client, ERR_CMD_ARGS( "PRIVMSG", "<nickname> <message>" ) );
		return ;
	}

	Client *TargetClient = FindClientWithNickname( Tokens[1] );
	if ( !TargetClient  )
	{
		SendToClient( client, ERR_INEXISTANT_CLIENT( Tokens[1] ) );
		return ;
	}

	std::string message;
	for ( size_t i = 2; i < Tokens.size(); ++i )
	{
		if ( i > 2 )
			message += " ";
		message += Tokens[i];
	}
	
	SendToClient( TargetClient, PRIV_MSG( client->getNickname(), message ) );
}

void	Server::ChannelMessage( std::vector< std::string > Tokens, Client *client )
{
	if ( Tokens.size() < 3 )
	{
		SendToClient( client, ERR_CMD_ARGS( "CHANNEL", "<channel topic> <message>" ) );
		return ;
	}

	Channel *channel = FindChannelWithTopic( Tokens[1] );
	if ( !channel )
	{
		SendToClient( client, ERR_INEXISTANT_CHANNEL( Tokens[1] ) );
		return ;
	}

	if ( !InChannel( client, channel ) )
	{
		ERR_NOT_IN_CHANNEL( Tokens[1] );
		return ;
	}

	std::string message;
	for ( size_t i = 2; i < Tokens.size(); ++i )
	{
		if ( i > 2 )
			message += " ";
		message += Tokens[i];
	}

	SendToChannel( client, channel, message );
}


void	Server::ChannelList( Client *client )
{
	if ( this->_Channels.size() == 0 )
	{
		SendToClient( client, GREEN + "No channel exists." + WHITE );
		return ;
	}

	std::stringstream	list;
	list << BLUE << "Active channels (" << this->_Channels.size() << "):" << WHITE << CRLF;

	for ( size_t i = 0; i < this->_Channels.size(); i++ )
	{
		list << GREEN << "- " << YELLOW << this->_Channels[i].getTopic() << WHITE
			<< " (" << this->_Channels[i].getClients().size() << " users)";
		if ( i + 1 < this->_Channels.size())
			list << CRLF;
	}

	SendToClient( client, list.str() );
}

void	Server::ExecCommand( std::vector<std::string> Tokens, Client *client )
{
	int			i = 0;
	std::string	cmds[13] = {	"USER",				//0
								"NICK",				//1
								"PASS",				//2
								"TOPIC",			//3
								"KICK",				//4
								"INVITE",			//5
								"JOIN",				//6
								"MODE",				//7
								"PRIVMSG",			//8
								"HELP",				//9
								"CHANNEL",			//10
								"HELP_MODE",		//11
								"CHANNEL_ON"};		//12
	if ( Tokens.empty() )
		return ;
	while ( i < 13 && Tokens[0] != cmds[i] )
		i++;
	switch ( i )
	{
		case 0:		//USER
			SetUsername( Tokens, client );
			break ;
		case 1:		//NICK
			if ( IsRegistered( client ) )
				SetNickname( Tokens, client );
			break ;
		case 2:		//PASS
			SetPassword( Tokens, client );
			break ;
		case 3:		//TOPIC
			if ( IsRegistered( client ) )
				ChangeTopic( Tokens, client );
			break ;
		case 4:		//KICK
			if ( IsRegistered( client ) )
				KickClient( Tokens, client );
			break ;
		case 5:		//INVITE
			if ( IsRegistered( client ) )
				InviteClient( Tokens, client );
			break ;
		case 6:		//JOIN
			if ( IsRegistered( client ) )
				JoinChannel( Tokens, client );
			break ;
		case 7:		//MODE
			if ( IsRegistered( client ) )
				ChangeMode( Tokens, client );
			break ;
		case 8:		//PRIVMSG
			if ( IsRegistered( client ) )
				SendPrivMsg( Tokens, client );
			break ;
		case 9:		//HELP
			SendToClient( client, COMMAND_LIST );
			break ;
		case 10:	//CHANNEL
			if ( IsRegistered( client ) )
				ChannelMessage( Tokens, client );
			break ;
		case 11:	//HELP_MODE
			SendToClient( client, HELP_MODE );
			break ;
		case 12:	//CHANNEL_ON
			ChannelList( client );
			break ;
		default:
			SendToClient( client, ERR_CMD_NOT_FOUND( Tokens[0] ) );
			break ;
	}
}