/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParsingData.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:40 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/14 17:06:42 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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


