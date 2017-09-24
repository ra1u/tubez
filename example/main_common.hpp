
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <utility>
#include <chrono>

template <typename T, typename U>
static inline auto time_result(T &&f, U &&in) {
  auto tic = std::chrono::system_clock::now();
  auto r = f(in);
  auto toc = std::chrono::system_clock::now();
  std::chrono::duration<double> diff = toc - tic;
  return std::make_pair(r, diff.count());
};



template <typename T, typename U>
auto main_perf_helper(int argc, char **argv, T &&tube_f, U &&naive_f) -> int {
  uint64_t N = 10;
  if (argc == 2) {
    N = boost::lexical_cast<uint64_t>(argv[1]);
  }

  auto rn = time_result(std::forward<U>(naive_f), N);
  std::cout << "naive func " << rn.first << " time " << rn.second << "\n";
  std::cout << "***\n";
  auto r = time_result(std::forward<T>(tube_f), N);
  std::cout << "tube func  " << r.first << " time " << r.second << "\n";

  return 0;
};
