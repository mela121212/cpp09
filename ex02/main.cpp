#include "PmergeMe.hpp"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <stdexcept>

static void printVector(const std::vector<int>& v)
{
    for (std::size_t i = 0; i < v.size(); ++i)
        std::cout << v[i] << (i + 1 == v.size() ? '\n' : ' ');
}

int main(int argc, char **argv)
{
    try
    {
        PmergeMe p;
        p.parse(argc, argv);

        std::cout << "Before: ";
        printVector(p.getVector());

        std::clock_t start = std::clock();
        p.sortVector();
        std::clock_t end = std::clock();
        double vectorTime = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000000.0;

        start = std::clock();
        p.sortDeque();
        end = std::clock();
        double dequeTime = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000000.0;

        std::cout << "After:  ";
        printVector(p.getVector());
        std::cout << std::fixed << std::setprecision(5);
        std::cout << "Time to process a range of " << p.getVector().size()
                  << " elements with std::vector : " << vectorTime << " us\n";
        std::cout << "Time to process a range of " << p.getDeque().size()
                  << " elements with std::deque : " << dequeTime << " us\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
