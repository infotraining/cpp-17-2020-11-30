#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

namespace Cpp98
{
    void fold_98()
    {
        std::vector<int> vec = {1, 2, 3, 4, 5};

        auto sum = std::accumulate(std::begin(vec), std::end(vec), 0);
        std::cout << "sum: " << sum << "\n";

        auto result = std::accumulate(std::begin(vec), std::end(vec), "0"s,
            [](const std::string& reduced, int item) {
                return "("s + reduced + " + "s + std::to_string(item) + ")"s;
            });

        std::cout << result << "\n";
    }
}

namespace BeforeCpp17
{
    template <typename T>
    auto sum(const T& last)
    {
        return last;
    }

    template <typename THead, typename... TTail>
    auto sum(const THead& head, const TTail&... tail)
    {
        return head + sum(tail...);
    }
}

template <typename... TArgs>
auto sum(TArgs... args)
{
    return (... + args); // (((arg1 + arg2) + arg3) + arg4) - unary left fold
}

template <typename... TArgs>
auto sum_right(TArgs... args)
{
    return (args + ...); // (arg1 + (arg2 + (arg3 + arg4))) - unary right fold
}

////////////////////////////////
// print(TArgs...)

namespace BeforeCpp17
{
    void print()
    {
        std::cout << "\n";
    }

    template <typename THead, typename... TTail>
    void print(const THead& head, const TTail&... tail)
    {
        std::cout << head << " ";
        print(tail...);
    }
}

template <typename... TArgs>
void print(const TArgs&... args)
{
    bool is_first = true;
    auto with_space = [&is_first](const auto& arg) {
        if (!is_first)
            std::cout << " ";
        is_first = false;
        return arg;
    };

    (std::cout << ... << with_space(args)) << "\n"; // binary left fold
}

template <typename... TArgs>
void print_lines(const TArgs&... args)
{
    (..., (std::cout << args << "\n"));
}

//////////////////////////////////////////////////////////////////
// call(TArgs&&...)

void f(int x)
{
    std::cout << "f(" << x << ")\n";
}

template <typename TFunc, typename... TArgs>
void call(TFunc f, TArgs&&... args)
{
    (..., f(std::forward<TArgs>(args))); // left fold - ,
}

TEST_CASE("fold expressions")
{
    Cpp98::fold_98();

    REQUIRE(sum(1, 2, 3, 4) == 10);
    REQUIRE(sum(1, 2, 3.14, 4) == Approx(10.14));

    print(1, 2, 3, "4_string"s);
    print_lines(1, 2, 3, "4_string"s);

    call(f, 1, 2, 3, 4, 5);
}

template <typename... Args>
bool all_true(const Args&... args)
{
    return (... && args);
}

TEST_CASE("identity element")
{
    REQUIRE(all_true() == true);
    REQUIRE(all_true(true, true, false) == false);
}