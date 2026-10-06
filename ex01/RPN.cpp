#include "RPN.hpp"
#include <sstream>
#include <iostream>
#include <stdexcept>

RPN::RPN() {}

RPN::RPN(const RPN &other) : _stack(other._stack) {}

RPN &RPN::operator=(const RPN &other)
{
    if (this != &other)
        _stack = other._stack;
    return *this;
}

RPN::~RPN() {}

bool RPN::isOperator(const std::string &token) const
{
    return token == "+" || token == "-" || token == "*" || token == "/";
}

int RPN::calculate(int left, int right, char op) const
{
    if (op == '+')
    {
        if (right > 0 && left > INT_MAX - right)
            throw std::runtime_error("integer overflow");
        if (right < 0 && left < INT_MIN - right)
            throw std::runtime_error("integer underflow");

        return left + right;
    }

    if (op == '-')
    {
        if (right < 0 && left > INT_MAX + right)
            throw std::runtime_error("integer overflow");
        if (right > 0 && left < INT_MIN + right)
            throw std::runtime_error("integer underflow");

        return left - right;
    }

    if (op == '*')
    {
        if (left > 0 && right > 0 && left > INT_MAX / right)
            throw std::runtime_error("integer overflow");

        if (left < 0 && right < 0 && left < INT_MAX / right)
            throw std::runtime_error("integer overflow");

        if (left > 0 && right < 0 && right < INT_MIN / left)
            throw std::runtime_error("integer underflow");

        if (left < 0 && right > 0 && left < INT_MIN / right)
            throw std::runtime_error("integer underflow");

        return left * right;
    }

    if (op == '/')
    {
        if (right == 0)
            throw std::runtime_error("division by zero");

        if (left == INT_MIN && right == -1)
            throw std::runtime_error("integer overflow");

        return left / right;
    }

    throw std::runtime_error("invalid operator");
}

void RPN::evaluate(const std::string &expression)
{
    std::stringstream ss(expression);
    std::string token;

    while (ss >> token)
    {
        if (token.length() == 1 && token[0] >= '0' && token[0] <= '9')
        {
            _stack.push(token[0] - '0');
        }
        else if (isOperator(token))
        {
            if (_stack.size() < 2)
                throw std::runtime_error("not enough operands");

            int right = _stack.top();
            _stack.pop();

            int left = _stack.top();
            _stack.pop();

            int result = calculate(left, right, token[0]);
            _stack.push(result);
        }
        else
        {
            throw std::runtime_error("invalid token");
        }
    }

    if (_stack.size() != 1)
        throw std::runtime_error("invalid expression");

    std::cout << _stack.top() << std::endl;
}