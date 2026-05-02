/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/01 03:03:31 by jdupuis           #+#    #+#             */
/*   Updated: 2026/05/02 02:03:28 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include "Include.hpp"

class Parser
{
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