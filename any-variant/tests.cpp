#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include <vector>
#include <any>
#include <variant>

#include "catch.hpp"

using namespace std;

TEST_CASE("any")
{
    std::any anything;

    REQUIRE(anything.has_value() == false);

    anything = 3;
    anything = 3.14;
    anything = "text"s;
    anything = vector{1, 2, 3};

    REQUIRE(anything.has_value());

    SECTION("any_cast")
    {
        auto vec = std::any_cast<std::vector<int>>(anything);
        REQUIRE(vec == vector{1, 2, 3});

        REQUIRE_THROWS_AS(std::any_cast<int>(anything), std::bad_any_cast);
    }

    SECTION("any_cast with pointer")
    {
        vector<int>* vec = std::any_cast<std::vector<int>>(&anything);

        REQUIRE(vec != nullptr);
        REQUIRE(*vec == vector{1, 2, 3});

        REQUIRE(std::any_cast<int>(&anything) == nullptr);
    }

    SECTION("any + RTTI")
    {
        const type_info& type_desc = anything.type();

        std::cout << type_desc.name() << "\n";
    }
}

////////////////////////////////////
// wide interfaces

class Observer
{
public:
    virtual void update(const std::any& sender, const string& msg) = 0;
    virtual ~Observer() = default;
};

class TempMonitor
{
    std::vector<Observer*> observes_;
public:
    void notify()
    {
        for(const auto& o : observes_)
            o->update(this, std::to_string(get_temp()));
    }
    double get_temp() const
    {
        return 23.88;
    }
};

class Logger : public Observer
{
public:
    void update(const std::any& sender, const string& msg) override
    {
        TempMonitor* const* monitor = std::any_cast<TempMonitor*>(&sender);
        if (monitor)
            (*monitor)->get_temp();
    }
};

//////////////////////////////////////////////////
//////////////////////////////////////////////////

struct NoDefaultConstructible
{
    int value;

    NoDefaultConstructible(int v)
        : value {v}
    {
    }
};

TEST_CASE("std::variant")
{
    std::variant<int, double, std::string, std::vector<int>> v1;

    REQUIRE(std::holds_alternative<int>(v1));
    REQUIRE(std::get<int>(v1) == 0);

    std::variant<std::monostate, NoDefaultConstructible, int> v2;
    REQUIRE(v2.index() == 0);    

    v1 = 3.14;
    v1 = "text"s;
    v1 = vector{1, 2, 3};

    REQUIRE(std::get<std::vector<int>>(v1) == vector{1, 2, 3});
    REQUIRE_THROWS_AS(std::get<int>(v1), std::bad_variant_access);

    REQUIRE(std::get_if<int>(&v1) == nullptr);
    REQUIRE(*std::get_if<std::vector<int>>(&v1) == vector{1, 2, 3});
}

struct Value
{
    float v;

    Value(float v)
        : v {v}
    {
    }

    ~Value()
    {
        cout << "~Value(" << v << ")\n";
    }
};

struct Evil
{
    string v;

    Evil(string v)
        : v {std::move(v)}
    {
    }

    Evil(Evil&& other)
    {
        throw std::runtime_error("42");
    }
};

TEST_CASE("valueless variant")
{
    variant<Value, Evil> v {12.0f};

    try
    {
        v.emplace<Evil>(Evil {"evil"});
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    REQUIRE(v.valueless_by_exception());
    REQUIRE(v.index() == std::variant_npos);
}

/////////////////////////
// visiting std::variant

struct Printer
{
    void operator()(int x) { std::cout << "int: " << x << "\n"; }
    void operator()(double x) { std::cout << "double: " << x << "\n"; }
    void operator()(const std::string& s) { std::cout << "string: " << s << "\n"; }
};

template <typename... Closures>
struct overload : Closures...
{
    using Closures::operator()...; // since C++17
};

template<typename... Closures>
overload(Closures...) -> overload<Closures...>;

[[nodiscard]] std::variant<std::string, std::errc> load_content(const std::string& filename)
{
    if (filename == "")
        return std::errc::bad_file_descriptor;
    return "content"s;
}

TEST_CASE("visiting variant")
{
    std::variant<int, double, string> v1 = 3.14;
    
    std::visit(Printer{}, v1);

    auto printer = [](const auto& v) { std::cout << typeid(v).name() << " - " << v << "\n"; };
    std::visit(printer, v1);

    // local_printer
    auto local_printer = overload {
        [](int x) { std::cout << "int: " << x << "\n"; },
        [](double x) { std::cout << "double: " << x << "\n"; },
        [](const std::string& s) { std::cout << "string: " << s << "\n"; }
    };

    std::visit(local_printer, v1);
}