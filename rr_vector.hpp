
#pragma once


namespace rr_vector{

  template<class Iter>
  bool iter_done(const std::pair<Iter, Iter>& p) { return p.first == p.second; }

  template<class Iter, template<class> class Storage, bool erase_dead>
  class round_robin_iterator: public std::iterator_traits<Iter>
  {
  public:
    using IterVec = Storage<std::pair<Iter, Iter>>;
    using reference = typename std::iterator_traits<Iter>::reference;
  protected:
    IterVec others;
    size_t current_index = 0;

  public:
    bool is_end() const {return others.empty(); }

    void add_iters(const Iter& _begin, const Iter& _end) {
      others.emplace_back(_begin, _end);
    }

    reference operator*() { return *(others.at(current_index).first); }

    bool operator!=(const round_robin_iterator& rr_it) const {
      if(!is_end()){
        if(!rr_it.is_end()){
          return current_index != rr_it.current_index;
        } else return true;
      } else return !rr_it.is_end();
    }

    round_robin_iterator& operator++() {
      if(!is_end()) {
        auto& it = others.at(current_index);
        if(++it.first == it.second){
          others.erase(others.begin() + current_index);
        } else ++current_index;
        if(!is_end()) current_index %= others.size();
      }
      return *this;
    }

  };

  template<class Iter, template<class> class Storage>
  class round_robin_iterator<Iter, Storage, false>: public round_robin_iterator<Iter, Storage, true>
  {
    using Parent = round_robin_iterator<Iter, Storage, true>;
  protected:
    using Parent::others;
    using Parent::current_index;
    size_t live_iters = 0;
    
    inline void inc_index() { ++current_index; current_index %= others.size(); }
  public:
    using Parent::Parent;

    bool is_end() const { return !live_iters; }
    
    void add_iters(const Iter& _begin, const Iter& _end) {
      Parent::add_iters(_begin, _end);
      ++live_iters;
    }

    round_robin_iterator& operator++() {
      if(!is_end()){
        auto& it = others.at(current_index);
        if(++it.first == it.second) --live_iters;
        if(live_iters)
          do inc_index(); while(iter_done(others.at(current_index)));
      }
      return *this;
    }

    bool operator!=(const round_robin_iterator& rr_it) const {
      if(!is_end()){
        if(!rr_it.is_end()){
          return current_index != rr_it.current_index;
        } else return true;
      } else return !rr_it.is_end();
    }

  };


}
