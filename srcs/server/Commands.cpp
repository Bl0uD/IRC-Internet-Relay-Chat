# include "../../includes/Server.hpp"

void	Server::PruneEmptyChannels( void )
{
	for ( std::vector< Channel >::iterator it = this->_Channels.begin(); it != this->_Channels.end(); )
	{
		if ( it->getClients().empty() )
			it = this->_Channels.erase( it );
		else
			++it;
	}
}

void	Server::SetUsername( Client *client, Parser parser )
{
	if ( Tokens.size() != 2 )
	{
		SendToClient( client, ERR_CMD_ARGS( "USER", "<your username>" ) );
		return ;
	}
	std::string newUsername = Tokens[1];
	std::string oldUsername = client->getUsername();

	if ( newUsername != oldUsername
		&& this->_Usernames.find( newUsername ) != this->_Usernames.end() )
	{
		SendToClient( client, ERR_USERNAME_USED( newUsername ) );
		return ;
	}

	if ( oldUsername != "" )
		this->_Usernames.erase( oldUsername );
	client->setNickname( newUsername );
	this->_Usernames.insert( newUsername );
	

	client->setUsername( Tokens[1] );
	SendToClient( client , UPDATE_USERNAME( client->getUsername() ) );
	if ( !client->getPassword().empty() )
		client->setRegistered( true );
}

void	Server::SetNickname( Client *client, Parser parser )
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

void	Server::SetPassword( Client *client, Parser parser )
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


void	Server::ChangeTopic( Client *client, Parser parser )
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

void	Server::KickClient( Client *client, Parser parser )
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
		PruneEmptyChannels();
		SendToClient( TargetClient, RECEIVED_KICK( channel->getTopic() ) );
		SendToAllMembers( channel, CLIENT_KICKED( channel->getTopic(), TargetClient->getNickname() ) );
	}
	else
		SendToClient( client, ERR_CLI_NOT_IN_CHAN( TargetClient->getNickname(), channel->getTopic() ) );
}

void	Server::InviteClient( Client *client, Parser parser )
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

void	Server::JoinChannel( Client *client, Parser parser )
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
	bool	hasValidPassword = false;
	if ( Tokens.size() > 2 )
		hasValidPassword = channel->getPassword() == Tokens[2];

	if ( channel->getPasswordRestriction() && Tokens.size() == 3 && !hasValidPassword  ) 
		SendToClient( client, WRONG_PASSWORD( channel->getTopic() ));

	if ( channel->getInviteOnly() && !isInvited && !hasValidPassword )
	{
		if ( channel->getPasswordRestriction() )
			SendToClient( client, GREEN + "Channel needs a password or an invite.\nTry " + YELLOW + "JOIN" + GREEN + " <" + YELLOW + channel->getTopic() + GREEN + "> <" + YELLOW + "password" + GREEN + ">" + WHITE + CRLFNL );
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
		SendToClient( client, CHANNEL_FULL( channel->getTopic() ) );
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

void	Server::ChangeMode( Client *client, Parser parser )
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
			{
				SendToClient( client, YELLOW + TargetClient->getNickname() + " has been promoted to Operator grade." + WHITE + CRLFNL );
				SendToClient( TargetClient, GREEN + "You have been promoted to Operator grade of " + YELLOW + channel->getTopic() + GREEN + "." + WHITE + CRLFNL );
			}
			else
			{
				SendToClient( client, YELLOW + TargetClient->getNickname() + " was relieved of his duties as an operator." + WHITE + CRLFNL );
				SendToClient( TargetClient, RED + "You are no longer an operator of " + YELLOW + channel->getTopic() + RED + "." + WHITE + CRLFNL );
			}
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
				SendToClient( client, CHANNEL_LIMIT( channel->getTopic(), UserLimitation ) );
			else
				SendToClient( client, CHANNEL_LIMIT_R( channel->getTopic() ) );
			break;
		}
		default:
			SendToClient( client, HELP_MODE );
			break ;
	}
}

void	Server::SendPrivMsg( Client *client, Parser parser )
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

void	Server::ChannelMessage( Client *client, Parser parser )
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
		SendToClient( client, ERR_NOT_IN_CHANNEL( Tokens[1] ) );
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


void	Server::ChannelList( Client *client, Parser parser )
{
	size_t activeCount = 0;

	for ( size_t i = 0; i < this->_Channels.size(); ++i )
	{
		if ( !this->_Channels[i].getClients().empty() )
			++activeCount;
	}

	if ( activeCount == 0 )
	{
		SendToClient( client, GREEN + "No channel exists." + WHITE );
		return ;
	}

	std::stringstream	list;
	list << BLUE << "Active channels (" << activeCount << "):" << WHITE << CRLF;

	size_t printed = 0;
	for ( size_t i = 0; i < this->_Channels.size(); i++ )
	{
		if ( this->_Channels[i].getClients().empty() )
			continue ;

		++printed;
		list << GREEN << "- " << YELLOW << this->_Channels[i].getTopic() << WHITE
			<< " (" << this->_Channels[i].getClients().size() << " users)";
		if ( printed < activeCount )
			list << CRLF;
	}

	SendToClient( client, list.str() + CRLFNL );
}

void Server::cmdCap(Client *client, Parser cmd) {
	if (cmd.params[0] == "LS")
	return (this->respond(client, RPL_CAP()));
}

void Server::cmdQuit(Client *client, Parser cmd) {
	std::cout << cmd.trailing << std::endl;

	this->sendMessToAllCommonUsers(client, RPL_QUIT(client->getPrefix(), cmd.trailing));
	this->clients.erase(searchForClient(client));
	client->setToBeDeleted(true);
}

