/*! \file tube.hpp
    \brief A Documented file.
    
    Details.
*/

#pragma once

#include "stream.hpp"

namespace tubez {

/*!
* Create generic tube class from having internal state t
* one push function and other pull function 
*
* returns class Tube
*  
* 1. argument - state S received as pointer that can be modified
* 2. argument - push function
* 3. argument - pull function
*
*
* @include example/doc/tube_generic.hpp
* 
*/
template <typename Ta, typename Fa, typename Pa>
auto tube_generic(Ta &&t, Fa &&fpush, Pa &&fpull) {
  return make_tube_singleton(
      detail::tubeGenC<std::remove_reference_t<Ta>, std::remove_reference_t<Fa>,
                       std::remove_reference_t<Pa>>(
          std::forward<Ta>(t), std::forward<Fa>(fpush), std::forward<Pa>(fpull)));
}

/*! map function, 
 * 
 *  returns *tube*
 * 
 * takes function B(A)
 * transforms stream A into stream B
 * 
 * @include example/doc/map.hpp 
 */
template <typename P> auto map(P p) {
  auto fpush = [p](auto *_void, auto *gen, auto v) { //
    return gen->push(p(v));
  };
  auto fpull = [p](auto *_void, auto src) {
    auto r = src->pull();
    using Rt = boost::optional<decltype(p(*r))>;
    if (r) {
      return Rt(p(*r));
    } else {
      return Rt();
    }
  };
  return tube_generic(Unit(), fpush, fpull);
}

/*! 
    filter function, 
  
    returns *tube*
    argument function *bool(A)*
  
    takes function bool(A) as predicate
    values where predicate evaluates true are send to downstream 
 */
template <typename P> auto filter(P p) {
  auto fpush = [p](Unit *_, auto *gen, auto v) {
    if (p(v))
      return gen->push(v);
    else
      return StreamStatus::open; 
  };
  auto fpull = [p](Unit *_, auto *src) -> decltype(src->pull()) {
    using Rt = decltype(src->pull());
    while (1) {
      Rt r = src->pull();
      if (r) {
        if (p(*r)) {
          return r;
        }
      } else {
        return Rt(); // is it better to return r?
      }
    }
  };
  return tube_generic(Unit(), fpush, fpull);
}

/*! take function, 
 * 
 * returns *tube*
 * argument number N 
 * 
 * allows only N values to pass trough stream and then it terminates stream
 *   
 */
template <typename T> auto take(T n) {
  using Tv = std::remove_reference_t<T>;
  auto fpush = [](Tv *_cnt, auto *gen, auto v) -> StreamStatus {
    if (*_cnt > Tv(0)) {
      --(*_cnt);
      return gen->push(v);
    } else {
      return StreamStatus::terminated; // no more data
    }
  };
  auto fpull = [](Tv *_cnt, auto *src) {
    using Rt = decltype(src->pull());
    if (*_cnt > Tv(0)) {
      --(*_cnt);
      return src->pull();
    } else {
      return Rt(); // no more data
    }
  };
  return tube_generic(Tv(n), fpush, fpull);
}

/*! take while function, 
 * 
 *  returns *tube*
 *  argument function *bool(A)*
 * 
 * allows values to pass trough as long as predicate *bool(A)* evaluates true
 * when predicate evaluates false it terminates stream
 */
template <typename P> auto take_while(P p) {
  auto fpush = [p](auto *_void, auto *gen, auto v) -> StreamStatus {
    if (p(v))
      return gen->push(v);
    else
      return StreamStatus::terminated; // no more data
  };
  auto fpull = [p](auto *_void, auto src) {
    auto r = src->pull();
    using Rt = decltype(r);
    if (r && p(*r)) {
      return r;
    } else {
      return Rt();
    }
  };
  return tube_generic(Unit(), fpush, fpull);
}
}
