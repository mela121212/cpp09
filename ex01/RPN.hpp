#ifndef RPN_HPP
#define RPN_HPP

#include <stack>
#include <string>

class RPN
{
private:
    std::stack<int> _stack;

    bool isOperator(const std::string &token) const;
    int calculate(int left, int right, char op) const;

public:
    RPN();
    RPN(const RPN &other);
    RPN &operator=(const RPN &other);
    ~RPN();

    void evaluate(const std::string &expression);
};

#endif