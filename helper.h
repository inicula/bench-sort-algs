#pragma once
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <cstring>

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

template<typename T, typename U, typename Gen, typename Comp = std::less<T>>
std::vector<T> almost_sorted(const std::size_t n, const U min, const U max, Gen& g,
                             Comp c = Comp{})
{
    std::vector<T> vec = random<T>(n, min, max, g);
    auto endpoint = vec.end() - (vec.size() / 20);
    std::sort(vec.begin(), endpoint, c);
    return vec;
}

template<typename T, typename U, typename Gen, typename Comp = std::less<T>>
std::vector<T> sorted(const std::size_t n, const U min, const U max, Gen& g, Comp c = Comp{})
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
auto get_timepoint_count(const T& timepoint)
{
    const auto present = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(present - timepoint).count();
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

consteval unsigned exp(unsigned x, unsigned y)
{
    if(y == 0)
    {
        return 1;
    }
    if(x == 2)
    {
        return 1 << y;
    }
    unsigned res = x;
    for(unsigned i = 1; i < y; ++i)
    {
        res *= x;
    }
    return res;
}
