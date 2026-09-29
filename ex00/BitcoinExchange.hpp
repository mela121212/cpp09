#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <map>
#include <string>

class BitcoinExchange
{
private:
	std::map<std::string, float> _database;

	bool isValidDate(const std::string &date) const;
	bool isLeapYear(int year) const;
	int daysInMonth(int year, int month) const;

public:
	BitcoinExchange();
	BitcoinExchange(const BitcoinExchange &other);
	BitcoinExchange &operator=(const BitcoinExchange &other);
	~BitcoinExchange();

	void loadDatabase(const std::string &filename);
	void processInput(const std::string &filename) const;
};

#endif