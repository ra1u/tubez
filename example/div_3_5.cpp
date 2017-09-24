#include "../src/tubez.hpp"
#include "main_common.hpp"

// return sum of first N integers >= 1 that are divisible by 3 or 5

uint64_t div_3_5(uint64_t n) {
  auto t = tubez::iter(uint64_t(1), [](uint64_t a) { return a + 1; }) |
           tubez::filter([](auto x) { 
             return (x % 3 == 0) || (x % 5 == 0); 
           }) |
           tubez::take(n) | tubez::sum<uint64_t>();
  return run_push(t);
}

uint64_t div_3_5_naive(uint64_t N) {
  uint64_t r = 0, i = 1, c = 0;
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