void Server::cmdPart(Client *client, Parser cmd) {
	if (cmd.params.empty() || cmd.params.size() > 1) {
		std::cout << RED"ERR_NEEDMOREPARAMS"DEFAULT << std::endl;
		return this->respond(client, ERR_NEEDMOREPARAMS(client->getNickName(), cmd.command));
	}

	std::vector<std::string> chans = split(cmd.params[0], ',');
	std::vector<std::string>::iterator chanIt;

	for (chanIt = chans.begin(); chanIt != chans.end(); ++chanIt) {
		std::string channelName = (*chanIt);
		Channel *channel = getChannelByName(channelName);
		if (channelName.empty() || channel == NULL) {
			std::cout << RED"ERR_NOSUCHCHANNEL"DEFAULT << std::endl;
			this->respond(client, ERR_NOSUCHCHANNEL((*chanIt)));
		}
		else if (channel != NULL && !channel->isClientInChan(client)) {
			std::cout << RED"ERR_NOTONCHANNEL"DEFAULT << std::endl;
			this->respond(client, ERR_NOSUCHCHANNEL((*chanIt)));
		}
		else {
			channel->writeInChan(client, RPL_PART(channel->getName(), cmd.trailing), true);
			if (channel->removeClient(client) == false) {
				this->channels.erase(this->searchForChannel(channel));
				delete (channel);
			}

		}
	}
}

void	Help( Client *client, Parser parser )
{
	SendToClient( client, COMMAND_LIST );
}

void Server::ExecCommand( Client *client ) 
{
	parserIt it = this->_parsedMessages.begin();
	std::string commandsStr[NB_CMD] = {"CAP", "PASS", "NICK", "USER", "QUIT", "JOIN", "PART", "TOPIC", "PRIVMSG", "NOTICE", "MODE", "KICK", "INVITE"};
	cmdFunc_t	commandsFunc[NB_CMD] = {&Server::cmdCap, &Server::SetPassword, &Server::SetNickname, &Server::SetUsername, &Server::cmdQuit, &Server::JoinChannel, &Server::cmdPart, &Server::ChangeTopic &Server::SendPrivMsg, &Server::Help, &Server::ChangeMode, &Server::KickClient, &Server::InviteClient};

	while (this->_parsedMessages.size() >= 1)
	{
		int i;
		for ( i = 0; i < NB_CMD && commandsStr[i] != (*it).command; i++ ){}
		// std::cout << "i = " << i << std::endl;
		if ( (i == 2 || i == 3) && !client->getIsLog() )
		{
			std::cout << RED << "ERR_NOTREGISTERED" << WHITE << std::endl;
			this->respond(client, ERR_NOTREGISTERED((*it).command));
		}
		else if ( i > 3 && client->getIsAuth() == false )
		{
			std::cout << RED << "ERR_NOTREGISTERED" << WHITE << std::endl;
			this->respond( client, ERR_NOTREGISTERED((*it).command) );
		}
		else if ( i == NB_CMD )
		{
			std::cout << RED << "ERR_UNKNOWNCOMMAND" << WHITE << std::endl;
			this->respond( client, ERR_UNKNOWNCOMMAND( client->getNickName(), (*it).command ) );
		}
		else
		{
			// std::cout << commandsStr[i] << std::endl;
			(this->*commandsFunc[i])(client, *it);
		}
		this->_parsedMessages.erase(it);
		std::cout << std::endl;
	}
}

//void	Server::ExecCommand( std::vector<std::string> Tokens, Client *client )
//{
//	int			i = 0;
//	std::string	cmds[13] = {	"USER",				//0
//								"NICK",				//1
//								"PASS",				//2
//								"TOPIC",			//3
//								"KICK",				//4
//								"INVITE",			//5
//								"JOIN",				//6
//								"MODE",				//7
//								"PRIVMSG",			//8
//								"HELP",				//9
//								"CHANNEL",			//10
//								"HELP_MODE",		//11
//								"CHANNEL_ON"};		//12
//	if ( Tokens.empty() )
//		return ;
//	while ( i < 13 && Tokens[0] != cmds[i] )
//		i++;
//	switch ( i )
//	{
//		case 0:		//USER
//			SetUsername( Tokens, client );
//			break ;
//		case 1:		//NICK
//			if ( IsRegistered( client ) )
//				SetNickname( Tokens, client );
//			break ;
//		case 2:		//PASS
//			SetPassword( Tokens, client );
//			break ;
//		case 3:		//TOPIC
//			if ( IsRegistered( client ) )
//				ChangeTopic( Tokens, client );
//			break ;
//		case 4:		//KICK
//			if ( IsRegistered( client ) )
//				KickClient( Tokens, client );
//			break ;
//		case 5:		//INVITE
//			if ( IsRegistered( client ) )
//				InviteClient( Tokens, client );
//			break ;
//		case 6:		//JOIN
//			if ( IsRegistered( client ) )
//				JoinChannel( Tokens, client );
//			break ;
//		case 7:		//MODE
//			if ( IsRegistered( client ) )
//				ChangeMode( Tokens, client );
//			break ;
//		case 8:		//PRIVMSG
//			if ( IsRegistered( client ) )
//				SendPrivMsg( Tokens, client );
//			break ;
//		case 9:		//HELP
//			SendToClient( client, COMMAND_LIST );
//			break ;
//		case 10:	//CHANNEL
//			if ( IsRegistered( client ) )
//				ChannelMessage( Tokens, client );
//			break ;
//		case 11:	//HELP_MODE
//			SendToClient( client, HELP_MODE );
//			break ;
//		case 12:	//CHANNEL_ON
//			ChannelList( client );
//			break ;
//		default:
//			SendToClient( client, ERR_CMD_NOT_FOUND( Tokens[0] ) );
//			break ;
//	}
//}
