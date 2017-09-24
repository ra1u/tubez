#include "../src/tubez.hpp"
#include "main_common.hpp"


auto sum_digits_tube(uint64_t N) {
  auto t = tubez::count(uint64_t(0)) |
           tubez::take_while([N](auto n) { return n < N; }) |
           tubez::sum<uint64_t>();
  return run_push(t);
}


uint64_t sum_digits_for(uint64_t N) {
  uint64_t r = 0;
  for (uint64_t i = 0; i < N; ++i) {
    r += i;
  }
  return r;
}


int main(int argc, char **argv) {
  return main_perf_helper(argc, argv, sum_digits_tube, sum_digits_for);
}
