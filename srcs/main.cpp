/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: norabino <norabino@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 14:01:55 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/02 17:57:443 by norabino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../includes/Server.hpp"

// bool	setPortPassword( Server Irc, char **av )
// {
	
// 	return ( true );
// }

int main ( int ac, char **av )
{
	if ( ac != 3 )
	{
		std::cerr << ERR_WRONG_USAGE;
		return ( 1 );
	}
	try
	{
		//Server	Irc( av );
		std::cout << DISPLAY_SERVER;
	}
	catch( std::exception &e )
	{
		//std::cerr << ??
	}
	(void)av;
	return ( 0 );
}