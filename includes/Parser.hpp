/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/01 03:03:31 by jdupuis           #+#    #+#             */
/*   Updated: 2026/05/01 03:03:32 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include "Include.hpp"

class Parser
{
	/*
	Simple parsing struct used by the server to split an incoming raw
	message into its IRC components.
	Fields are public for direct access by command handlers.
	*/
	public:
		/* Parsed message components */
		std::string					fullCmd;
		std::string					prefix;
		std::string					command;
		std::vector<std::string>	params;
		std::string					trailing;

		/* Flags */
		bool    hasTrailing;

		/* Behavior */
		void    parseMessage(std::string message);

		/* Construction */
		Parser();
		~Parser();
};