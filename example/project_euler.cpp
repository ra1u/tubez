#include "../src/tubez.hpp"
#include "main_common.hpp"

// https://projecteuler.net/problem=1
// Multiples of 3 and 5
//
// If we list all the natural numbers below 10 that are multiples of 3 or 5, we
// get 3, 5, 6 and 9. The sum of these multiples is 23.
// Find the sum of all the multiples of 3 or 5 below 1000.


// use N = 1000 to solve 
auto pe1(uint64_t N) {
  auto t = tubez::iter(uint64_t(0), [](auto a) { return a + 1; }) |
           tubez::filter([](auto n) { return (n % 3 == 0) || (n % 5 == 0); }) |
           tubez::take_while([N](auto n) { return n < N; }) | 
           tubez::sum<uint64_t>();
  return run_push(t);
}

size_t pe1_naive(size_t N) {
  uint64_t r = 0;
  for (uint64_t i = 0; i < N; ++i) {
    if ((i % 3 == 0) || (i % 5 == 0)) {
      r += i;
    }
  }
  return r;
}

int main(int argc, char **argv) {
  return main_perf_helper(argc, argv, pe1, pe1_naive);
}
