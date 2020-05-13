
#pragma once
#include <utility>
#include <cassert>

// credit goes entirely to Mooing Duck <https://stackoverflow.com/users/845092/mooing-duck>

namespace rr_tuple{
//index a tuple by runtime parameter
template<size_t try_idx, size_t max, class R, class F, class T>
struct visit_runtime_impl {
    R operator()(T&& tuple, size_t idx, F&& function) {
        if (idx == try_idx) return function(std::get<try_idx>(tuple));
        return visit_runtime_impl<try_idx+1, max, R, F, T>{}(tuple, idx, std::forward<F&&>(function));
    }
};
template <size_t max, class R, class F, class T>
struct visit_runtime_impl<max, max, R, F, T> {
    R operator()(T&& tuple, size_t idx, F&& function) {assert(false);}
};
template<class R, class F, class...Ts>
R visit_runtime(std::tuple<Ts...>& tuple, size_t idx, F&& function) 
{return visit_runtime_impl<0, sizeof...(Ts), R, F, std::tuple<Ts...>&>{}(tuple, idx, std::forward<F&&>(function));}
template<class R, class F, class...Ts>
R visit_runtime(const std::tuple<Ts...>& tuple, size_t idx, F&& function) 
{return visit_runtime_impl<0, sizeof...(Ts), R, F, const std::tuple<Ts...>&>{}(tuple, idx, std::forward<F&&>(function));}

struct pair_first_deref {
    template<class Iter>
    auto operator()(std::pair<Iter, Iter>& pair) {return *pair.first;}
    template<class Iter>
    auto operator()(const std::pair<Iter, Iter>& pair) {return *pair.first;}
};
struct pair_first_increment {
    template<class Iter>
    void operator()(std::pair<Iter, Iter>& pair) {++pair.first;}
};
struct pair_its_equal {
    template<class Iter>
    bool operator()(const std::pair<Iter, Iter>& pair) {return pair.first == pair.second;}
};

template<class...Its>
class round_robin_iterator {
    static const size_t pair_count = sizeof...(Its);
    std::tuple<std::pair<Its, Its>...> iter_pairs_;
    int pair_idx_;
public:
    typedef std::ptrdiff_t difference_type;
    typedef std::common_type_t<typename std::iterator_traits<Its>::value_type...> value_type;
    typedef value_type reference;
    typedef value_type* pointer;
    typedef std::output_iterator_tag iterator_category;
    round_robin_iterator(std::pair<Its, Its>...iter_pairs) : iter_pairs_(iter_pairs...), pair_idx_(0) {}
    reference operator*() const { return visit_runtime<reference>(iter_pairs_, pair_idx_, pair_first_deref{}); }
    round_robin_iterator& operator++() {
        visit_runtime<void>(iter_pairs_, pair_idx_, pair_first_increment{});
        pair_idx_ = (pair_idx_+1)%pair_count; 
        size_t inc_count=0;
        while(++inc_count<pair_count && visit_runtime<bool>(iter_pairs_, pair_idx_, pair_its_equal{})) 
            pair_idx_ = (pair_idx_+1)%pair_count; 
        if (inc_count==pair_count) pair_idx_=0;
        return *this;
    }
    bool operator==(const round_robin_iterator& rhs) const { return iter_pairs_==rhs.iter_pairs_ && pair_idx_==rhs.pair_idx_; }
    bool operator!=(const round_robin_iterator& rhs) const { return iter_pairs_!=rhs.iter_pairs_ || pair_idx_!=rhs.pair_idx_; }
};
template<class...Ranges>
auto round_robin_first_it(const Ranges&...ranges) {return round_robin_iterator<decltype(std::end(ranges))...>{std::make_pair(std::begin(ranges), std::end(ranges))...};}
template<class...Ranges>
auto round_robin_last_it(const Ranges&...ranges) {return round_robin_iterator<decltype(std::end(ranges))...>{std::make_pair(std::end(ranges), std::end(ranges))...};}
}

