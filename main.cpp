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
            temp.push_back(std::move(*left));
            ++left;
        }
        else
        {
            temp.push_back(std::move(*right));
            ++right;
        }
    }

    if(left != mid)
    {
        temp.insert(temp.end(), std::make_move_iterator(left), std::make_move_iterator(mid));
    }
    if(right != end)
    {
        temp.insert(temp.end(), std::make_move_iterator(right), std::make_move_iterator(end));
    }

    std::move(temp.begin(), temp.end(), begin);
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
auto partition(It begin, const It end)
{
    const auto pivot = *(end - 1);
    auto i = begin;
    for(auto j = begin; j != (end - 1); ++j)
    {
        if(*j <= pivot)
        {
            std::swap(*j, *i);
            ++i;
        }
    }
    std::swap(*i, *(end - 1));
    return i;
}

template<typename It, typename Gen>
auto random_partition(It begin, It end, Gen& gen)
{
    const u64 pivotidx = std::uniform_int_distribution<u64>(0, (end - begin) - 1)(gen);
    std::swap(*(end - 1), *(begin + pivotidx));
    return partition(begin, end);
}

template<typename It, typename Gen>
void quick_sort_helper(It begin, It end, Gen& gen)
{
    if((end - begin) > 1)
    {
        auto pivot = random_partition(begin, end, gen);
        quick_sort_helper(begin, pivot, gen);
        quick_sort_helper(pivot + 1, end, gen);
    }
}

template<typename It>
void quick_sort(It begin, It end)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    quick_sort_helper(begin, end, gen);
}

template<typename It>
void quick_sort_last(It begin, It end)
{
    if((end - begin) > 1)
    {
        auto pivot = partition(begin, end);
        quick_sort_last(begin, pivot);
        quick_sort_last(pivot + 1, end);
    }
}

template<typename Container, typename = void>
struct SortMethods
{
    using It = typename Container::iterator;
    static constexpr void (*list[])(It, It) = {bubble_sort, merge_sort, quick_sort,
                                               quick_sort_last, std::sort};

    static constexpr const char* namelist[] = {
        "bubble_sort", "merge_sort", "quick_sort (random)", "quick_sort (last)", "std::sort"};
};

template<typename Container>
struct SortMethods<Container,
                   std::enable_if_t<std::is_unsigned_v<typename Container::value_type>>>
{
    using It = typename Container::iterator;
    static constexpr void (*list[])(It, It) = {count_sort, bubble_sort,     merge_sort,
                                               quick_sort, quick_sort_last, std::sort};

    static constexpr const char* namelist[] = {"count_sort",        "bubble_sort",
                                               "merge_sort",        "quick_sort (random)",
                                               "quick_sort (last)", "std::sort"};
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

template<typename Vec, typename Method>
u64 calculate_elapsed(const Vec& in_vec, Vec& out_vec, Method method)
{
    std::vector<u64> elapsed;
    u64 elapsed_total = 0;
    bool needs_check = true;

    while(elapsed_total < exp(10, 7))
    {
        out_vec = in_vec;

        auto tp = std::chrono::high_resolution_clock::now();
        method(out_vec.begin(), out_vec.end());
        u64 duration = get_timepoint_count(tp);

        if(needs_check && !std::is_sorted(out_vec.cbegin(), out_vec.cend()))
        {
            return U64MAX;
        }
        else
        {
            needs_check = false;
        }

        elapsed.push_back(duration);
        elapsed_total += duration;
    }

    return (std::accumulate(elapsed.cbegin(), elapsed.cend(), 0ull) / elapsed.size());
}

template<typename T, typename GeneratorFunction>
void benchmark_sort_methods(const std::string& inputtype, int plotpos, GeneratorFunction pred,
                            auto&&... generator_args)
{
    using VectorType = std::vector<T>;

    const auto& method_list = SortMethods<VectorType>::list;
    const auto& method_names = SortMethods<VectorType>::namelist;
    constexpr auto n_methods = std::size(method_list);
    std::array<bool, n_methods> reached_limit;
    std::memset(reached_limit.data(), false, n_methods);

    std::vector<std::vector<u64>> time_list;
    time_list.resize(n_methods);

    unsigned currentsize = 1;
    for(u64 i = 2; i <= exp(2, 15); i *= 2)
    {
        if(std::find(reached_limit.cbegin(), reached_limit.cend(), false) ==
           reached_limit.cend())
        {
            break;
        }
        std::cerr << "Genereaza vector de marime: 2^" << currentsize++ << '\n';

        std::random_device rd;
        std::mt19937 gen(rd());
        const std::vector<T> to_sort = pred(i, 0u, i, gen, generator_args...);

        for(unsigned m_idx = 0; m_idx < n_methods; ++m_idx)
        {
            if(reached_limit[m_idx] == true)
            {
                continue;
            }
            std::cerr << "Sorteaza prin metoda: " << method_names[m_idx] << "..." << '\n';

            std::vector<T> vec;
            u64 elapsed = calculate_elapsed(to_sort, vec, method_list[m_idx]);

            if(elapsed != U64MAX)
            {
                time_list[m_idx].push_back(elapsed);
            }
            else
            {
                std::cerr << "hit this\n";
                reached_limit[m_idx] = true;
                std::cerr << "Nu s-a putut sorta din motivul de mai sus\n";
                continue;
            }

            if(elapsed > (exp(10, 10) / 3))
            {
                reached_limit[m_idx] = true;
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
