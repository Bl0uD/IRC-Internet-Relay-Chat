/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/01 03:02:53 by jdupuis           #+#    #+#             */
/*   Updated: 2026/05/01 03:02:54 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../../includes/Server.hpp"

Parser::Parser() : prefix(""), command(""), hasTrailing(false) {}

Parser::~Parser() {}

void	Parser::parseMessage( std::string message )
{
	size_t pos = 0;

	this->fullCmd = message;
	this->prefix.clear();
	this->command.clear();
	this->params.clear();
	this->trailing.clear();
	this->hasTrailing = false;

	if ( message[0] == ':' )
	{
		pos = message.find( ' ' );
		this->prefix = message.substr( 1, pos - 1 );
		message = message.substr( pos + 1 );
	}
	pos = message.find( ' ' );
	if ( pos == std::string::npos )
	{
		this->command = message.substr( 0, message.find('\n') );
		message = "";
	}
	else
	{
		this->command = message.substr( 0, pos );
		message = message.substr( pos + 1 );
	}
	std::transform( this->command.begin(), this->command.end(), this->command.begin(), ::toupper );

	pos = message.find( ":" );
	if ( pos != std::string::npos )
	{
		this->hasTrailing = true;
		this->trailing = message.substr( pos + 1 );
		message = message.substr( 0, pos );
		this->params = split( message, ' ' );
	}
	else
		this->params = split( message, ' ' );
}