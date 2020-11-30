#include <algorithm>
#include <iostream>
#include <map>
#include <mutex>
#include <numeric>
#include <queue>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

TEST_CASE("if with initializer")
{
    std::vector data = {1, 42, 665, 3453, 6576};

    if (auto pos = std::find(begin(data), end(data), 665); pos != end(data))
    {
        std::cout << "Found..." << *pos << "\n";
    }
    else
    {
        std::cout << "Item not found...\n";
        assert(pos == end(data));
    }

    SECTION("with structure bindings")
    {
        std::map<int, string> dict = {{1, "jeden"}};

        if (auto [pos, was_inserted] = dict.emplace(1, "one"); was_inserted)
        {
            const auto& [key, value] = *pos;
            std::cout << "Item " << key << " - " << value << " was inserted...\n";
        }
        else
        {
            std::cout << "Item with key " << pos->first << " was already in map...\n";
        }
    }
}

TEST_CASE("if with mutex")
{
    std::queue<int> q;
    std::mutex mtx_q;

    // thread1
    {
        lock_guard lk {mtx_q};
        q.push(42);
    }

    SECTION("thread2")
    {
        if (std::lock_guard lk {mtx_q}; !q.empty())
        {
            auto value = q.front();
            q.pop();
        } // mtx.unlock()
    }
}

template <typename T>
auto make_computation(T arg)
{
    puts(__PRETTY_FUNCTION__);
    return arg * arg;
}

template <typename T>
auto optimized_computation(T arg)
{
    puts(__PRETTY_FUNCTION__);
    return std::pow(arg, 2.0);
}

namespace BeforeCpp17
{
    template <typename T>
    auto compute(T obj) -> std::enable_if_t<std::is_floating_point_v<T>, T>
    {
        return optimized_computation(obj);
    }

    template <typename T>
    auto compute(T obj) -> std::enable_if_t<!std::is_floating_point_v<T>, T>
    {
        return make_computation(obj);
    }
}

template <typename T>
auto compute(T obj)
{
    if constexpr (std::is_floating_point_v<T>)
    {
        return optimized_computation(obj);
    }
    else
    {
        return make_computation(obj);
    }
}

TEST_CASE("compute")
{
    compute(42);
    compute(4.14);
}

template <typename T1, typename T2, size_t N, size_t K>
auto my_copy(T1 (&source)[N], T2 (&target)[K])
{
    static_assert(N >= K, "size of targer array must be larger than size of source");

    if constexpr (std::is_same_v<T1, T2> && std::is_trivially_copyable_v<T1>)
    {
        memcpy(target, source, N * sizeof(T1));
        std::cout << "my_copy by memcpy\n";
    }
    else
    {
        for (size_t i = 0; i < N; ++i)
            target[i] = source[i];
        std::cout << "my_copy by loop\n";
    }
}

TEST_CASE("mcopy")
{
    SECTION("copy using loop")
    {
        std::string words1[] = {"one", "two", "three"};
        std::string words2[3];

        my_copy(words1, words2);

        REQUIRE(std::equal(begin(words2), end(words2), begin(words1)));
    }

    SECTION("copy using memcpy")
    {
        int tab1[] = {1, 2, 3};
        int tab2[3];

        my_copy(tab1, tab2);

        REQUIRE(std::equal(begin(tab1), end(tab1), begin(tab2)));
    }
}

/////////////////////////////////////
//  if constexpr + variadic templates

// void print()
// {
//     std::cout << "\n";
// }

template <typename Head, typename... Tail>
void print(const Head& h, const Tail&... tail)
{
    std::cout << h << " ";
    
    if constexpr(sizeof...(tail) > 0)
        print(tail...); 
    else
        std::cout << "\n";    
}

template <typename... Ts>
struct get_size
{
    static constexpr size_t value = sizeof...(Ts);
};

TEST_CASE("variadic print")
{
    static_assert(get_size<int, double, string>::value == 3);

    print(1, 3.14, "text"s, "abc");
      // print(1, tail(3.14, "text"s, "abc"));
      //    print(3.14, tail("text"s, "abc"));
      //        print("text"s, tail("abc"));
      //            print("abc", tail())
}