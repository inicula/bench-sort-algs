#include "helper.h"

constexpr const char* usage = "Usage: sh makepyplot.sh (unsigned | string | double) 2^<n'th power> [sort method names]";
constexpr const char* usage_avail= "sh makeplot.sh pentru a afisa algoritmii disponibili";
constexpr u64 callstack_limit = 1ull << 16ull;
bool errflag = false;
std::string why;

template<typename It>
void count_sort(It begin, const It end)
{
    using T = typename std::iterator_traits<It>::value_type;

    const u64 size = end - begin;
    if(size <= 1)
    {
        return;
    }

    const auto max = *std::max_element(begin, end);
    const u64 bytes_needed = size * sizeof(T) + (static_cast<u64>(max) + 1ull) * sizeof(T);
    if(bytes_needed >= (4ull * powu64(10, 9)))
    {
        errflag = true;
        why = "Input-ul necesita depasirea limitei de memorie.";
        return;
    }

    std::vector<T> freq;
    freq.resize(max + 1);

    It it = begin;
    while(it != end)
    {
        ++freq[*it];
        ++it;
    }

    u64 idx = 0;
    for(T i = 0; i <= max; ++i)
    {
        std::size_t count = freq[i];
        while(count != 0)
        {
            begin[idx] = i;
            --count;
            ++idx;
        }
    }
}

template<typename It>
__always_inline It radix_partition(It begin, const It end, const int bitpos)
{
    using T = typename std::iterator_traits<It>::value_type;

    It l_begin = begin;
    while(begin != end)
    {
        if(!((*begin) & (static_cast<T>(1) << bitpos)))
        {
            std::swap(*l_begin, *begin);
            ++l_begin;
        }
        ++begin;
    }
    return l_begin;
}

template<typename It>
void radix_helper(It begin, const It end, const int bitpos)
{
    if(bitpos < 0)
    {
        return;
    }
    if((end - begin) > 1)
    {
        It part_it = radix_partition(begin, end, bitpos);
        radix_helper(begin, part_it, bitpos - 1);
        radix_helper(part_it, end, bitpos - 1);
    }
}

template<typename It>
void radix_sort(It begin, It end)
{
    if((end - begin) > 1)
    {
        radix_helper(begin, end, max_bit_pos(*std::max_element(begin, end)));
    }
}

