# include "../../includes/Server.hpp"

Parser::Parser(/* args */) : prefix(""), command(""), hasTrailing(false) {}

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

	// std::cout << "one message: '" << message << "'" << std::endl;
	if (message[0] == ':') {
		pos = message.find(' ');
		this->prefix = message.substr(1, pos - 1);
		message = message.substr(pos + 1);
	}
	pos = message.find(' ');
	if (pos == std::string::npos) {
		this->command = message.substr(0, message.find('\n'));
		message = "";
	}
	else {
		this->command = message.substr(0, pos);
		message = message.substr(pos + 1);
	}
	std::transform( this->command.begin(), this->command.end(), this->command.begin(), ::toupper );

	pos = message.find(":");
	if (pos != std::string::npos) {
		this->hasTrailing = true;
		this->trailing = message.substr(pos + 1);
		message = message.substr(0, pos);
		this->params = split(message, ' ');
	}
	else
		this->params = split(message, ' ');

	// std::cout << "prefix: '" << this->prefix << "'" << std::endl;
	// std::cout << "command: '" << this->command << "'" << std::endl;
	// std::cout << "params: " << 
	// 	for_each(this->params.begin(), this->params.end(), affStr)
	// 	<< std::endl;
	// std::cout << "trailing: '" << this->trailing << "'" << std::endl;
	// std::cout << std::endl;
}