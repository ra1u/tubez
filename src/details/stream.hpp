#pragma once

#include <type_traits>
#include <utility> // std::pair
#include "base.hpp"

#include <boost/type_traits.hpp>
#include <boost/optional.hpp>

namespace tubez {


enum class StreamStatus {
    terminated,
    open
};

namespace detail {

// compose Tube and Drain into Drain
template <typename T, typename D> struct DrainClass {
  template <typename Da>
  DrainClass(T &t, Da &&d)
      : tube(t), drain(std::forward<Da>(d)) {
    static_assert(is_auto_constuctor_ref_v<Da, D>, "");
  }
  template <typename V> auto push(V &&v) {
    return tube.push(drain, std::forward<V>(v));
  }
  auto get_result() { //
    return drain.get_result();
  }
  T &tube;
  D drain;
};

template <typename T, typename D> auto make_drainclass(T &t, D&& d) {
  return DrainClass<T, std::remove_reference_t<D>>(t, std::forward<D>(d));
}



// we us this class to drain inside by reference. 
// This enables function run_push compose by reference 
template <typename D> struct DrainRefClass {
  DrainRefClass(D &d) : drain(d) {}
  template <typename V> auto push(V &&v) {
    return drain.push(std::forward<V>(v));
  }
  auto get_result() { //
    return drain.get_result();
  }
  D &drain;
};



// 
// compose Source and one Tube into Source
template <typename S, typename T> struct SourceClass {
  template <typename Sa> SourceClass(Sa &s, T &t)
      : source(std::forward<Sa>(s)), tube(t) {
    static_assert(is_auto_constuctor_ref_v<Sa, S>, "");
  }
  auto pull() { //
    return tube.pull(source);
  }
  S source;
  T &tube;
};

template <typename S, typename T> auto make_sourceclass(S &&s, T &t) {
  return SourceClass<std::remove_reference_t<S>, T>(
      std::forward<S>(s), t);
}

// Src ref wrapper
template <typename S> struct SourceRefClass {
  SourceRefClass(S &s) : src(s) {}
  auto pull() {
    return src.pull();
  }
  S &src;
};



/**************************************************
 * Tubez functions
 **************************************************
 */

struct Void {
  Void() = delete;
};
struct Unit {};


//
template <typename T, typename F, typename P> struct tubeGenC {
  template <typename Ta, typename Fa, typename Pa >
  tubeGenC(Ta &&t, Fa &&fun_push,Pa &&fun_pull)
      : _state(t), _fun_push(fun_push), _fun_pull(fun_pull){}

  template <typename Da, typename Va> StreamStatus push(Da &d, Va &&v) {
    return _fun_push(&_state, &d, std::forward<Va>(v));
  }

  template <typename S> 
  auto pull(S& src) { //
    return _fun_pull(&_state,&src);
  }

  T _state;
  F _fun_push;
  P _fun_pull;
};

/************************************************
 *
 ************************************************/

template <typename T, typename F, typename P> struct sourceGenC {
  template <typename Ta, typename Fa, typename Fp>
  sourceGenC(Ta &&state, Fa &&fun_push, Fp &&fun_pull)
      : _state(std::forward<Ta>(state)), _fun_push(std::forward<Fa>(fun_push)),
        _fun_pull(std::forward<Fp>(fun_pull)) {
    static_assert(is_auto_constuctor_ref_v<Ta, T>, "");
    static_assert(is_auto_constuctor_ref_v<Fa, F>, "");
  }

  template <typename D> void run_push(D &d) {
    while (_fun_push(&_state, &d) == StreamStatus::open) {
    }
  }

  template <typename X = Void> 
  auto pull() { //
    return _fun_pull(&_state);
  }

  T _state;
  F _fun_push;
  P _fun_pull;
};


/************************************************
 *
 ************************************************/

//
template <typename T, typename F, typename R> struct drainGenC {
  template <typename Ta, typename Fa, typename Ra>
  drainGenC(Ta &&t, Fa &&fm, Ra &&rf)
      : _state(std::forward<Ta>(t)), _fun(std::forward<Fa>(fm)),
        _result_transform(std::forward<Ra>(rf)) {

    static_assert(is_auto_constuctor_ref_v<Ta, T>, "");
    static_assert(is_auto_constuctor_ref_v<Fa, F>, "");
    static_assert(is_auto_constuctor_ref_v<Ra, R>, "");
  }

  template <typename Va> StreamStatus push(Va &&v) {
    return _fun(_state, std::forward<Va>(v));
  }

  auto get_result() { return _result_transform(_state); }

  template <typename S> auto run_pull(S &s) {
    while (1) {
      auto x = s.pull();
      if (!x) {
        break;
      }
      if (push(*x) == StreamStatus::terminated) {
        break;
      }
    }
    return get_result();
  }

  T _state;
  F _fun;
  R _result_transform;
};


}

// namespace end
}
