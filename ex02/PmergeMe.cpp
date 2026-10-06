#include "PmergeMe.hpp"
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <stdexcept>

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe& other) : _vector(other._vector), _deque(other._deque) {}

PmergeMe& PmergeMe::operator=(const PmergeMe& other)
{
    if (this != &other)
    {
        _vector = other._vector;
        _deque = other._deque;
    }
    return *this;
}

PmergeMe::~PmergeMe() {}

void PmergeMe::parse(int argc, char **argv)
{
    if (argc < 2)
        throw std::runtime_error("Error");

    for (int i = 1; i < argc; ++i)
    {
        std::string s(argv[i]);

        if (s.empty())
            throw std::runtime_error("Error");

        for (std::string::size_type j = 0; j < s.size(); ++j)
        {
            if (s[j] < '0' || s[j] > '9')
                throw std::runtime_error("Error");
        }

        errno = 0;
        char *end = 0;
        long n = std::strtol(s.c_str(), &end, 10);

        if (errno == ERANGE || *end != '\0' || n > INT_MAX)
            throw std::runtime_error("Error");

        _vector.push_back(static_cast<int>(n));
        _deque.push_back(static_cast<int>(n));
    }

}

/*

* Returns the order in which pend elements must be inserted.
*
* Example:
*
* n = 1 -> 0
* n = 2 -> 0 1
* n = 3 -> 0 2 1
* n = 5 -> 0 2 1 4 3
*
* This is based on the Jacobsthal sequence.
  */
std::vector<int> PmergeMe::jacobsthalOrder(int n) const
{
    std::vector<int> order;

    if (n <= 0)
    return order;

    order.push_back(0);

    int previous = 1;
    int current = 3;
    int inserted = 1;

    while (inserted < n)
    {
        int last = current - 1;

        if (last >= n)
            last = n - 1;

        int i = last;

        while (i >= inserted)
        {
            order.push_back(i);
            --i;
        }

        inserted = last + 1;

        int next = current + 2 * previous;
        previous = current;
        current = next;
    }
    return order;
}

int PmergeMe::lowerBoundVector(const std::vector<int>& v, int value, int end) const
{
    int left = 0;
    int right = end;

    while (left < right)
    {
        int middle = left + (right - left) / 2;

        if (v[middle] < value)
            left = middle + 1;
        else
            right = middle;
    }
    return left;
}

int PmergeMe::lowerBoundDeque(const std::deque<int>& d,
int value,
int end) const
{
int left = 0;
int right = end;

while (left < right)
{
    int middle = left + (right - left) / 2;

    if (d[middle] < value)
        left = middle + 1;
    else
        right = middle;
}

return left;

}

/*

* Insert directly at a known position.
*
* We do NOT perform another lower_bound here.
* The position was already calculated by the caller.
  */
  void PmergeMe::insertVector(std::vector<int>& v,
  int value,
  int position)
  {
  v.insert(v.begin() + position, value);
  }

void PmergeMe::insertDeque(std::deque<int>& d,
int value,
int position)
{
d.insert(d.begin() + position, value);
}

/*

* Ford-Johnson / Merge-Insertion sort using std::vector.
*
* Each pair is:
*
* ```
  first  = small
  ```
* ```
  second = large
  ```
*
* The important thing is that we keep the association between
* small and large while sorting the large elements recursively.
  */
  void PmergeMe::fordJohnsonVector(std::vector<int>& v)
  {
  if (v.size() <= 1)
  return;

  /*

  * Create pairs.
  *
  * Example:
  *
  * 9 3 7 2 8 1
  *
  * becomes:
  *
  * (3,9) (2,7) (1,8)
    */
    std::vector< std::pair<int, int> > pairs;

  for (std::size_t i = 0; i + 1 < v.size(); i += 2)
  {
  int a = v[i];
  int b = v[i + 1];

   if (a < b)
       pairs.push_back(std::make_pair(a, b));
   else
       pairs.push_back(std::make_pair(b, a));

  }

  /*

  * If there is an odd element, keep it aside.
    */
    bool hasOdd = (v.size() % 2 != 0);
    int odd = 0;

  if (hasOdd)
  odd = v[v.size() - 1];

  /*

  * Extract the large elements.
    */
    std::vector<int> bigs;

  for (std::size_t i = 0; i < pairs.size(); ++i)
  bigs.push_back(pairs[i].second);

  /*

  * Recursively sort the large elements.
    */
    fordJohnsonVector(bigs);

  /*

  * Rebuild the pairs in the same order as the sorted bigs.
  *
  * We remove each pair after using it.
  *
  * This also works with duplicate values.
    */
    std::vector< std::pair<int, int> > sortedPairs;

  for (std::size_t i = 0; i < bigs.size(); ++i)
  {
  for (std::size_t j = 0; j < pairs.size(); ++j)
  {
  if (pairs[j].second == bigs[i])
  {
  sortedPairs.push_back(pairs[j]);
  pairs.erase(pairs.begin() + j);
  break;
  }
  }
  }

  /*

  * Main chain:
  *
  * First put the smallest element of the first pair.
  * Then put all the large elements.
  *
  * Example:
  *
  * pairs:
  * (3,7) (2,8) (5,9)
  *
  * main:
  * 3 7 8 9
    */
    std::vector<int> main;

  main.push_back(sortedPairs[0].first);

  for (std::size_t i = 0; i < sortedPairs.size(); ++i)
  main.push_back(sortedPairs[i].second);

  /*

  * pend contains all the remaining small elements.
  *
  * a1 was already inserted into main.
  *
  * pend:
  * a2 a3 a4 ...
    */
    std::vector<int> pend;

  for (std::size_t i = 1; i < sortedPairs.size(); ++i)
  pend.push_back(sortedPairs[i].first);

  /*

  * Insert pend elements using Jacobsthal order.
    */
    std::vector<int> order =
    jacobsthalOrder(static_cast<int>(pend.size()));

  for (std::size_t i = 0; i < order.size(); ++i)
  {
  int pendIndex = order[i];
  int value = pend[pendIndex];
   /*
    * pend[0] corresponds to sortedPairs[1]
    * pend[1] corresponds to sortedPairs[2]
    * etc.
    *
    * Therefore:
    *
    * sortedPairs[pendIndex + 1]
    */
   int correspondingLarge =
       sortedPairs[pendIndex + 1].second;

   /*
    * We only need to search before the corresponding
    * large element.
    *
    * Since main is sorted, find the position of large.
    */
   int end = lowerBoundVector(main, correspondingLarge, static_cast<int>(main.size()));

   /*
    * Now search for value only in [0, end).
    */
   int position = lowerBoundVector(main, value, end);

   /*
    * Insert directly.
    */
   insertVector(main, value, position);

  }

  /*

  * Insert the odd element.
  *
  * It has no corresponding large element, so we can search
  * through the complete main chain.
    */
    if (hasOdd)
    {
    int position = lowerBoundVector(
    main,
    odd,
    static_cast<int>(main.size())
    );

    insertVector(main, odd, position);
    }

  v = main;
  }

