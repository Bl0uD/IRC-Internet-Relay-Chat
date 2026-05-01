/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:55 by jdupuis           #+#    #+#             */
/*   Updated: 2026/05/01 03:13:14 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	}
	catch( const std::exception &e )
	{
		std::cerr << e.what() << std::endl;
		return ( 1 );
	}
	return ( 0 );
}