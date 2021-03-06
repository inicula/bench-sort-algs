#pragma once
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <array>
#include <iostream>

struct None
{
};

using u64 = std::uint64_t;
constexpr u64 U64MAX = std::numeric_limits<u64>::max();

std::vector<unsigned int> first_n(const unsigned int n)
{
    std::vector<unsigned int> vec;
    vec.resize(n);
    std::iota(vec.begin(), vec.end(), 0u);
    return vec;
}

template<typename Gen>
std::string make_string(std::size_t n, Gen& gen)
{
    std::uniform_int_distribution<int> distrib(65, 122);

    std::string str;
    str.reserve(n);

    while(n != 0)
    {
        str.push_back(static_cast<char>(distrib(gen)));
        --n;
    }

    return str;
}

template<typename T, typename U = T, typename Gen = std::mt19937>
std::vector<T> random(std::size_t n, const U min, const U max, Gen& gen)
{
    auto distrib = [&]()
    {
        if constexpr(std::is_same_v<T, std::string>)
        {
            return None{};
        }
        else if constexpr(std::is_integral_v<T>)
        {
            return std::uniform_int_distribution<T>(min, max);
        }
        else
        {
            return std::uniform_real_distribution<T>(min, max);
        }
    }();

    std::vector<T> vec;
    vec.reserve(n);

    while(n != 0)
    {
        if constexpr(std::is_same_v<T, std::string>)
        {
            vec.push_back(make_string(max, gen));
        }
        else
        {
            vec.push_back(distrib(gen));
        }
        --n;
    }

    return vec;
}

template<typename T, typename Comp = std::less<T>, typename U = T, typename Gen = std::mt19937>
std::vector<T> almost_sorted(const std::size_t n, const U min, const U max, Gen& g,
                             Comp c = Comp{})
{
    std::vector<T> vec = random<T>(n, min, max, g);
    auto endpoint = vec.end() - (vec.size() / 20);
    std::sort(vec.begin(), endpoint, c);
    return vec;
}

template<typename T, typename Comp = std::less<T>, typename U = T, typename Gen = std::mt19937>
std::vector<T> sorted(const std::size_t n, const U min, const U max, Gen& g,
                      const Comp& c = Comp())
{
    if constexpr(std::is_unsigned_v<T>)
    {
        std::vector<T> vec;
        vec.resize(n);
        std::iota(vec.begin(), vec.end(), T(0));
        std::sort(vec.begin(), vec.end(), c);
        return vec;
    }
    else
    {
        std::vector<T> vec = random<T>(n, min, max, g);
        std::sort(vec.begin(), vec.end(), c);
        return vec;
    }
}

template<typename T>
u64 get_timepoint_count(const T& timepoint)
{
    const auto present = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(present - timepoint).count();
}

constexpr u64 powu64(u64 x, u64 y)
{
    if(y == 0)
    {
        return 1;
    }
    if(x == 2)
    {
        return 1 << y;
    }
    u64 res = x;
    for(u64 i = 1; i < y; ++i)
    {
        res *= x;
    }
    return res;
}

template<typename T>
constexpr const char* predefined = "plt.xscale('log', base=2)\n"
                                   "plt.yscale('log', base=10)\n"
                                   "plt.gca().set_xticks(xloc)\n"
                                   "plt.gca().set_yticks(yloc)\n"
                                   "plt.grid(True)\n"
                                   "plt.xlabel('no. elements')\n"
                                   "plt.ylabel('nanoseconds')\n";

template<>
constexpr const char* predefined<std::string> = "plt.xscale('log', base=2)\n"
                                                "plt.yscale('log', base=10)\n"
                                                "plt.gca().set_xticks(xloc)\n"
                                                "plt.gca().set_yticks(yloc)\n"
                                                "plt.grid(True)\n"
                                                "plt.xlabel('no. bytes in container (x 4)')\n"
                                                "plt.ylabel('nanoseconds')\n";

template<typename T>
constexpr const char* suptitle = "";

template<>
constexpr const char* suptitle<unsigned> = "plt.suptitle('unsigned values')";

template<>
constexpr const char* suptitle<double> = "plt.suptitle('double values')";

template<>
constexpr const char* suptitle<std::string> = "plt.suptitle('std::string objects')";

const std::array<std::string, 7> markers = {"'bs'", "'g^'", "'rP'", "'mD'", "'co'", "'yX'", "'kh'"};

std::string size_range(unsigned i)
{
    return "[2**x for x in range(1, " + std::to_string(i + 1) + ")]";
}

std::string timepoints_array(const std::vector<u64>& vec)
{
    std::string str = "[";
    for(auto el : vec)
    {
        str += std::to_string(el);
        str += ',';
    }
    str += ']';
    return str;
}

void subplot_pos(unsigned i)
{
    std::cout << "plt.subplot(22" << i << ")\n";
}

void subplot_title(const std::string& str)
{
    std::cout << "plt.title('" << str << "')\n";
}

void plot_command(const std::vector<std::vector<u64>>& timpi)
{
    auto current_marker = markers.cbegin();
    std::cout << "plt.plot(";
    for(auto& tmetoda : timpi)
    {
        std::cout << size_range(tmetoda.size()) << ',' << timepoints_array(tmetoda) << ','
                  << *current_marker << ',';
        ++current_marker;
    }
    std::cout << ")\n";
}

u64 string_limit(const double n_elements)
{
    return (2.0 * std::round(std::sqrt(n_elements)));
}

template<typename T>
consteval auto get_masks()
{
    constexpr int digits = std::numeric_limits<T>::digits - 1;
    std::array<T, digits + 1> d = {};
    int current = 0;
    while(current <= digits)
    {
        d[digits - current] = static_cast<T>(1) << current;
        ++current;
    }
    return d;
}

template<typename T>
int max_bit_pos(const T value)
{
    constexpr auto masks = get_masks<T>();
    int c_bitpos = std::numeric_limits<T>::digits - 1;
    for(auto mask : masks)
    {
        if(value & mask)
        {
            return c_bitpos;
        }
        --c_bitpos;
    }
    return -1;
}