/*

* Same algorithm for std::deque.
  */
  void PmergeMe::fordJohnsonDeque(std::deque<int>& d)
  {
  if (d.size() <= 1)
  return;

  std::deque< std::pair<int, int> > pairs;

  for (std::size_t i = 0; i + 1 < d.size(); i += 2)
  {
  int a = d[i];
  int b = d[i + 1];

   if (a < b)
       pairs.push_back(std::make_pair(a, b));
   else
       pairs.push_back(std::make_pair(b, a));

  }

  bool hasOdd = (d.size() % 2 != 0);
  int odd = 0;

  if (hasOdd)
  odd = d[d.size() - 1];

  /*

  * Extract large elements.
    */
    std::deque<int> bigs;

  for (std::size_t i = 0; i < pairs.size(); ++i)
  bigs.push_back(pairs[i].second);

  /*

  * Recursively sort large elements.
    */
    fordJohnsonDeque(bigs);

  /*

  * Rebuild the pair order.
    */
    std::deque< std::pair<int, int> > sortedPairs;

  for (std::size_t i = 0; i < bigs.size(); ++i)
  {
  for (std::size_t j = 0; j < pairs.size(); ++j)
  {
  if (pairs[j].second == bigs[i])
  {
  sortedPairs.push_back(pairs[j]);
  pairs.erase(pairs.begin() + j);
  break;
  }
  }
  }

  /*

  * Build main chain.
    */
    std::deque<int> main;

  main.push_back(sortedPairs[0].first);

  for (std::size_t i = 0; i < sortedPairs.size(); ++i)
  main.push_back(sortedPairs[i].second);

  /*

  * Build pend.
    */
    std::deque<int> pend;

  for (std::size_t i = 1; i < sortedPairs.size(); ++i)
  pend.push_back(sortedPairs[i].first);

  /*

  * Jacobsthal insertion order.
    */
    std::vector<int> order = jacobsthalOrder(static_cast<int>(pend.size()));

  for (std::size_t i = 0; i < order.size(); ++i)
  {
  int pendIndex = order[i];
  int value = pend[pendIndex];

   int correspondingLarge =
       sortedPairs[pendIndex + 1].second;

   /*
    * Find the position of the corresponding large element.
    */
   int end = lowerBoundDeque(main,correspondingLarge,static_cast<int>(main.size()));

   /*
    * Search only before that large element.
    */
   int position = lowerBoundDeque(main, value, end);

   /*
    * Insert directly.
    */
   insertDeque(main, value, position);

  }

  /*

  * Insert odd element.
    */
    if (hasOdd)
    {
    int position = lowerBoundDeque(
    main,
    odd,
    static_cast<int>(main.size())
    );

    insertDeque(main, odd, position);
    }

  d = main;
  }

/*

* Public functions.
*
* These are intentionally small:
*
* sortVector()
* ```
   -> fordJohnsonVector(_vector)
  ```
*
* sortDeque()
* ```
   -> fordJohnsonDeque(_deque)
  ```
*
* The algorithm itself remains private.
  */
  void PmergeMe::sortVector()
  {
  fordJohnsonVector(_vector);
  }

void PmergeMe::sortDeque()
{
fordJohnsonDeque(_deque);
}

const std::vector<int>& PmergeMe::getVector() const
{
return _vector;
}

const std::deque<int>& PmergeMe::getDeque() const
{
return _deque;
}
