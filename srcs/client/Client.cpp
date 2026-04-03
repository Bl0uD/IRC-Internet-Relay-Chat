/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 14:13:09 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/03 17:52:57 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Client.hpp"

Client::~Client( void ) {}

Client::Client( void ) {}

Client::Client( std::string a )
{
	(void)a;
}

Client::Client( Client const &copy )
{
	*this = copy;
}

Client	&Client::operator=( Client const &instance )
{
	(void)instance;
	return ( *this );
}

