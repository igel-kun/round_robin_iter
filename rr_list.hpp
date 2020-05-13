
#pragma once

#include <list>

namespace rr_list{

  template<class Iter>
  class round_robin_iterator: public std::iterator_traits<Iter>
  {
  public:
    using IterList = std::list<std::pair<Iter, Iter>>;
    using typename std::iterator_traits<Iter>::reference;
  protected:
    IterList others;
    typename IterList::iterator current = others.end();

    round_robin_iterator(const round_robin_iterator&) = delete; // no copy!
    round_robin_iterator& operator=(const round_robin_iterator&) = delete; // no copy!
  public:
    round_robin_iterator() = default;
    round_robin_iterator(round_robin_iterator&&) = default;
    round_robin_iterator& operator=(round_robin_iterator&&) = default;
    
    bool is_end() const { return others.empty(); }

    round_robin_iterator& operator++() {
      if(!is_end()) {
        if(++(current->first) == current->second)
          current = others.erase(current);
        else ++current;
        if(current == others.end()) current = others.begin();
      }
      return *this;
    }

    reference operator*() { return *(current->first); }

    void add_iters(const Iter& _begin, const Iter& _end) {
      others.emplace_back(_begin, _end);
      if(others.size() == 1) current = others.begin();
    }

    bool operator!=(const round_robin_iterator& rr_it) const {
      if(!is_end()){
        if(!rr_it.is_end()){
          return current != rr_it.current;
        } else return true;
      } else return !rr_it.is_end();
    }
  };
}
