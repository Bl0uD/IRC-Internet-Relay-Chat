/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/01 03:03:31 by jdupuis           #+#    #+#             */
/*   Updated: 2026/05/03 16:04:02 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include "Include.hpp"

class Parser
{
	public:
		std::string					fullCmd;
		std::string					prefix;
		std::string					command;
		std::vector<std::string>	params;
		std::string					trailing;

		Parser();
		~Parser();

		bool    hasTrailing;
		void    parseMessage(std::string message);

};