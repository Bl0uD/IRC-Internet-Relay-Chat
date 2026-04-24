# pragma once

# include "Include.hpp"

class Parser
{
	private:

	public:
		std::string fullCmd;
		std::string prefix;
		std::string command;
		std::vector<std::string> params;
		std::string trailing;

		bool hasTrailing;

		void	parseMessage(std::string message);

		Parser(/* args */);
		~Parser();

};