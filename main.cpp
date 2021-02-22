#include "helper.h"
#include <iostream>

template<typename It>
void count_sort(It begin, const It end)
{
    const unsigned int max = *std::max_element(begin, end);
    std::vector<unsigned int> freq;
    freq.resize(max + 1);

    auto it = begin();
    while(it != end)
    {
        ++freq[*it];
        ++it;
    }

    unsigned int idx = 0;
    for(unsigned int i = 0; i <= max; ++i)
    {
        unsigned int count = freq[i];
        while(count != 0)
        {
            begin[idx] = i;
            --count;
            ++idx;
        }
    }
}

template<typename It>
void count_sort_str(It begin, const It end)
{
    auto it = begin;
    std::array<unsigned int, 128> freq = {};

    while(it != end)
    {
        ++freq[*it];
        ++it;
    }

    char* dest = begin.base();
    for(unsigned int i = 0; i < 128; ++i)
    {
        std::memset(dest, static_cast<char>(i), freq[i]);
        dest += freq[i];
    }
}

int main()
{
    std::string line;
    while(std::getline(std::cin, line))
    {
        const auto args = split(line);
        for(unsigned n = 10; n <= 100000000; n *= 10)
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            if(args[0] == "int")
            {
                const int minv = std::stoi(args[2]);
                const int maxv = std::stoi(args[3]);
                const std::vector<int> vec = get_vector<int>(n, args[1], minv, maxv, gen);
            }
            else if(args[0] == "unsigned")
            {
                const unsigned minv = std::stoul(args[2]);
                const unsigned maxv = std::stoul(args[3]);
                const std::vector<unsigned> vec =
                    get_vector<unsigned>(n, args[1], minv, maxv, gen);
            }
            else if(args[0] == "double")
            {
                const double minv = std::stod(args[2]);
                const double maxv = std::stod(args[3]);
                const std::vector<double> vec =
                    get_vector<double>(n, args[1], minv, maxv, gen);
            }
            else if(args[0] == "string")
            {
                const int minv = std::stoi(args[2]);
                const int maxv = std::stoi(args[3]);
                const std::vector<std::string> vec =
                    get_vector<std::string>(n, args[1], minv, maxv, gen);
            }
        }
    }
}
