#include "../src/tubez.hpp"
#include "main_common.hpp"
#include <vector>

// append at the end of vector v numbers 
// and return vector
template <typename T> auto vector_test(T &&v) {
  // make vector with numbers from 0 to 1000 in step 2 {0,2,4 .. 1000}
  auto t = tubez::iter(0, [](auto a) { return a + 1; }) | // {1,2,3,4 ..... }
           tubez::map([](auto a) { return a * 2; }) |     // multiply by 2
           tubez::take_while([](auto a) { return a <= 1000; }) | //
           tubez::push_back(std::forward<T>(v));
  return tubez::run_push(t); // return value is void, do not peek it
}

// append at the end of vector v numbers as explained passed as reference
// and return (void)
template <typename T> auto vector_test_ref(T v) {
  // make vector with numbers from 0 to 1000 in step 2 {0,2,4 .. 1000}
  auto t = tubez::iter(0, [](auto a) { return a + 1; }) | // {1,2,3,4 ..... }
           tubez::map([](auto a) { return a * 2; }) |     // multiply by 2
           tubez::take_while([](auto a) { return a <= 1000; }) | //
           tubez::push_back_ref(std::forward<T>(v));
  return tubez::run_push(t); // return value is void, do not peek it
}


template <typename T> auto vector_sum_ref(const T &v) {
  typename T::value_type zero(0); // zero of type T
  auto t =
      tubez::from_range(v.cbegin(), v.cend()) | 
      tubez::reducer_pure(zero, [](auto a, auto b) -> auto { return a + b; }); // sum
  return tubez::run_push(t); // return value is void, do not peek it
}

int main() {
  auto v1 = std::vector<uint64_t>();
  int r = 0;
  // auto v2 = std::vector<int>();
  for (int i = 0; i < 1000000; ++i) {
    vector_test_ref(std::ref(v1));
    r += vector_sum_ref(v1);
    v1.clear();
  }
  std::cout << r << "\n";

  return 0;
}
