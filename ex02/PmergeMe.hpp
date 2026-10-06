#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <string>
#include <iostream>

class PmergeMe
{
private:
    std::vector<int> _vector;
    std::deque<int> _deque;

    void fordJohnsonVector(std::vector<int>& v);
    void fordJohnsonDeque(std::deque<int>& d);

    void insertVector(std::vector<int>& v, int value, int position);
    void insertDeque(std::deque<int>& d, int value, int position);

    int lowerBoundVector(const std::vector<int>& v, int value, int end) const;
    int lowerBoundDeque(const std::deque<int>& d, int value, int end) const;

std::vector<int> jacobsthalOrder(int n) const;

public:
    PmergeMe();
    PmergeMe(const PmergeMe& other);
    PmergeMe& operator=(const PmergeMe& other);
    ~PmergeMe();

    void parse(int argc, char **argv);

    void sortVector();
    void sortDeque();

    const std::vector<int>& getVector() const;
    const std::deque<int>& getDeque() const;

};

#endif
