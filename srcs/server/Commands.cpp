# include "../../includes/Server.hpp"

void	Server::SetUsername( Client *client, Parser cmd )
{
	std::string currentNick = client->getNickname().empty() ? "*" : client->getNickname();

	if ( client->getRegistered() )
	{
		this->respond( client, ERR_ALREADYREGISTRED( currentNick ) );
		return ;
	}
	if ( cmd.params.size() < 3 || !cmd.hasTrailing )
	{
		this->respond( client, ERR_NEEDMOREPARAMS( currentNick, "USER" ) );
		return ;
	}
	std::string newUsername = cmd.params[0];
	std::string oldUsername = client->getUsername();

	if ( oldUsername != "" )
		this->_Usernames.erase( oldUsername );
	this->_Usernames.insert( newUsername );
	

	client->setUsername( cmd.params[0] );
	client->setLog( true );
	if ( !client->getPassword().empty() && !client->getNickname().empty() )
	{
		client->setAuth( true );
		this->SendWelcome( client );
	}
}

void	Server::SetNickname( Client *client, Parser cmd )
{
	if (  cmd.params.empty() || cmd.params.size() > 2 )
	{
		SendToClient( client, ERR_CMD_ARGS( "NICK", "<your nickname>" ) );
		return ;
	}

	std::string newNickname = cmd.params[0];
	std::string oldNickname = client->getNickname();

	if ( newNickname != oldNickname
		&& this->_ClientNames.find( newNickname ) != this->_ClientNames.end() )
	{
		std::string currentNick = oldNickname.empty() ? "*" : oldNickname;
		this->respond( client, ERR_NICKNAMEINUSE( currentNick, newNickname ) );
		return ;
	}

	if ( oldNickname != "" )
		this->_ClientNames.erase( oldNickname );
	client->setNickname( newNickname );
	this->_ClientNames.insert( newNickname );
	if ( client->getRegistered() )
		SendToClient( client, ":" + oldNickname + " NICK :" + client->getNickname() );
	else if ( !client->getUsername().empty() && !client->getPassword().empty() )
	{
		client->setAuth( true );
		this->SendWelcome( client );
	}
}

void	Server::SetPassword( Client *client, Parser cmd )
{
	if (  cmd.params.empty() || cmd.params.size() > 4  )
	{
		SendToClient( client, ERR_CMD_ARGS( "PASS", "<password>" ) );
		return ;
	}
	client->setPassword( cmd.params[0] );
	client->setLog( true );
	if ( !client->getUsername().empty() && !client->getNickname().empty() )
	{
		client->setAuth( true );
		this->SendWelcome( client );
	}
}


void	Server::ChangeTopic( Client *client, Parser cmd )
{
	if ( cmd.params.size() < 3 )
	{
		SendToClient( client, ERR_CMD_ARGS( "TOPIC", "<old topic> <new topic>" ) );
		return ;
	}

	std::string oldTopic = cmd.params[0];
	Channel	*channel = FindChannelWithName( oldTopic );
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

	std::string newTopic = cmd.params[1];
	SendToChannel( client, channel, "TOPIC " + channel->getName() + " :" + newTopic, true );
	channel->setTopic( newTopic );
	return ;
}

void	Server::KickClient( Client *client, Parser cmd )
{
	if (  cmd.params.empty() || cmd.params.size() > 3 )
	{
		SendToClient( client, ERR_CMD_ARGS( "KICK", "<channel topic> <username>" ) );
		return ;
	}

	Channel  *channel = FindChannelWithName( cmd.params[0] );
	if ( channel == NULL )
	{
		SendToClient( client, ERR_INEXISTANT_CHANNEL( cmd.params[0] ) );
		return ;
	}

	Client *TargetClient = FindClientWithNickname( cmd.params[1] );
	if ( TargetClient == NULL )
	{
		SendToClient( client, ERR_INEXISTANT_CLIENT( cmd.params[1] ) );
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
		SendToChannel( client, channel, RPL_KICK( channel->getName(), TargetClient->getNickname(), cmd.trailing ), true );
		channel->removeClient( TargetClient );
		channel->removeOperator( TargetClient->getFd() );
		channel->removePendingClient( TargetClient->getFd() );
		PruneEmptyChannels();
	}
	else
		SendToClient( client, ERR_CLI_NOT_IN_CHAN( TargetClient->getNickname(), channel->getTopic() ) );
}

