# include "../../includes/Server.hpp"

void	Server::SetUsername( Client *client, Parser cmd )
{
	if ( cmd.params.empty() || cmd.params.size() > 4 )
	{
		SendToClient( client, ERR_CMD_ARGS( "USER", "<username> <hostname> <servername> :<realname>" ) );
		return ;
	}
	std::string newUsername = cmd.params[0];
	std::string oldUsername = client->getUsername();

	if ( newUsername != oldUsername
		&& this->_Usernames.find( newUsername ) != this->_Usernames.end() )
	{
		SendToClient( client, ERR_USERNAME_USED( newUsername ) );
		return ;
	}

	if ( oldUsername != "" )
		this->_Usernames.erase( oldUsername );
	this->_Usernames.insert( newUsername );
	

	client->setUsername( cmd.params[0] );
	client->setLog( true );
	if ( !client->getPassword().empty() )
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
		SendToClient( client, ERR_NICKNAME_USED( newNickname ) );
		return ;
	}

	if ( oldNickname != "" )
		this->_ClientNames.erase( oldNickname );
	client->setNickname( newNickname );
	this->_ClientNames.insert( newNickname );
	if ( client->getRegistered() )
		SendToClient( client, ":" + oldNickname + " NICK :" + client->getNickname() );
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
	if ( !client->getUsername().empty() )
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
	SendToAllMembers( channel, NEW_TOPIC_SET( channel->getTopic(), newTopic, client->getNickname() ) );
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
		channel->removeClient( TargetClient );
		channel->setOperator( this, client, TargetClient, '-' );
		channel->removePendingClient( TargetClient->getFd() );
		PruneEmptyChannels();
		SendToClient( TargetClient, RECEIVED_KICK( channel->getTopic() ) );
		SendToAllMembers( channel, CLIENT_KICKED( channel->getTopic(), TargetClient->getNickname() ) );
	}
	else
		SendToClient( client, ERR_CLI_NOT_IN_CHAN( TargetClient->getNickname(), channel->getTopic() ) );
}

