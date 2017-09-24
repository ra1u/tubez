#pragma once

#include "stream.hpp"

namespace tubez {

template <typename Ta, typename Fa, typename Ra>
static auto drain_generic(Ta &&state, Fa &&consume_fun, Ra &&result_transform) {
  return make_drain1(detail::drainGenC<std::remove_reference_t<Ta>,
                                       std::remove_reference_t<Fa>,
                                       std::remove_reference_t<Ra>>(
      std::forward<Ta>(state), std::forward<Fa>(consume_fun),
      std::forward<Ra>(result_transform)));
}

template <typename R, typename T> static auto reducer(T &&t, R r) {
  return drain_generic(std::forward<T>(t), [r](auto &a, auto b) {
                                             r(a, b);
                                             return StreamStatus::open;
                                           },
                       [](auto &&x) -> auto { return x; });
}

template <typename R, typename T> static auto reducer_pure(T &&t, R r) {
  return drain_generic(std::forward<T>(t), [r](auto &a, auto b) {
                                             a = r(a, b);
                                             return StreamStatus::open;
                                           },
                       [](auto &&x) -> auto { return x; });
}

template <typename T = int> static auto sum() {
  return tubez::reducer_pure(T(0), [](T a, T b) -> T { return a + b; });
}

template <typename R, typename T>
static auto reducer_ref(std::reference_wrapper<T> t, R r) {
  return drain_generic(t, [r](auto a, auto &&b) {
                            r(a, b);
                            return StreamStatus::open;
                          },
                       [](auto x) -> void {});
}

template <typename T> static auto push_back(T &&t) {
  return drain_generic(std::forward<T>(t), [](auto &a, auto b) {
                                             a.push_back(b);
                                             return StreamStatus::open;
                                           },
                       [](auto &&x) -> auto { return x; });
}

template <typename T> static auto push_back_ref(T &&t) {
  return reducer_ref(t, [](auto a, auto b) { a.get().push_back(b); });
}

// namespace end
}
