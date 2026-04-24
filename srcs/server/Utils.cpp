# include "../../includes/Server.hpp"

bool	getStringUntil(const std::string& input, std::string& result, char delimiter, size_t& startPos)
{
	// Vérifier si startPos dépasse la longueur de la chaîne
	if (startPos >= input.length()) {
		return false;
	}
	// Trouver la position de départ effective en sautant les delim
	size_t begin = startPos;
	while (begin < input.length() && input[begin] == delimiter) {
		++begin;
	}
	// Si tout est des delim ou que la chaîne est vide
	if (begin >= input.length()) {
		return false;
	}
	// Trouver la position du délimiteur ou la fin de la chaîne
	size_t end = begin;
	while (end < input.length() && input[end] != delimiter) {
		++end;
	}
	// Exclure les espaces en fin de mot
	size_t wordEnd = end;
	while (wordEnd > begin && input[wordEnd - 1] == delimiter) {
		--wordEnd;
	}
	// Extraire le résultat
	result = input.substr(begin, wordEnd - begin);
	// Mettre à jour startPos
	startPos = (end < input.length()) ? end + 1 : end;
	return !result.empty();
}