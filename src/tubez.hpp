/** \file tubez.hpp
 * A brief file description.
 * A more elaborated file description.
 */

#pragma once

#include "details/stream.hpp"
#include "details/source.hpp"
#include "details/tube.hpp"
#include "details/drain.hpp"
#include <boost/hana/fold_right.hpp>
#include <boost/hana/fold_left.hpp>

///
/// namespace doc
///
namespace tubez {


// construct object Tube, for filtering data out of stream
// constructor takes


///
/// put on mainpage
///
template <typename S, typename T, typename D>
auto run_push(Capsule<S, T, D> &b) {
  auto x = hana::fold_right(
      b.tube, detail::DrainRefClass<D>(b.drain),
      [](auto &t, auto d) { return detail::make_drainclass(t, d); });
  b.source.run_push(x);
  return x.get_result();
}

template <typename S, typename T, typename D>
auto run_pull(Capsule<S, T, D> &b) {
  auto x = hana::fold_left(
      b.tube, detail::SourceRefClass<S>(b.source),
      [](auto s, auto &t) { return detail::make_sourceclass(s, t); });
  return b.drain.run_pull(x);
}

template <typename S, typename T, typename D>
auto run(Capsule<S, T, D> &b) {
    return run_push(b);
}


// namespace end
}
