#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <vector>
#include <thread>
#include <new>

#include "catch.hpp"

using namespace std;

namespace Cpp11
{
    tuple<int, int, double> calc_stats(const vector<int>& data)
    {
        vector<int>::const_iterator min_pos, max_pos;
        tie(min_pos, max_pos) = minmax_element(data.begin(), data.end());

        double avg = accumulate(data.begin(), data.end(), 0.0) / data.size();

        return make_tuple(*min_pos, *max_pos, avg);
    }
}

template <typename TContainer>
decltype(auto) get_item_at(TContainer& container, size_t index)
{
    return container[index];
}

TEST_CASE("auto vs. decltype(auto)")
{
    vector vec = {1, 2, 3};

    get_item_at(vec, 1) = 13;

    REQUIRE(vec[1] == 13);
}

template <typename TContainer>
auto calc_stats(const TContainer& data)
{
    auto [min_pos, max_pos] = minmax_element(std::begin(data), std::end(data));

    double avg = accumulate(std::begin(data), std::end(data), 0.0) / std::size(data);

    return tuple(*min_pos, *max_pos, avg);
}

TEST_CASE("Before C++17")
{
    int data[] = {4, 42, 665, 1, 123, 13};

    SECTION("C++11")
    {
        auto result = calc_stats(data);
        REQUIRE(get<0>(result) == 1);
        REQUIRE(get<1>(result) == 665);

        int min, max;
        double avg;
        tie(min, max, avg) = calc_stats(data);

        REQUIRE(min == 1);
        REQUIRE(max == 665);
        REQUIRE(avg == Approx(141.333));
    }

    SECTION("C++17")
    {
        auto [min, max, avg] = calc_stats(data);

        REQUIRE(min == 1);
        REQUIRE(max == 665);
        REQUIRE(avg == Approx(141.333));
    }
}

std::array<int, 3> get_coord()
{
    return array {1, 2, 3};
}

struct ErrorCode
{
    const int errc;
    const char* m;
};

ErrorCode open_file(const char* filename)
{
    return ErrorCode {13, "Error#13"};
}

TEST_CASE("structured bindings - details")
{
    SECTION("native arrays")
    {
        int tab[2] = {1, 2};

        auto [x, y] = tab;

        REQUIRE(x == 1);
        REQUIRE(y == 2);
    }

    SECTION("std::array")
    {
        auto [x, y, z] = get_coord();

        REQUIRE(x == 1);
        REQUIRE(z == 3);
    }

    SECTION("std::pair")
    {
        std::map<int, std::string> dict = {{1, "one"}, {2, "three"}};

        auto [pos, was_inserted] = dict.insert(std::pair(3, "three"s));

        REQUIRE(pos->first == 3);
        REQUIRE(pos->second == "three"s);

        const auto& [key, value] = *pos;
        REQUIRE(key == 3);
        REQUIRE(value == "three"s);

        for (const auto& item : dict)
        {
            std::cout << item.first << " - " << item.second << "\n";
        }

        SECTION("Since C++17")
        {
            for (const auto& [key, value] : dict)
            {
                std::cout << key << " - " << value << "\n";
            }
        }
    }

    SECTION("struct")
    {
        const auto [error_code, message] = open_file("file.dat");
        REQUIRE(error_code == 13);
        REQUIRE(message == "Error#13"s);
    }
}

struct Timestamp
{
    int h, m, s;
};

TEST_CASE("structured bindings - how it works")
{
    Timestamp t1{11, 50, 0};

    auto& [hour, min, sec] = t1;

    hour = 12;

    REQUIRE(t1.h == 12);

    SECTION("works like this - 1")
    {
        auto& entity = t1;

        auto& hours = t1.h;
        auto& min = t1.m;
        auto& sec = t1.s;
    }

    Timestamp t2{11, 58, 0};
    alignas(128) auto [t2_hour, t2_min, t2_sec] = t2;
    
    SECTION("works like this - 2")
    {
        alignas(128) auto entity = t2;
        auto& t2_hours = t2.h;
        auto& t2_min =   t2.m;
        auto& t2_sec =   t2.s;
    }
}

void count_events(int& counter)
{
    for(size_t i = 0; i < 1'000'000; ++i)
        // if even occurs
        ++counter;
}

TEST_CASE("alignas")
{
    int counter1{};
    alignas(std::hardware_destructive_interference_size) int counter2{};

    std::thread thd1{&count_events, std::ref(counter1)};
    std::thread thd2{&count_events, std::ref(counter2)};

    thd1.join();
    thd2.join();
}