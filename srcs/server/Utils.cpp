/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/01 03:03:53 by jdupuis           #+#    #+#             */
/*   Updated: 2026/05/01 03:03:54 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../../includes/Server.hpp"

std::vector<std::string>	split( std::string toSplit, char delim )
{
	std::vector<std::string>	splited;
	std::string					res = "";
	size_t						pos = 0;

	if ( toSplit.empty() )
		splited.push_back( "" );
	else
	{
		while ( getStringUntil(toSplit, res, delim, pos) )
			splited.push_back( res );
	}

	return ( splited );
}

bool	getStringUntil( const std::string& input, std::string& result, char delimiter, size_t& startPos )
{
	// Vérifier si startPos dépasse la longueur de la chaîne
	if ( startPos >= input.length() )
		return false;
		
	// Trouver la position de départ effective en sautant les delim
	size_t	begin = startPos;
	while ( begin < input.length() && input[begin] == delimiter )
		++begin;

	// Si tout est des delim ou que la chaîne est vide
	if ( begin >= input.length() )
		return false;

	// Trouver la position du délimiteur ou la fin de la chaîne
	size_t	end = begin;
	while ( end < input.length() && input[end] != delimiter )
		++end;

	// Exclure les espaces en fin de mot
	size_t	wordEnd = end;
	while ( wordEnd > begin && input[wordEnd - 1] == delimiter )
		--wordEnd;

	// Extraire le résultat
	result = input.substr( begin, wordEnd - begin );

	// Mettre à jour startPos
	startPos = ( end < input.length() ) ? end + 1 : end;

	return !result.empty();
}

bool	IsOperator( Client *client, Channel *channel )
{
	return ( channel->getOperators().find( client->getFd() ) != channel->getOperators().end() );
}

int		ft_atoi( std::string str )
{
	std::stringstream	ss( str );
	int					res = -1;
	ss >> res;
	return ( res );
}