template<typename It>
void bubble_sort(It begin, const It end)
{
    const std::size_t size = end - begin;

    for(std::size_t i = 0; i < size - 1; ++i)
    {
        bool swapped = false;
        for(std::size_t j = 0; j < size - i - 1; ++j)
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
__always_inline void merge(const It begin, const It mid, const It end)
{
    using T = typename std::iterator_traits<It>::value_type;

    std::vector<T> temp;
    temp.reserve(end - begin);

    It left = begin;
    It right = mid;

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

    It mid = begin + size / 2;
    merge_sort(begin, mid);
    merge_sort(mid, end);

    merge(begin, mid, end);
}

template<typename It>
__always_inline It partition(It begin, const It end)
{
    const auto pivot = *(end - 1);
    It i = begin;
    for(It j = begin; j != (end - 1); ++j)
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
__always_inline It random_partition(It begin, It end, Gen& gen)
{
    const u64 pivotidx = std::uniform_int_distribution<u64>(0, (end - begin) - 1)(gen);
    std::swap(*(end - 1), *(begin + pivotidx));
    return partition(begin, end);
}

template<typename It, typename Gen>
void quick_sort_helper(It begin, It end, Gen& gen, std::size_t& rcall)
{
    if(errflag)
    {
        return;
    }
    if(rcall >= callstack_limit)
    {
        errflag = true;
        why = "Input-ul necesita depasirea limitei call stack-ului.";
        return;
    }
    if((end - begin) > 1)
    {
        It pivot = random_partition(begin, end, gen);
        ++rcall;
        quick_sort_helper(begin, pivot, gen, rcall);
        --rcall;
        ++rcall;
        quick_sort_helper(pivot + 1, end, gen, rcall);
        --rcall;
    }
}

template<typename It>
void quick_sort(It begin, It end)
{
    static std::mt19937 gen(std::random_device{}());
    std::size_t rcall = 1;
    quick_sort_helper(begin, end, gen, rcall);
}

template<typename It>
void quick_sort_last_helper(It begin, It end, std::size_t& rcall)
{
    if(errflag)
    {
        return;
    }
    if(rcall >= callstack_limit)
    {
        errflag = true;
        why = "Input-ul necesita depasirea limitei call stack-ului.";
        return;
    }
    if((end - begin) > 1)
    {
        It pivot = partition(begin, end);
        ++rcall;
        quick_sort_last_helper(begin, pivot, rcall);
        --rcall;
        ++rcall;
        quick_sort_last_helper(pivot + 1, end, rcall);
        --rcall;
    }
}

template<typename It>
void quick_sort_last(It begin, It end)
{
    std::size_t rcall = 1;
    quick_sort_last_helper(begin, end, rcall);
}

template<typename Container>
using sort_func_t = void (*)(typename Container::iterator, typename Container::iterator);

template<typename Container, typename = void>
struct SortMethods
{
    using It = typename Container::iterator;
    static constexpr void (*list[])(It, It) = {
        bubble_sort,
        merge_sort,
        quick_sort,
        quick_sort_last,
        std::sort
    };
    static constexpr std::string_view namelist[] = {
        "bubble_sort",
        "merge_sort",
        "quick_sort_rand",
        "quick_sort_last",
        "std::sort"
    };
};

template<typename Container>
struct SortMethods<Container,
                   std::enable_if_t<std::is_unsigned_v<typename Container::value_type>>>
{
    using It = typename Container::iterator;
    static constexpr void (*list[])(It, It) = {
        count_sort,
        radix_sort,
        bubble_sort,
        merge_sort,
        quick_sort,
        quick_sort_last,
        std::sort
    };
    static constexpr std::string_view namelist[] = {
        "count_sort",
        "radix_sort",
        "bubble_sort",
        "merge_sort",
        "quick_sort_rand",
        "quick_sort_last",
        "std::sort"
    };
};

template<typename Container>
struct SortFunctions
{
    using func_t = sort_func_t<Container>;

    bool empty() const
    {
        return fptrs.empty() || names.empty();
    }

    std::vector<func_t> fptrs;
    std::vector<std::string_view> names;
};

template<typename T>
auto get_sort_methods(const int argc, const char* argv[])
{
    using VecType = std::vector<T>;

    const auto& list = SortMethods<VecType>::list;
    const auto& namelist = SortMethods<VecType>::namelist;

    SortFunctions<VecType> sf;

    if(argc == 3)
    {
        sf.fptrs = std::vector<typename SortFunctions<VecType>::func_t>(std::begin(list), std::end(list));
        sf.names = std::vector<std::string_view>(std::begin(namelist), std::end(namelist));
        return sf;
    }

    for(int i = 3; i < argc; ++i)
    {
        for(unsigned j = 0; j < std::size(namelist); ++j)
        {
            if(namelist[j] == argv[i])
            {
                sf.fptrs.push_back(list[j]);
                sf.names.push_back(namelist[j]);
                break;
            }
        }
    }

    return sf;
}

void legends(const auto& namelist)
{
    fmt::print("plt.legend([");
    for(auto l : namelist)
    {
        fmt::print("'{}', ", l);
    }
    fmt::print("])\n");
}

template<typename Vec, typename Method>
u64 calculate_elapsed(const Vec& in_vec, Method method)
{
    u64 elapsed_total = 0;
    u64 iterations = 0;
    bool needs_check = true;

    Vec out_vec;
    while(elapsed_total < powu64(10, 7))
    {
        out_vec = in_vec;

        auto tp = std::chrono::high_resolution_clock::now();
        method(out_vec.begin(), out_vec.end());
        u64 duration = get_timepoint_count(tp);

        if(errflag || (needs_check && !std::is_sorted(out_vec.cbegin(), out_vec.cend())))
        {
            return U64MAX;
        }
        else
        {
            needs_check = false;
        }

        elapsed_total += duration;
        ++iterations;
    }

    return elapsed_total / iterations;
}

template<typename T>
void benchmark_sort_methods(const u64 max_size, const auto& sort_funcs, const int plotpos, const auto& gmethod)
{
    const auto& method_list = sort_funcs.fptrs;
    const auto& method_names = sort_funcs.names;
    const auto n_methods = std::size(method_list);
    std::vector<bool> reached_limit(n_methods, false);

    std::vector<std::vector<u64>> time_list;
    time_list.resize(n_methods);

    unsigned currentsize = 1;
    for(u64 i = 2; i <= powu64(2, max_size); i *= 2)
    {
        if(std::find(reached_limit.cbegin(), reached_limit.cend(), false) ==
           reached_limit.cend())
        {
            break;
        }
        fmt::print(stderr, "Genereaza vector de marime: 2^{} [TIP: {}]\n", currentsize++,
                   gmethod.description);

        const std::vector<T> to_sort = [&]()
        {
            std::random_device rd;
            std::mt19937 gen(rd());

            if constexpr(std::is_same_v<T, std::string>)
            {
                const auto strsize = string_limit(i);
                return gmethod.ptr(strsize, 1ull, strsize, gen);
            }
            else
            {
                return gmethod.ptr(i, static_cast<T>(0), i, gen);
            }
        }();

        for(unsigned m_idx = 0; m_idx < n_methods; ++m_idx)
        {
            if(reached_limit[m_idx] == true)
            {
                continue;
            }
            fmt::print(stderr, "Sorteaza prin metoda: {} ... ", method_names[m_idx]);

            const u64 elapsed = calculate_elapsed(to_sort, method_list[m_idx]);

            if(elapsed != U64MAX)
            {
                fmt::print(stderr, "DONE!\n");
                time_list[m_idx].push_back(elapsed);
            }
            else
            {
                fmt::print(stderr,
                           "\nAlgoritmul nu a putut sorta input-ul din motivul urmator: {}\n",
                           why);

                reached_limit[m_idx] = true;
                errflag = false;
                why.clear();
                continue;
            }

            if(elapsed > (powu64(10, 10) / 4))
            {
                reached_limit[m_idx] = true;
            }
        }
    }

    subplot_pos(plotpos);
    fmt::print(predefined<T>);
    subplot_title(gmethod.description);
    plot_command(time_list);
    legends(method_names);
}

template<typename T, typename... GenTypes>
void benchmark_type(const int argc, const char* argv[], const u64 size)
{
    auto sort_methods = get_sort_methods<T>(argc, argv);
    if(sort_methods.empty())
    {
        fmt::print(stderr, "Vectorul de metode este gol\n{}\n{}\n", usage, usage_avail);
        std::exit(EXIT_FAILURE);
    }

    fmt::print("{}{}", pyheader, suptitle<T>);

    unsigned tmp = 0;
    for(const auto& gmethod : GeneratorMethods<T, GenTypes...>{})
    {
        benchmark_sort_methods<T>(size, sort_methods, ++tmp, gmethod);
    }
}

int main(int argc, const char* argv[])
{
    if(argc == 1)
    {
        fmt::print(stderr, "Algoritmi pentru valori unsigned\n");
        int k = 1;
        for(auto mname : SortMethods<std::vector<unsigned>>::namelist)
        {
            fmt::print(stderr, "{}. {}\n", k, mname);
            ++k;
        }

        fmt::print(stderr, "Algoritmi generici\n");
        k = 1;
        for(auto mname : SortMethods<std::vector<std::string>>::namelist)
        {
            fmt::print(stderr, "{}. {}\n", k, mname);
            ++k;
        }
    }

    if(argc < 3)
    {
        fmt::print(stderr, "{}\n", usage);
        return 1;
    }

    const u64 size = std::stoull(argv[2]);
    if(size < 1)
    {
        fmt::print(stderr, "Marimea vectorului e prea mica\n");
        return 1;
    }
    else if(size > 28)
    {
        fmt::print(stderr, "Marimea vectorului e prea mare\n");
        return 1;
    }

    const std::string_view arg = argv[1];
    if(arg == "unsigned")
    {
        benchmark_type<unsigned>(argc, argv, size);
    }
    else if(arg == "string" || arg == "std::string")
    {
        benchmark_type<std::string, u64>(argc, argv, size);
    }
    else if(arg == "double")
    {
        benchmark_type<double>(argc, argv, size);
    }
    else
    {
        fmt::print(stderr, "{}\n{}\n", usage, usage_avail);
        return 1;
    }

    fmt::print("plt.show()\n");
    return 0;
}