void	Server::InviteClient( Client *client, Parser cmd )
{
	if ( cmd.params.size() != 2 )
	{
		SendToClient( client, ERR_CMD_ARGS( "INVITE", "<channel name, client nickname>" ) );
		return ;
	}

	std::string	channelName;
	std::string	targetNickname;
	if ( !cmd.params[0].empty() && cmd.params[0][0] == '#' )
	{
		channelName = cmd.params[0];
		targetNickname = cmd.params[1];
	}
	else if ( !cmd.params[1].empty() && cmd.params[1][0] == '#' )
	{
		channelName = cmd.params[1];
		targetNickname = cmd.params[0];
	}
	else
	{
		SendToClient( client, ERR_CMD_ARGS( "INVITE", "<#channel> <nickname>" ) );
		return ;
	}

	Channel *channel = FindChannelWithName( channelName );
	if ( !channel )
	{
		SendToClient( client, ERR_INEXISTANT_CHANNEL( channelName ) );
		return ;
	}

	if ( !IsOperator( client, channel ) )
	{
		SendToClient( client, ERR_NOT_OPERATOR( cmd.params[0] ) );
		return ;
	}

	if ( !InChannel( client, channel ) )
	{
		SendToClient( client, ERR_NOT_IN_CHANNEL( cmd.params[0] ) );
		return ;
	}

	Client *TargetClient = FindClientWithNickname( targetNickname );
	if ( !TargetClient )
	{
		SendToClient( client, ERR_INEXISTANT_CLIENT( targetNickname ) );
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

void	Server::JoinChannel( Client *client, Parser cmd )
{
	if (  cmd.params.empty() || cmd.params.size() > 2 )
	{
		SendToClient( client, ERR_CMD_ARGS( "JOIN", "<channel name>" ) );
		return ;
	}

	std::string Name = cmd.params[0];
	Channel *channel = FindChannelWithName( Name );
	bool	isNewChannel = false;
	if ( !channel )
	{
		Channel newChannel;
		newChannel.setId( ++this->_NextChannelId );

		newChannel.setName( Name );
		this->_Channels.push_back( newChannel );
		channel = &this->_Channels.back();
		isNewChannel = true;
	}
	if ( InChannel( client, channel ))
	{
		SendToClient( client, ALREADY_IN_CHANNEL( Name ));
		return ;
	}

	bool	isInvited = channel->hasPendingClient( client->getFd() );
	bool	hasValidPassword = false;
	std::string	providedPassword;
	if ( cmd.params.size() > 1 )
	{
		providedPassword = cmd.params[1];
		if ( !providedPassword.empty() && providedPassword[0] == ':' )
			providedPassword = providedPassword.substr( 1 );
		hasValidPassword = channel->getPassword() == providedPassword;
	}

	if ( !isNewChannel && ( channel->getInviteOnly() || channel->getPasswordRestriction() )
		&& !isInvited && !hasValidPassword )
	{
		if ( channel->getPasswordRestriction() && channel->getInviteOnly() )
			SendToClient( client, GREEN + "Channel needs a password or an invite.\nTry " + YELLOW + "JOIN" + GREEN + " <" + YELLOW + channel->getTopic() + GREEN + "> <" + YELLOW + "password" + GREEN + ">" + WHITE + CRLFNL );
		else if ( channel->getPasswordRestriction() )
			this->respond( client, ERR_BADCHANNELKEY( client->getNickname(), channel->getName() ) );
		else
			this->respond( client, ERR_INVITEONLYCHAN( channel->getName() ) );
		return ;
	}

	if ( !IsChannelFull( channel ) )
	{
		channel->addClient( client->getFd() );
		if ( channel->getClients().size() == 1 )
			channel->setOperator( this, client, client, '+' );

		SendToChannel( client, channel, RPL_JOIN( channel->getName() ), true );
		if ( channel->getTopic().empty() )
			this->respond( client, RPL_NOTOPIC( channel->getName() ) );
		else
			this->respond( client, RPL_TOPIC( client->getNickname(), channel->getName(), channel->getTopic() ) );

		std::string names;
		for ( std::set<int>::const_iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it )
		{
			Client *member = FindClientWithFd( *it );
			if ( !member )
				continue;
			if ( !names.empty() )
				names += " ";
			if ( channel->getOperators().find( *it ) != channel->getOperators().end() )
				names += "@";
			names += member->getNickname();
		}
		this->respond( client, RPL_NAMREPLY( client->getNickname(), channel->getName() ) + names );
		this->respond( client, RPL_ENDOFNAMES( client->getNickname(), channel->getName() ) );

		if ( isInvited )
			channel->removePendingClient( client->getFd() );
	}
	else
		SendToClient( client, CHANNEL_FULL( channel->getTopic() ) );
}


void	Server::ChangeMode( Client *client, Parser cmd )
{
	if (cmd.params.size() < 2 || cmd.params[0].empty())
	{
		if ( cmd.params.size() == 1 && cmd.params[0][0] == '#' )
		{
			Channel	*channeltest = this->FindChannelWithName( cmd.params[0] );
			if ( channeltest == NULL )
			{
				std::cout << RED << "ERR_NOSUCHCHANNEL" << WHITE << std::endl;
				this->respond(client, ERR_NOSUCHCHANNEL(cmd.params[0]));
				return;
			}
			this->respond(client, RPL_CHANNELMODEIS( client->getNickname(), channeltest->getName(), channeltest->getModsForReply()) );
			return;
		}
		else
		{
			std::cout << RED << "ERR_NEEDMOREPARAMS" << WHITE << std::endl;
			this->respond( client, ERR_NEEDMOREPARAMS(client->getNickname(), cmd.command) );
		}
		return;
	}
	Channel	*channel = FindChannelWithName( cmd.params[0] );
	if ( channel == NULL )
	{
		std::cout << RED << "ERR_NOSUCHCHANNEL" << WHITE << std::endl;
		this->respond(client, ERR_NOSUCHCHANNEL(cmd.params[0]));
		return;
	}
	if ( !channel->isClientInChannel(client) )
	{
		std::cout << RED << "ERR_NOTONCHANNEL" << WHITE << std::endl;
		this->respond( client, ERR_NOTONCHANNEL(client->getNickname(), cmd.params[0]) );
		return;
	}
	if ( !channel->isClientAdmin(client) )
	{
		std::cout << RED << "ERR_CHANOPRIVSNEEDED" << WHITE << std::endl;
		this->respond( client, ERR_CHANOPRIVSNEEDED(client->getNickname(), cmd.params[0]) );
		return;
	}
	if ( cmd.params[1].empty() )
	{
		this->respond( client, RPL_CHANNELMODEIS(client->getNickname(), channel->getName(), channel->getModsForReply()) );
		return;
	}
	char	sign = '+';
	std::vector<std::string>	modeArgs;
	for ( size_t i = 2; i < cmd.params.size(); ++i )
	{
		if ( cmd.params[i].find( ',' ) != std::string::npos )
		{
			std::vector<std::string>	splitted = split( cmd.params[i], ',' );
			for ( size_t j = 0; j < splitted.size(); ++j )
			{
				if ( !splitted[j].empty() )
					modeArgs.push_back( splitted[j] );
			}
		}
		else if ( !cmd.params[i].empty() )
			modeArgs.push_back( cmd.params[i] );
	}
	if ( cmd.hasTrailing && !cmd.trailing.empty() )
	{
		std::vector<std::string>	splittedTrailing = split( cmd.trailing, ',' );
		for ( size_t i = 0; i < splittedTrailing.size(); ++i )
		{
			if ( !splittedTrailing[i].empty() )
				modeArgs.push_back( splittedTrailing[i] );
		}
	}
	size_t				arg_index = 0;
	for ( size_t i = 0; i < cmd.params[1].length(); ++i )
	{
		if ( cmd.params[1][i] == '+' || cmd.params[1][i] == '-' )
		{
			sign = cmd.params[1][i];
		}
		else if ( cmd.params[1][i] == ',' )
		{
			continue;
		}
		else
		{
			if ( cmd.params[1][i] != 'i' && cmd.params[1][i] != 't' && cmd.params[1][i] != 'k' && cmd.params[1][i] != 'o' && cmd.params[1][i] != 'l' )
			{
				this->respond( client, ERR_UNKNOWNMODE( client->getNickname(), std::string( 1, cmd.params[1][i] ) ) );
				return;
			}
			if ( cmd.params[1][i] == 'k' )
			{
				if ( sign == '+' && channel->getMods().count('k') == 1 )
				{
					std::cout << RED << "ERR_KEYSET" << WHITE << std::endl;
					this->respond( client, ERR_KEYSET(client->getNickname(), channel->getName()) );
					return ;
				}
				if ( sign == '-' )
				{
					channel->setKey(this, client, sign, "");
				}
				else if ( arg_index < modeArgs.size() )
				{
					channel->setKey(this, client, sign, modeArgs[arg_index]);
					arg_index ++;
				}
				else
				{
					std::cout << RED << "ERR_NEEDMOREPARAMS" << WHITE << std::endl;
					this->respond( client, ERR_NEEDMOREPARAMS( client->getNickname(), cmd.command) );
					return ;
				}
				// Réinitialiser sign à '+' après chaque flag sauf s'il y a un signe explicite après
				if ( i + 1 < cmd.params[1].length() && cmd.params[1][i + 1] != '+' && cmd.params[1][i + 1] != '-' )
					sign = '+';
			}
			if ( cmd.params[1][i] == 'l' )
			{
				if ( sign == '-' )
					channel->setUserLimitation( this, client, sign, "0" );
				else if ( arg_index < modeArgs.size() )
				{
					channel->setUserLimitation( this, client, sign, modeArgs[arg_index] );
					arg_index ++;
				}
				else
				{
					std::cout << RED << "ERR_NEEDMOREPARAMS" << WHITE << std::endl;
					this->respond( client, ERR_NEEDMOREPARAMS(client->getNickname(), cmd.command) );
					return;
				}
				// Réinitialiser sign à '+' après chaque flag sauf s'il y a un signe explicite après
				if ( i + 1 < cmd.params[1].length() && cmd.params[1][i + 1] != '+' && cmd.params[1][i + 1] != '-' )
					sign = '+';
			}
			if ( cmd.params[1][i] == 'i' )
			{
				channel->setMods(this, client, sign, 'i' );
				// Réinitialiser sign à '+' après chaque flag sauf s'il y a un signe explicite après
				if ( i + 1 < cmd.params[1].length() && cmd.params[1][i + 1] != '+' && cmd.params[1][i + 1] != '-' )
					sign = '+';
			}
			if ( cmd.params[1][i] == 't' )
			{
				channel->setMods(this, client, sign, 't' );
				// Réinitialiser sign à '+' après chaque flag sauf s'il y a un signe explicite après
				if ( i + 1 < cmd.params[1].length() && cmd.params[1][i + 1] != '+' && cmd.params[1][i + 1] != '-' )
					sign = '+';
			}
			if ( cmd.params[1][i] == 'o' )
			{
				if ( arg_index < modeArgs.size() )
				{
					Client *Target = FindClientWithNickname( modeArgs[arg_index] );
					if ( Target != NULL && channel->isClientInChannel( Target ) == true )
						channel->setOperator( this, client, Target, sign );
					else
					{
						std::cout << RED << "ERR_USERNOTINCHANNEL" << WHITE << std::endl;
						this->respond( client, ERR_USERNOTINCHANNEL( client->getNickname(), modeArgs[arg_index], channel->getName()) );
						return;
					}
					arg_index ++;
				}
				else
				{
					std::cout << RED << "ERR_NEEDMOREPARAMS" << WHITE << std::endl;
					this->respond( client, ERR_NEEDMOREPARAMS( client->getNickname(), cmd.command) );
					return;
				}
				// Réinitialiser sign à '+' après chaque flag sauf s'il y a un signe explicite après
				if ( i + 1 < cmd.params[1].length() && cmd.params[1][i + 1] != '+' && cmd.params[1][i + 1] != '-' )
					sign = '+';
			}
		}
	}
}

void	Server::SendPrivMsg( Client *client, Parser cmd )
{
	if ( cmd.params.empty() || cmd.params[0].empty() )
	{
		std::cout << RED << "ERR_NORECIPIENT" << WHITE << std::endl;
		this->respond( client, ERR_NORECIPIENT(client->getNickname(), cmd.command) );
		return ;
	}

	if ( cmd.params.size() != 1 )
	{
		std::cout << RED << "ERR_NEEDMOREPARAMS" << WHITE << std::endl;
		this->respond( client, ERR_NEEDMOREPARAMS(client->getNickname(), cmd.command) );
		return ;
	}

	if ( !cmd.hasTrailing )
	{
		std::cout << RED << "ERR_NOTEXTTOSEND" << WHITE << std::endl;
		this->respond( client, ERR_NOTEXTTOSEND(client->getNickname(), cmd.command) );
		return ;
	}

	if ( cmd.params[0].find(',') != std::string::npos )
	{
		std::cout << RED << "ERR_TOOMANYTARGETS" << WHITE << std::endl;
		this->respond( client, ERR_TOOMANYTARGETS(client->getNickname(), cmd.command) );
		return ;
	}

	if ( cmd.params[0][0] == '#' )
	{
		Channel *channel = this->FindChannelWithName( cmd.params[0] );
		if ( channel == NULL )
		{
			std::cout << RED << "ERR_NOSUCHCHANNEL" << WHITE << std::endl;
			this->respond( client, ERR_NOSUCHCHANNEL( cmd.params[0] ) );
		}
		else if ( !channel->isClientInChannel( client ) )
		{
			std::cout << RED << "ERR_NOTONCHANNEL" << WHITE << std::endl;
			this->respond( client, ERR_NOTONCHANNEL( client->getNickname(), cmd.params[0] ) );
		}
		else
			SendToChannel( client, channel, RPL_PRIVMSG(cmd.params[0], cmd.trailing), false );
		return ;
	}

	Client *clienttest = this->FindClientWithNickname( cmd.params[0] );
	if ( clienttest == NULL || !clienttest->getIsAuth() )
	{
		std::cout << RED << "ERR_NOSUCHNICK" << WHITE << std::endl;
		this->respond( client, ERR_NOSUCHNICK(client->getNickname(), cmd.command) );
		return;
	}
	else
		SendToClient( clienttest, ":" + client->getPrefix() + " " + RPL_PRIVMSG(cmd.params[0], cmd.trailing) );
}

void	Server::ChannelList( Client *client, Parser parser )
{
	(void)parser;
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

void	Server::cmdQuit( Client *client, Parser cmd )
{
	(void)cmd;
	RemoveClient( client );
}

void	Server::cmdPart( Client *client, Parser cmd )
{
	if ( cmd.params.empty() || cmd.params.size() > 1 ) 
	{
		std::cout << RED << "ERR_NEEDMOREPARAMS" << WHITE << std::endl;
		return this->respond( client, ERR_NEEDMOREPARAMS(client->getNickname(), cmd.command) );
	}

	std::vector<std::string>			chans = split( cmd.params[0], ',' );
	std::vector<std::string>::iterator	chanIt;

	for ( chanIt = chans.begin(); chanIt != chans.end(); ++chanIt ) 
	{
		std::string	channelName = ( *chanIt );
		Channel		*channel = FindChannelWithName( channelName );
		if ( channelName.empty() || channel == NULL )
		{
			std::cout << RED << "ERR_NOSUCHCHANNEL" << WHITE << std::endl;
			this->respond( client, ERR_NOSUCHCHANNEL((*chanIt)) );
		}
		else if ( channel != NULL && !InChannel( client, channel ) )
		{
			std::cout << RED << "ERR_NOTONCHANNEL" << WHITE << std::endl;
			this->respond(client, ERR_NOTONCHANNEL( client->getNickname(), (*chanIt) ));
		}
		else
		{
			SendToChannel( client, channel, RPL_PART(channel->getName(), cmd.trailing), true );
			channel->removeClient( client );
			channel->removeOperator( client->getFd() );
			channel->removePendingClient( client->getFd() );
			PruneEmptyChannels();
		}
	}
}

void	Server::cmdHelp( Client *client, Parser cmd )
{
	(void)cmd;

	SendToClient( client, COMMAND_LIST );
}

void	Server::cmdPing( Client *client, Parser cmd )
{
	std::string token;

	if ( !cmd.params.empty() )
		token = cmd.params[0];
	else if ( cmd.hasTrailing )
		token = cmd.trailing;
	if ( token.empty() )
	{
		SendToClient( client, ERR_NEEDMOREPARAMS( client->getNickname(), "PING" ) );
		return ;
	}
	this->respond( client, "PONG :" + token );
}

void	Server::cmdCap( Client *client, Parser cmd )
{
	if ( !cmd.params.empty() && cmd.params[0] == "LS" )
		this->respond( client, RPL_CAP() );
}


void	Server::ExecCommand( Client *client ) 
{
	std::string commandsStr[NB_CMD] = {"CAP", "PASS", "NICK", "USER", "QUIT", "JOIN", "PART", "TOPIC", "PRIVMSG", "NOTICE", "MODE", "KICK", "INVITE", "PING"};
	cmdFunc_t	commandsFunc[NB_CMD] = {&Server::cmdCap, &Server::SetPassword, &Server::SetNickname, &Server::SetUsername, &Server::cmdQuit, &Server::JoinChannel, &Server::cmdPart, &Server::ChangeTopic, &Server::SendPrivMsg, &Server::cmdHelp, &Server::ChangeMode, &Server::KickClient, &Server::InviteClient, &Server::cmdPing};

	while (this->_parsedMessages.size() >= 1)
	{
		parserIt it = this->_parsedMessages.begin();
		int i;
		std::cout << "Commamd tried by" << YELLOW << client->getNickname() << WHITE << "\t:\t" << (*it).fullCmd << std::endl;
		for ( i = 0; i < NB_CMD && commandsStr[i] != (*it).command; i++ ){}
		if ( i > 3 && client->getIsAuth() == false )
		{
			std::cout << RED << "ERR_NOTREGISTERED" << WHITE << std::endl;
			this->respond( client, ERR_NOTREGISTERED((*it).command) );
		}
		else if ( i == NB_CMD )
		{
			std::cout << RED << "ERR_UNKNOWNCOMMAND" << WHITE << std::endl;
			this->respond( client, ERR_UNKNOWNCOMMAND( client->getNickname(), (*it).command ) );
		}
		else
			(this->*commandsFunc[i])(client, *it);
		this->_parsedMessages.erase(it);
		std::cout << GREEN << "\t- Command has succeed" << WHITE << "." << std::endl;
	}
}