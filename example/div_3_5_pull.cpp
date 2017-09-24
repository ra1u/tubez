#include "../src/tubez.hpp"
#include "main_common.hpp"

// return sum of first N integers >= 1 that are divisible by 3 or 5

using num_t = uint32_t;

num_t div_3_5(num_t n) {
  auto t =
      tubez::iter(num_t(1), [](num_t a) { return a + 1; }) |
      tubez::filter([](auto x) { return (x % 3 == 0) || (x % 5 == 0); }) |
      tubez::map([](auto a) { return a; }) | tubez::take(n) |
      tubez::reducer_pure(num_t(0), [](auto a, auto b) { return a + b; });
  return run_push(t);
}

num_t div_3_5_naive(num_t N) {
  int r = 0, i = 1, c = 0;
  while (c < N) {
    if ((i % 3 == 0) || (i % 5 == 0)) {
      r += i;
      ++c;
    }
    ++i;
  }
  return r;
}

int main(int argc, char **argv) {
  return main_perf_helper(argc, argv, div_3_5, div_3_5_naive);
}
