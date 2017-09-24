#pragma once

#include "stream.hpp"

namespace tubez {

using Void = detail::Void;
using Unit = detail::Unit;

//////// source
template <typename T, typename Fpush, typename Fpull>
static auto source_generic(T &&t, Fpush &&fpush, Fpull &&fpull) {
  return make_source(detail::sourceGenC<std::remove_reference_t<T>,
                                        std::remove_reference_t<Fpush>,
                                        std::remove_reference_t<Fpull>>(
      std::forward<T>(t), std::forward<Fpush>(fpush),
      std::forward<Fpull>(fpull)));
}

///
/// push function is made from pull function
template <typename T, typename Fpull>
static auto source_generic_pull(T &&t, Fpull &&fpull) {
  auto fpush = [fpull](auto *state, auto *drain) -> StreamStatus {
    auto val = fpull(state);
    if (val) {
      return drain->push(*val);
    } else {
      return StreamStatus::terminated;
    }
  };
  return source_generic(t, fpush, fpull);
}

// takes state type T and function T(T) that is repeatedly called with previous
// state
// that is consumed
template <typename T, typename F> static auto iter(T &&state, F &&fun) {
  auto fpush =
      [fun](std::remove_reference_t<T> *s, auto *drain) -> StreamStatus {
    StreamStatus r = drain->push(*s);
    if (r == StreamStatus::open) {
      *s = fun(*s);
    }
    return r;
  };
  auto fpull = [fun](std::remove_reference_t<T> *state) {
    auto r = *state;
    *state = fun(*state);
    return boost::optional<std::remove_reference_t<T>>(r);
  };
  return source_generic(std::forward<T>(state), fpush, fpull);
}

/// returns source
/// 2 arguments that are `begin` and `end` from where data is generated
///
/// example
/// std::vector<int> v({1,2,3});
/// auto src = from_range(v.cbegin(),v.cend());
template <typename R> static auto from_range(R begin, R end) {
  auto fpush = [](auto *rng, auto *drain) -> StreamStatus {
    if (rng->first == rng->second) {
      return StreamStatus::terminated;
    }
    StreamStatus r = drain->push(*rng->first);
    ++rng->first;
    return r;
  };

  auto fpull = [](auto *rng) {
    using Rt = boost::optional<decltype(*rng->first)>;
    if (rng->first == rng->second) {
      return Rt();
    }
    auto iter = rng->first;
    ++rng->first;
    return Rt(*iter);
  };

  return source_generic(std::make_pair(begin, end), fpush, fpull);
}

/// return source
/// 1. argument starting number
/// 2. argument step (default value is 1)
template <typename T> static auto count(T from, T step = T(1)) {
  return iter(from, [step](T a) { return a + step; });
}

// namespace end
}