void	Server::InviteClient( Client *client, Parser cmd )
{
	if (  cmd.params.empty() || cmd.params.size() > 3 )
	{
		SendToClient( client, ERR_CMD_ARGS( "INVITE", "<channel name, client nickname>" ) );
		return ;
	}

	Channel *channel = FindChannelWithName( cmd.params[0] );
	if ( !channel )
	{
		SendToClient( client, ERR_INEXISTANT_CHANNEL( cmd.params[0] ) );
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

	Client *TargetClient = FindClientWithNickname( cmd.params[1] );
	if ( !TargetClient )
	{
		SendToClient( client, ERR_INEXISTANT_CLIENT( cmd.params[1] ) );
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
	if ( !channel )
	{
		Channel newChannel;
		newChannel.setId( ++this->_NextChannelId );

		newChannel.setName( Name );
		this->_Channels.push_back( newChannel );
		this->_ClientNames.insert( Name );
		channel = &this->_Channels.back();
		SendToClient( client, CREATE_CHANNEL( Name ) );
	}
	if ( channel->getClients().empty() )
	{
		channel->setOperator( this, client, client, '+' );
		channel->addClient( client->getFd() );
		return ;
	}
	if ( InChannel( client, channel ))
	{
		SendToClient( client, ALREADY_IN_CHANNEL( Name ));
		return ;
	}

	bool	isInvited = channel->hasPendingClient( client->getFd() );
	bool	hasValidPassword = false;
	if ( cmd.params.size() > 1 )
		hasValidPassword = channel->getPassword() == cmd.params[1];

	if ( channel->getPasswordRestriction() && cmd.params.size() == 2 && !hasValidPassword  ) 
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
	std::string allowed = "itkol-+";
	char	sign;
	if ( cmd.params[1][0] != '+' || cmd.params[1][0] != '-' )
		sign = '+';
	std::vector<char>	mods;
	size_t				param_index = 2;
	for ( size_t i = 0; i < cmd.params[1].length(); ++i )
	{
		if ( cmd.params[1][i] == '+' || cmd.params[1][i] == '-' )
			sign = cmd.params[1][i];
		else
		{
			if ( cmd.params[1][i] == 'k' )
			{
				if ( channel->getMods().count('k') == 1 && sign == '+' )
				{
					std::cout << RED << "ERR_KEYSET" << WHITE << std::endl;
					this->respond( client, ERR_KEYSET(client->getNickname(), channel->getName()) );
					return ;
				}
				if (cmd.params.size() >= param_index + 1 && !cmd.params[param_index].empty())
				{
					channel->setKey(this, client, sign, cmd.params[param_index]);
					param_index ++;
				}
				else
				{
					std::cout << RED << "ERR_NEEDMOREPARAMS" << WHITE << std::endl;
					this->respond( client, ERR_NEEDMOREPARAMS( client->getNickname(), cmd.command) );
					return ;
				}
			}
			if ( cmd.params[1][i] == 'l' )
			{
				if ( cmd.params.size() >= param_index + 1 && !cmd.params[param_index].empty() )
				{
					channel->setUserLimitation( this, client, sign, cmd.params[param_index] );
					if ( sign == '+' )
						param_index ++;
				}
				else
				{
					std::cout << RED << "ERR_NEEDMOREPARAMS" << WHITE << std::endl;
					this->respond( client, ERR_NEEDMOREPARAMS(client->getNickname(), cmd.command) );
					return;
				}
			}
			if ( cmd.params[1][i] == 'i' )
				channel->setMods(this, client, sign, 'i' );
			if ( cmd.params[1][i] == 't' )
				channel->setMods(this, client, sign, 't' );
			if ( cmd.params[1][i] == 'o' )
			{
				if ( cmd.params.size() >= param_index + 1 && !cmd.params[param_index].empty() )
				{
					Client *Target = FindClientWithNickname( cmd.params[param_index] );
					if ( Target != NULL && channel->isClientInChannel( Target ) == true )
						channel->setOperator( this, client, Target, sign );
					else
					{
						std::cout << RED << "ERR_USERNOTINCHANNEL" << WHITE << std::endl;
						this->respond( client, ERR_USERNOTINCHANNEL( client->getNickname(), Target->getNickname(), channel->getName()) );
						return;
					}
					param_index ++;
				}
				else
				{
					std::cout << RED << "ERR_NEEDMOREPARAMS" << WHITE << std::endl;
					this->respond( client, ERR_NEEDMOREPARAMS( client->getNickname(), cmd.command) );
					return;
				}
			}
		}
	}
}

void	Server::SendPrivMsg( Client *client, Parser cmd )
{
	if ( cmd.params[0].empty() )
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
			std::cout << RED << "Channel " << cmd.params[0] << " does not exists." << WHITE << std::endl;
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
			this->respond(client, ERR_NOSUCHCHANNEL((*chanIt)));
		}
		else
			SendToChannel( client, channel, RPL_PART(channel->getName(), cmd.trailing), true );
	}
}

void	Server::Help( Client *client, Parser parser )
{
	(void)parser;
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
	std::string commandsStr[NB_CMD] = {"CAP", "PASS", "NICK", "USER", "QUIT", "JOIN", "PART", "TOPIC", "PRIVMSG", "NOTICE", "MODE", "KICK", "INVITE"};
	cmdFunc_t	commandsFunc[NB_CMD] = {&Server::cmdCap, &Server::SetPassword, &Server::SetNickname, &Server::SetUsername, &Server::cmdQuit, &Server::JoinChannel, &Server::cmdPart, &Server::ChangeTopic, &Server::SendPrivMsg, &Server::Help, &Server::ChangeMode, &Server::KickClient, &Server::InviteClient};

	while (this->_parsedMessages.size() >= 1)
	{
		parserIt it = this->_parsedMessages.begin();
		int i;
		for ( i = 0; i < NB_CMD && commandsStr[i] != (*it).command; i++ ){}
		if ( (*it).command == "PING" )
		{
			this->cmdPing( client, *it );
			this->_parsedMessages.erase(it);
			std::cout << std::endl;
			continue ;
		}
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
			this->respond( client, ERR_UNKNOWNCOMMAND( client->getNickname(), (*it).command ) );
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