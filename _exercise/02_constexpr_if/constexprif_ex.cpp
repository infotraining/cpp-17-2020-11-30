#include <iostream>
#include <list>
#include <vector>

#include "catch.hpp"

using namespace std;

namespace Explain
{
    template <typename T1, typename T2>
    struct is_same : std::false_type
    {
    };

    template <typename T>
    struct is_same<T, T> : std::true_type
    {        
    };

    // since C++17
    template <typename T1, typename T2>
    constexpr bool is_same_v = is_same<T1, T2>::value;
}

TEST_CASE("is_same")
{
    static_assert(Explain::is_same<int, double>::value == false);
    static_assert(Explain::is_same_v<int, int> == true);
}

template <typename Iterator>
void advance_it(Iterator& it, size_t n)
{
    using iterator_tag = typename std::iterator_traits<Iterator>::iterator_category;

    if constexpr (std::is_same_v<iterator_tag, random_access_iterator_tag>)
    {        
        it += n;
        std::cout << "it += n\n";
    }
    else
    {
        for(size_t i = 0; i < n; ++i)
            ++it;
        std::cout << "loop using it\n";
    }
}

TEST_CASE("constexpr-if with iterator categories")
{
    SECTION("random_access_iterator")
    {
        vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = data.begin();

        advance_it(it, 3);

        REQUIRE(*it == 4);
    }

    SECTION("input_iterator")
    {
        list<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        auto it = data.begin();

        advance_it(it, 3);

        REQUIRE(*it == 4);
    }
}