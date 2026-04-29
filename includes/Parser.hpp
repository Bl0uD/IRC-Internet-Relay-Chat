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