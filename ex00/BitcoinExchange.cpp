#include "BitcoinExchange.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cerrno>


BitcoinExchange::BitcoinExchange() : _database() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange &other) : _database(other._database) {}

BitcoinExchange &BitcoinExchange::operator=(const BitcoinExchange &other)
{
	if (this != &other)
		_database = other._database;
	return *this;
}

BitcoinExchange::~BitcoinExchange() {}

bool BitcoinExchange::isLeapYear(int year) const
{
	if (year % 400 == 0)
		return true;
	if (year % 100 == 0)
		return false;
	return (year % 4 == 0);
}

int BitcoinExchange::daysInMonth(int year, int month) const
{
	if (month == 2)
	{
		if (isLeapYear(year))
			return 29;
		return 28;
	}

	if (month == 4 || month == 6 || month == 9 || month == 11)
		return 30;

	return 31;
}

bool BitcoinExchange::isValidDate(const std::string &date) const
{
	if (date.length() != 10)
		return false;

	if (date[4] != '-' || date[7] != '-')
		return false;

	for (int i = 0; i < 10; ++i)
	{
		if (i == 4 || i == 7)
			continue;
		if (date[i] < '0' || date[i] > '9')
			return false;
	}

	int year = std::atoi(date.substr(0, 4).c_str());
	int month = std::atoi(date.substr(5, 2).c_str());
	int day = std::atoi(date.substr(8, 2).c_str());

	if (year < 1)
		return false;

	if (month < 1 || month > 12)
		return false;

	if (day < 1 || day > daysInMonth(year, month))
		return false;

	return true;
}

void BitcoinExchange::loadDatabase(const std::string &filename)
{
	std::ifstream file(filename.c_str());

	if (!file.is_open())
	{
		std::cerr << "Error: could not open database file." << std::endl;
		return;
	}

	std::string line;

	if (!std::getline(file, line))
		return;

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;

		std::stringstream ss(line);

		std::string date;
		std::string rateString;

		if (!std::getline(ss, date, ','))
			continue;

		if (!std::getline(ss, rateString))
			continue;

		if (!isValidDate(date))
			continue;

		char *end;
		const char *str = rateString.c_str();

		errno = 0;
		float rate = static_cast<float>(std::strtod(str, &end));

		if (str == end || *end != '\0' || errno == ERANGE)
			continue;

		if (rate < 0)
			continue;

		_database[date] = rate;
	}
}

void BitcoinExchange::processInput(const std::string &filename) const
{
	std::ifstream file(filename.c_str());

	if (!file.is_open())
	{
		std::cerr << "Error: could not open file." << std::endl;
		return;
	}

	std::string line;

	if (!std::getline(file, line))
		return;

	while (std::getline(file, line))
	{
		if (line.empty())
			continue;

		std::size_t separator = line.find('|');

		if (separator == std::string::npos)
		{
			std::cerr << "Error: bad input => " << line << std::endl;
			continue;
		}

		std::string date = line.substr(0, separator);
		std::string valueString = line.substr(separator + 1);

		while (!date.empty() && date[date.length() - 1] == ' ')
			date.erase(date.length() - 1);

		while (!valueString.empty() && valueString[0] == ' ')
			valueString.erase(0, 1);

		if (!isValidDate(date))
		{
			std::cerr << "Error: bad input => " << date << std::endl;
			continue;
		}

		char *end;
		const char *str = valueString.c_str();

		errno = 0;
		double value = std::strtod(str, &end);

		if (str == end || *end != '\0' || errno == ERANGE)
		{
			std::cerr << "Error: bad input => " << date << std::endl;
			continue;
		}

		if (value < 0)
		{
			std::cerr << "Error: not a positive number." << std::endl;
			continue;
		}

		if (value > 1000)
		{
			std::cerr << "Error: too large a number." << std::endl;
			continue;
		}

		if (_database.empty())
		{
			std::cerr << "Error: database is empty." << std::endl;
			continue;
		}

		std::map<std::string, float>::const_iterator it;
		it = _database.lower_bound(date);

		if (it == _database.end())
		{
			--it;
		}
		else if (it->first != date)
		{
			if (it == _database.begin())
			{
				std::cerr << "Error: date is before database range." << std::endl;
				continue;
			}
			--it;
		}

		double result = value * it->second;

		std::cout << date << " => " << value
				  << " = " << result << std::endl;
	}
}