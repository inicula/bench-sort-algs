#pragma once
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <cstring>
#include <queue>

using u64 = std::uint64_t;

std::vector<std::string> split(const std::string_view s, const char delim = ' ')
{
    auto start = s.begin();
    std::vector<std::string> res;

    for(auto it = s.begin(); it != s.end(); ++it)
    {
        if(*it == delim)
        {
            res.emplace_back(start, it);
            start = it + 1;
        }
    }
    res.emplace_back(start, s.end());

    return res;
}

template<typename Gen>
__always_inline std::string make_string(std::size_t n, Gen& gen)
{
    std::uniform_int_distribution<int> distrib(0, 127);

    std::string str;
    str.reserve(n);

    while(n != 0)
    {
        str.push_back(static_cast<char>(distrib(gen)));
        --n;
    }

    return str;
}

template<typename T, typename U, typename Gen>
std::vector<T> random(std::size_t n, const U min, const U max, Gen& gen)
{
    using distrib_type = std::conditional_t<
        std::is_same_v<T, std::string>, std::uniform_int_distribution<int>,
        std::conditional_t<std::is_integral_v<T>, std::uniform_int_distribution<T>,
                           std::uniform_real_distribution<T>>>;

    distrib_type distrib(min, max);

    std::vector<T> vec;
    vec.reserve(n);

    while(n != 0)
    {
        if constexpr(std::is_same_v<T, std::string>)
        {
            vec.push_back(make_string(distrib(gen), gen));
        }
        else
        {
            vec.push_back(distrib(gen));
        }
        --n;
    }

    return vec;
}

template<typename T, typename U, typename Gen, typename Comp>
std::vector<T> almost_sorted(const std::size_t n, const U min, const U max, Gen& g,
                             Comp c = std::less<T>{})
{
    std::vector<T> vec = random<T>(n, min, max, g);
    auto endpoint = vec.end() - (vec.size() / 20);
    std::sort(vec.begin(), endpoint, c);
    return vec;
}

template<typename T, typename U, typename Gen, typename Comp = std::less<T>>
std::vector<T> sorted(const std::size_t n, const U min, const U max, Gen& g, const Comp& c = Comp())
{
    std::vector<T> vec = random<T>(n, min, max, g);
    std::sort(vec.begin(), vec.end(), c);
    return vec;
}

inline std::vector<unsigned int> first_n(const unsigned int n)
{
    std::vector<unsigned int> vec;
    vec.resize(n);
    std::iota(vec.begin(), vec.end(), 0u);
    return vec;
}

template<typename T>
unsigned long long get_timepoint_count(const T& timepoint)
{
    const auto present = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(present - timepoint).count();
}

template<typename T, typename U, typename Gen>
std::vector<T> get_vector(const std::size_t n, const std::string_view arg, const U minv,
                          const U maxv, Gen& g)
{
    if constexpr(std::is_same_v<T, unsigned>)
    {
        if(arg == "firstn")
        {
            return first_n(n);
        }
    }
    if(arg == "random")
    {
        return random<T>(n, minv, maxv, g);
    }
    else if(arg == "sorted")
    {
        return sorted<T>(n, minv, maxv, g);
    }
    else if(arg == "almostsorted")
    {
        return almost_sorted<T>(n, minv, maxv, g);
    }
    return {};
}

consteval u64 exp(u64 x, u64 y)
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

constexpr const char* predefined = "plt.xscale('log', base=2)\n"
                                   "plt.yscale('log', base=10)\n"
                                   "plt.gca().set_xticks(xloc)\n"
                                   "plt.gca().set_yticks(yloc)\n"
                                   "plt.grid(True)\n"
                                   "plt.xlabel('no. elements')\n"
                                   "plt.ylabel('nanoseconds')\n";

const std::array<std::string, 6> markers = {"'bs'", "'g^'", "'r*'", "'mX'", "'co'", "'kH'"};

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

std::string subplot_pos(unsigned i)
{
    return "plt.subplot(22" + std::to_string(i) + ')';
}

std::string subplot_title(const std::string& str)
{
    return "plt.title('" + str + "')";
}

std::string plot_command(const std::vector<std::vector<u64>>& timpi)
{
    auto current_marker = markers.cbegin();
    std::string command = "plt.plot(";
    for(auto& tmetoda : timpi)
    {
        command = command + size_range(tmetoda.size()) + ',' + timepoints_array(tmetoda) +
                  ',' + *current_marker + ',';
        ++current_marker;
    }
    command += ')';
    return command;
}

std::string legends(const std::vector<std::string>& leg)
{
    std::string command = "plt.legend([";
    for(auto l : leg)
    {
        command = command + "'" + l + "', ";
    }
    command += "])";
    return command;
}
