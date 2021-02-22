#include "helper.h"
#include <iostream>

template<typename It>
void count_sort(It begin, const It end)
{
    const unsigned int max = *std::max_element(begin, end);
    if(max > (exp(10, 9) / 2))
    {
        std::cout << "Maximum value too big for CountSort\n";
        return;
    }
    std::vector<unsigned int> freq;
    freq.resize(max + 1);

    auto it = begin;
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
void bubble_sort(It begin, const It end)
{
    const unsigned size = std::distance(begin, end);

    for(unsigned i = 0; i < size - 1; ++i)
    {
        for(unsigned j = 0; j < size - i - 1; ++j)
        {
            if(begin[j] > begin[j + 1])
            {
                std::swap(begin[j], begin[j+1]);
            }
        }
    }
}

template<typename It>
void merge_sort(It begin, It end)
{
}

template<typename It>
void quick_sort(It begin, It end)
{
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

template<typename T>
void (*unsigned_sort_methods[5])(typename T::iterator, typename T::iterator) = {count_sort,
                                                                                std::sort};

int main()
{
    int k = 1;
    for(unsigned i = 2; i <= exp(2, 29); i *= 2)
    {
        std::cerr << k++ << '\n';
        std::random_device rd;
        std::mt19937 gen(rd());

        std::vector<unsigned> vec = random<unsigned>(i, 0u, i, gen);

        auto p = std::chrono::high_resolution_clock::now();
        bubble_sort(vec.begin(), vec.end());
        std::cout << get_timepoint_count(p) << '\n';
    }
}
