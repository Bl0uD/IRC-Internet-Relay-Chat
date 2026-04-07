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
	Client Cli = this->_Clients[i];
	Cli.setUsername( Tokens[1] );
	std::cout << "func user" << std::endl;
}

void	Server::SetNickname(  std::vector<std::string> Tokens, int ClientFd )
{

	if ( Tokens.size() != 2 )
	{
		std::cout << ERR_CMD_ARGS( "NICK", "<your nickname>");
		return ;
	}
	int i = FindClient( ClientFd );
	Client Cli = this->_Clients[i];
	Cli.setNickname( Tokens[1] );
	std::cout << "func nick" << std::endl;
}

void	Server::ChangeTopic(  std::vector<std::string> Tokens, int ClientFd )
{
	(void)Tokens;
	(void)ClientFd;
	std::cout << "func topic" << std::endl;
}

void	Server::KickClient(  std::vector<std::string> Tokens, int ClientFd )
{
	(void)Tokens;
	(void)ClientFd;
	std::cout << "func kick" << std::endl;
}

void	Server::InviteClient(  std::vector<std::string> Tokens, int ClientFd )
{
	(void)Tokens;
	(void)ClientFd;
	std::cout << "func invite" << std::endl;
}

void	Server::JoinChannel(  std::vector<std::string> Tokens, int ClientFd )
{
	(void)Tokens;
	(void)ClientFd;
	std::cout << "func Join" << std::endl;
}

void	Server::ChangeMode(  std::vector<std::string> Tokens, int ClientFd )
{
	(void)Tokens;
	(void)ClientFd;
	std::cout << "func mode" << std::endl;
}

void	Server::SendPrivMsg(  std::vector<std::string> Tokens, int ClientFd )
{
	(void)Tokens;
	(void)ClientFd;
	std::cout << "func privmsg" << std::endl;
}

void	Server::ExecCommand( std::vector<std::string> Tokens, int ClientFd )
{
	std::string	cmds[8] = {"USER", "NICK", "TOPIC", "KICK", "INVITE", "JOIN", "MODE", "PRIVMSG"};
	int			i = 0;
	while ( i < 8 && Tokens[0] != cmds[i] )
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
		default:
			std::cout << ERR_CMD_NOT_FOUND( Tokens[0] );
			break ;
	}
}