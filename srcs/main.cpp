# include "../includes/Server.hpp"

int main ( int ac, char **av )
{
	if ( ac != 3 )
	{
		std::cerr << ERR_WRONG_USAGE;
		return ( 1 );
	}
	try
	{
		Server	Irc( av );
		signal( SIGINT, Irc.SignalHandler );
		signal( SIGQUIT, Irc.SignalHandler );
		signal( SIGPIPE, SIG_IGN );
		Irc.Init();
		while ( Irc.getStatus() )
			Irc.Running();
		//CloseFds(); // close the server file descriptors when the server stops*/

	}
	catch( const std::exception &e )
	{
		std::cerr << e.what() << std::endl;
		return ( 1 );
	}
	return ( 0 );
}