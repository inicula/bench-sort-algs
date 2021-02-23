#include "helper.h"
#include <iostream>
#include <iterator>

template<typename It>
void count_sort(It begin, const It end)
{
    using T = typename std::iterator_traits<It>::value_type;

    const auto max = *std::max_element(begin, end);
    if(max > exp(2, 29))
    {
        std::cerr << "Maximum value too big for count_sort\n";
        return;
    }
    std::vector<T> freq;
    freq.resize(max + 1);

    auto it = begin;
    while(it != end)
    {
        ++freq[*it];
        ++it;
    }

    u64 idx = 0;
    for(T i = 0; i <= max; ++i)
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
void merge(const It begin, const It mid, const It end)
{
    using T = typename std::iterator_traits<It>::value_type;

    std::vector<T> temp;
    temp.reserve(end - begin);

    auto left = begin;
    auto right = mid;

    while(left != mid && right != end)
    {
        if(*left < *right)
        {
            if constexpr(std::is_arithmetic_v<T>)
            {
                temp.push_back(*left);
            }
            else
            {
                temp.push_back(std::move(*left));
            }
            ++left;
        }
        else
        {
            if constexpr(std::is_arithmetic_v<T>)
            {
                temp.push_back(*right);
            }
            else
            {
                temp.push_back(std::move(*right));
            }
            ++right;
        }
    }

    if(left != mid)
    {
        if constexpr(std::is_arithmetic_v<T>)
        {
            temp.insert(temp.end(), left, mid);
        }
        else
        {
            temp.insert(temp.end(), std::make_move_iterator(left),
                        std::make_move_iterator(mid));
        }
    }
    if(right != end)
    {
        if constexpr(std::is_arithmetic_v<T>)
        {
            temp.insert(temp.end(), right, end);
        }
        else
        {
            temp.insert(temp.end(), std::make_move_iterator(right),
                        std::make_move_iterator(end));
        }
    }

    if constexpr(std::is_arithmetic_v<T>)
    {
        std::copy(temp.cbegin(), temp.cend(), begin);
    }
    else
    {
        std::move(temp.begin(), temp.end(), begin);
    }
}

template<typename It>
void merge_sort(It begin, It end)
{
    const auto size = end - begin;

    if(size <= 1)
    {
        return;
    }

    auto mid = begin + size / 2;
    merge_sort(begin, mid);
    merge_sort(mid, end);

    merge(begin, mid, end);
}

template<typename It>
void quick_sort(It begin, It end)
{
}

template<typename Container, typename = void>
struct SortMethods
{
    static constexpr void (*list[])(typename Container::iterator,
                                    typename Container::iterator) = {bubble_sort, merge_sort,
                                                                     std::sort};
    static constexpr std::string_view namelist[] = {"bubble_sort", "merge_sort", "std::sort"};
};

template<typename Container>
struct SortMethods<Container,
                   std::enable_if_t<std::is_unsigned_v<typename Container::value_type>>>
{
    static constexpr void (*list[])(typename Container::iterator,
                                    typename Container::iterator) = {count_sort, bubble_sort,
                                                                     merge_sort, std::sort};
    static constexpr std::string_view namelist[] = {"count_sort", "bubble_sort", "merge_sort",
                                                    "std::sort"};
};

template<typename Container>
void legends()
{
    std::cout << "plt.legend([";
    for(auto l : SortMethods<Container>::namelist)
    {
        std::cout << "'" << l << "', ";
    }
    std::cout << "])\n";
}

template<typename T, typename GeneratorFunction>
void benchmark_sort_methods(const std::string& inputtype, int plotpos, GeneratorFunction pred,
                            auto&&... generator_args)
{
    using VectorType = std::vector<T>;
    const auto& method_list = SortMethods<VectorType>::list;
    const auto& method_names = SortMethods<VectorType>::namelist;

    std::vector<std::vector<u64>> time_list;
    unsigned k = 0;
    for(auto metoda : method_list)
    {
        std::cerr << "Sorteaza prin metoda: " << method_names[k++] << "..." << '\n';
        time_list.push_back({});
        int ptwo = 1;
        for(u64 i = 2; i <= exp(2, 10); i *= 2)
        {
            std::cerr << "Marimea vectorului: 2^" << ptwo++ << '\n';
            std::random_device rd;
            std::mt19937 gen(rd());

            std::vector<T> vec = pred(i, 0u, i, gen, generator_args...);

            auto p = std::chrono::high_resolution_clock::now();
            metoda(vec.begin(), vec.end());
            u64 elapsed = get_timepoint_count(p);

            if(std::is_sorted(vec.cbegin(), vec.cend()))
            {
                time_list.back().push_back(elapsed);
            }
            else
            {
                std::cerr << "Nu s-a putut sorta din motivul de mai sus\n";
            }

            if(elapsed > (1 * exp(10, 10)))
            {
                break;
            }
        }
    }

    subplot_pos(plotpos);
    std::cout << predefined;
    subplot_title(inputtype);
    plot_command(time_list);
    legends<VectorType>();
}

int main()
{
    benchmark_sort_methods<unsigned>("random", 1, random<unsigned>);

    benchmark_sort_methods<unsigned>("almost sorted", 2, almost_sorted<unsigned>,
                                     std::less<unsigned>{});

    benchmark_sort_methods<unsigned>("almost sorted (decreasing)", 3,
                                     almost_sorted<unsigned, std::greater<unsigned>>,
                                     std::greater<unsigned>{});

    benchmark_sort_methods<unsigned>("sorted", 4, sorted<unsigned>, std::less<unsigned>{});
}
