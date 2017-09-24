#pragma once

#include <type_traits>

#include <boost/hana/concat.hpp>
#include <boost/hana/tuple.hpp>


namespace tubez {

namespace hana = boost::hana;

// utility

// check both argument same
template <typename T, typename U>
constexpr bool is_same_ref_v =
    std::is_same<std::remove_reference_t<T>, std::remove_reference_t<U>>::value;


// check that universal reference type is same as concrete type
template <typename T, typename U>
constexpr bool is_auto_constuctor_ref_v =
    std::is_same<std::remove_reference_t<T>, U>::value;


template <typename T> struct Tube {
  Tube(const T &t) : tube(t){};
  Tube(T &&t) : tube(std::move(t)){};
  using TubeTag = void;
  T tube;
};

template <typename T> auto make_tube(T &&t) {
  return Tube<std::remove_reference_t<T>>(std::forward<T>(t));
};

template <typename T> auto make_tube_singleton(T &&t) {
  return make_tube(hana::make_tuple(std::forward<T>(t)));
};

// Source is source + tuple_tube
template <typename S, typename T> struct Source {
  template <typename Sa, typename Ta>
  Source(Sa &&s, Ta &&t)
      : source(std::forward<Sa>(s)), tube(std::forward<Ta>(t)) {
    static_assert(is_auto_constuctor_ref_v<Sa, S>, "");
    static_assert(is_auto_constuctor_ref_v<Ta, T>, "");
  };
  using SourceTag = void;

  S source;
  T tube; // hana::tuple
};

template <typename S, typename T> auto make_source2(S &&s, T &&t) {
  return Source<std::remove_reference_t<S>, std::remove_reference_t<T>>(
      std::forward<S>(s), std::forward<T>(t));
};

template <typename S> auto make_source(S &&s) {
  return make_source2(std::forward<S>(s), hana::make_tuple());
};

// drain is tuple_tube + drain
template <typename T, typename D> struct Drain {
  template <typename Ta, typename Da>
  Drain(Ta &&t, Da &&d)
      : tube(std::forward<Ta>(t)), drain(std::forward<Da>(d)) {
    static_assert(is_auto_constuctor_ref_v<Ta, T>, "");
    static_assert(is_auto_constuctor_ref_v<Da, D>, "");
  };
  using DrainTag = void;

  T tube;
  D drain;
};

template <typename T, typename D> auto make_drain(T &&t, D &&d) {
  return Drain<std::remove_reference_t<T>, std::remove_reference_t<D>>(
      std::forward<T>(t), std::forward<D>(d));
};

template <typename D> auto make_drain1(D &&d) {
  return make_drain(hana::make_tuple(), std::forward<D>(d));
};

// capsule = source + tube + drain
template <typename S, typename T, typename D> struct Capsule {
  template <typename Sa, typename Ta, typename Da>
  Capsule(Sa &&s, Ta &&t, Da &&d)
      : source(std::forward<Sa>(s)), tube(std::forward<Ta>(t)),
        drain(std::forward<Da>(d)) {
    static_assert(is_auto_constuctor_ref_v<Sa, S>, "");
    static_assert(is_auto_constuctor_ref_v<Da, D>, "");
    static_assert(is_auto_constuctor_ref_v<Ta, T>, "");
  };
  using CapsuleTag = void;

  S source;
  T tube; // hana::tuple
  D drain;
};

template <typename S, typename T, typename D>
auto make_capsule(S &&s, T &&t, D &&d) {
  return Capsule<std::remove_reference_t<S>, std::remove_reference_t<T>,
                std::remove_reference_t<D>>(
      std::forward<S>(s), std::forward<T>(t), std::forward<D>(d));
};

template <typename L, typename R, typename = void, typename = void>
struct Compose {};

template <typename L, typename R>
struct Compose<L, R, typename L::TubeTag, typename R::TubeTag> {
  template <typename A, typename B> static auto compose(A &&a, B &&b) {
    static_assert(is_same_ref_v<L, A>, "");
    static_assert(is_same_ref_v<R, B>, "");
    return make_tube(hana::concat(a.tube, b.tube));
  }
};

template <typename L, typename R>
struct Compose<L, R, typename L::SourceTag, typename R::TubeTag> {
  template <typename A, typename B> static auto compose(A &&a, B &&b) {
    static_assert(is_same_ref_v<A, L>, "");
    static_assert(is_same_ref_v<B, R>, "");
    return make_source2(a.source, hana::concat(a.tube, b.tube));
  }
};

template <typename L, typename R>
struct Compose<L, R, typename L::TubeTag, typename R::DrainTag> {
  template <typename A, typename B> static auto compose(A &&a, B &&b) {
    static_assert(is_same_ref_v<A, L>, "");
    static_assert(is_same_ref_v<B, R>, "");
    return make_drain(hana::concat(a.tube, b.tube), b.drain);
  }
};

template <typename L, typename R>
struct Compose<L, R, typename L::SourceTag, typename R::DrainTag> {
  template <typename A, typename B> static auto compose(A &&a, B &&b) {
    static_assert(is_same_ref_v<A, L>, "");
    static_assert(is_same_ref_v<B, R>, "");
    return make_capsule(a.source, hana::concat(a.tube, b.tube), b.drain);
  }
};

template <typename A, typename B> auto compose(A &&a, B &&b) {
  using T = Compose<std::remove_reference_t<A>, std::remove_reference_t<B>>;
  return T::compose(std::forward<A>(a), std::forward<B>(b));
}


template <typename A, typename B> auto operator|(A &&a, B &&b) {
  return compose(std::forward<A>(a), std::forward<B>(b));
}

// namespace end
}
