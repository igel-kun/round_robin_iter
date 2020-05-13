
#include <vector>
#include <deque>
#include <iostream>
#include <chrono>
#include <numeric>
#include <map>
#include "rr_tuple.hpp"
#include "rr_list.hpp"
#include "rr_vector.hpp"

template<int depth = 4>
struct blob
{
  blob<depth-1> a,b,c,d,e;
  int x;
  //blob(const size_t x): a(1), b(2), d(3), e(4), f(5), g(6), h(7), i(8), j(9), k(0), l(1), m(2) {}
  blob(const size_t _x): x(_x) {}
  blob() {}
};
template<>
struct blob<0> { int x; };

size_t operator+(const size_t x, const blob<>& y) { return x + y.x; }


template<class Iter, class T>
void bench(const std::vector<std::vector<T>>& data)
{
  Iter _begin, _end;
  for(const auto& vec: data)
    _begin.add_iters(vec.begin(), vec.end());

  const auto start = std::chrono::system_clock::now();
  const size_t result = std::accumulate(std::move(_begin), std::move(_end), (size_t)0ul);
  const auto elapsed = std::chrono::system_clock::now() - start;
  std::cerr << "ignore this: "<<result<<'\n';
  std::cout << '\t' << elapsed.count() << '\n';
}

template<class T>
void bench_all(const std::vector<std::vector<T>>& containers)
{
  using vec_iter = typename std::vector<T>::const_iterator;
  std::cout << "list-based (erase): ";
  bench<rr_list::round_robin_iterator<vec_iter>>(containers);
  std::cout << "vector-based (erase): ";
  bench<rr_vector::round_robin_iterator<vec_iter, std::vector, true>>(containers);
  std::cout << "deque-based (erase): ";
  bench<rr_vector::round_robin_iterator<vec_iter, std::deque, true>>(containers);
  std::cout << "vector-based (skip): ";
  bench<rr_vector::round_robin_iterator<vec_iter, std::vector, false>>(containers);
  std::cout << "deque-based (skip): ";
  bench<rr_vector::round_robin_iterator<vec_iter, std::deque, false>>(containers);

}

template<class T>
void bench(const size_t num_containers, const size_t small_cont, const size_t large_cont, const size_t init)
{
  std::cout << "1 large vector ("<<large_cont<<" elements) + "<<num_containers - 1<<" small vectors ("<<small_cont<<" elements):\n";
  std::vector<std::vector<T>> containers(num_containers);
  containers[0].reserve(large_cont);
  for(size_t j = 0; j != large_cont; ++j) containers[0].emplace_back(init);
  for(size_t i = 1; i != num_containers; ++i)
    for(size_t j = 0; j != small_cont; ++j) containers[i].emplace_back(init);
  bench_all(containers);

  std::cout <<"\n" << num_containers << " progressivly larger vectors (from "<<small_cont<<" elements to "<<large_cont<<" elements)\n";
  const double diff_per_vec = double(large_cont - small_cont) / (num_containers - 1);
  for(size_t i = 0; i != num_containers; ++i){
    const size_t size = small_cont + diff_per_vec * i;
    containers[i].clear();
    containers[i].reserve(size);
    for(size_t j = 0; j != size; ++j) containers[i].emplace_back(init);
  }
  bench_all(containers);

}


int main(int argc, char** argv)
{
  size_t num_containers, small_cont, large_cont;
  if((argc >= 4) && (num_containers = std::stol(argv[1])) &&
      (small_cont = std::stol(argv[2])) && (large_cont = std::stol(argv[3]))) {
    std::cout << "benchmarking int-containers\n";
    bench<int>(num_containers, small_cont, large_cont, 111);
    std::cout << "\n\n benchmarking containers for size-"<<sizeof(blob<>)<<" objects\n";
    bench<blob<>>(num_containers, small_cont, large_cont, 111);
  } else std::cout << "usage: "<<argv[0]<<" <#containers> <small container> <large container> <large element>\n";
}
