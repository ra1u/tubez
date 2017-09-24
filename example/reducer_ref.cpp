#include "../src/tubez.hpp"
#include "main_common.hpp"
#include <vector>

static auto sum() {
  return tubez::reducer(uint64_t(0), [](uint64_t &a, const uint64_t b) -> void
     { a += b; });
}


static auto push_back = [](auto s0){
   return tubez::reducer(s0, [](auto s,auto val){
      s.get().push_back(val);
   });
};


template <typename T>
static auto push_back_pure(T s0){
   return tubez::reducer_pure(s0, [](auto s,auto val){
       s.get().push_back(val);
       return s;
   });
}

auto reducer_tube(uint64_t N) {
  std::vector<uint64_t> v;
  auto t = tubez::iter(uint64_t(0), [](auto a) { return a + 1; }) |
           tubez::take_while([N](auto n) { return n < N; }) |
           push_back_pure(std::ref(v));
  return run_push(t).get().back();
}

uint64_t reducer_for(uint64_t N) {
  uint64_t r = 0;
  for (uint64_t i = 0; i < N; ++i) {
    r += i;
  }
  return r;
}

int main(int argc, char **argv) {
  return main_perf_helper(argc, argv, reducer_tube, reducer_for);
}
