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
        bool swapped = false;
        for(unsigned j = 0; j < size - i - 1; ++j)
        {
            if(begin[j] > begin[j + 1])
            {
                std::swap(begin[j], begin[j + 1]);
                swapped = true;
            }
        }
        if(!swapped)
        {
            break;
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

template<typename T>
void (*unsigned_sort_methods[3])(typename T::iterator,
                                 typename T::iterator) = {count_sort, bubble_sort, std::sort};

template<typename T, typename GeneratorFunction>
void benchmark_method(const std::string& inputtype, int plotpos, GeneratorFunction pred,
                      auto&&... generator_args)
{
    std::vector<std::vector<u64>> timpi;
    int k = 0;
    for(auto metoda : unsigned_sort_methods<std::vector<unsigned>>)
    {
        std::cerr << "Sorting with method: " << k++ << "..." << '\n';
        timpi.push_back({});
        int ptwo = 1;
        for(u64 i = 2; i <= exp(2, 27); i *= 2)
        {
            std::cerr << "Marimea vectorului: 2^" << ptwo++ << '\n';
            std::random_device rd;
            std::mt19937 gen(rd());

            std::vector<T> vec = pred(i, 0u, i, gen, generator_args...);

            auto p = std::chrono::high_resolution_clock::now();
            metoda(vec.begin(), vec.end());
            u64 elapsed = get_timepoint_count(p);

            if(std::is_sorted(vec.begin(), vec.end()))
            {
                timpi.back().push_back(elapsed);
            }
            else
            {
                std::cerr << "eroare\n";
                return;
            }

            if(elapsed > (2 * exp(10, 10)))
            {
                break;
            }
        }
    }

    std::cout << subplot_pos(plotpos) << '\n';
    std::cout << predefined;
    std::cout << subplot_title(inputtype) << '\n';
    std::cout << plot_command(timpi) << '\n';
    std::cout << legends({"count_sort", "bubble_sort", "std::sort"}) << '\n';
}

int main()
{
    benchmark_method<unsigned>("random",
                               1,
                               random<unsigned, unsigned, std::mt19937>);

    benchmark_method<unsigned>("almost sorted",
                               2,
                               almost_sorted<unsigned, unsigned, std::mt19937, std::less<unsigned>>,
                               std::less<unsigned>{});

    benchmark_method<unsigned>("almost sorted (decreasing)",
                               3,
                               almost_sorted<unsigned, unsigned, std::mt19937, std::greater<unsigned>>,
                               std::greater<unsigned>{});

    benchmark_method<unsigned>("sorted",
                               4,
                               sorted<unsigned, unsigned, std::mt19937>,
                               std::less<unsigned>{});
}
