# include "../../includes/Server.hpp"

void	Server::Parse( std::string message ) 
{
	//std::string message = data; //jsp si ca marche mais il nous faut une string
	size_t		pos = 0;
	std::string	res;
	Parser		parsed;

	if (!this->_parsedMessages.empty())
		this->_parsedMessages.clear();

	message.erase( std::remove( message.begin(), message.end(), '\r' ), message.end() );
	while ( getStringUntil( message, res, '\n', pos ) )
	{
		if (res != "") {
			parsed.parseMessage( res );
			this->_parsedMessages.push_back( parsed );
		}
		res = "";
	}
}