#include "main_common.hpp"
#include "../src/tubez.hpp"


auto sum_digits(uint64_t N) {
  auto t =
      tubez::iter(uint64_t(0), [](auto a) { return a + 1; }) |
      tubez::take(N) |
      tubez::reducer_pure(uint64_t(0), [](auto a, auto b) { return a + b; });
  return run_pull(t);
}


uint64_t sum_digits_naive(uint64_t N) {
  uint64_t r = 0;
  for (uint64_t i = 0; i < N; ++i) {
    r += i;
  }
  return r;
}


int main(int argc, char **argv) {
  return main_perf_helper(argc, argv, sum_digits, sum_digits_naive);
}
