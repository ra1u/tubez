Tubez
=====

C++ library for streams
-----------------------

Experimental tubez program that calculates sum of
first `N` numbers

``` cpp
count(uint64_t(0)) 
   | take(N) 
   | sum<uint64_t>();
```

Here `count` generates stream of numbers `uint64_t` from 0 toward
infinity modulo 2\^64. It `take`-s first `N` numbers from infinite
stream and calculates `sum` in drain. Operator `|` works similar as same
operator from unix pipes.

We should expect *tubez* expression from example to compile into
something similar as code using `for` loop.

``` cpp
uint64_t sum_digits_for(uint64_t N) {
  uint64_t r = 0;
  for (uint64_t i = 0; i < N; ++i) {
    r += i;
  }
  return r;
}
```

Both implementations results in **exactly same binary code** when
comparing either `clang` and `gcc` output for each of implementations.

Minimum requirements
-------------------

1. It requires compiler supporting c++14 (tested with `gcc-6` and
`clang-3.6`).
2. boost libraries 1.61 or better due to requrement of
[hana](http://www.boost.org/libs/hana/). Additionaly it requires
[boost::optional](http://www.boost.org/libs/optional/).

Composition
-----------

From project euler 1 problem:

> If we list all the natural numbers below 10 that are multiples of 3 or
> 5, we get 3, 5, 6 and 9. The sum of these multiples is 23. Find the
> sum of all the multiples of 3 or 5 below 1000.

``` cpp
// use N = 1000 to solve 
auto pe1(uint64_t N) {
  auto t = 
    tubez::iter(uint64_t(0), [](auto a) { return a + 1; }) |              // source
    tubez::filter([](auto n) { return (n % 3 == 0) || (n % 5 == 0); }) |  // tube
    tubez::take_while([N](auto n) { return n < N; }) |                    // tube
    tubez::sum<uint64_t>();                                               // drain
  return run(t);
}
```

Composition here works by composing

    source | tube | {tube} | drain

Stream composition works in way that we compose one `source` than any
number of `tubez` and finaly `drain`. Under the hood it results in type
called `capsule` as closed tube on both ends.



Composition is associative

    (a | b) | c == a | (b | c)

this enables making new tubez from other tubez by tubing them in tubez. 


Source streams
--------------

Source streams are stream generators. They generate stream of data out
of thin air.

#### Source `count`

``` cpp
template <typename T>
auto count(T from, T step = T(1));
// return source
// 1. argument starting number
// 2. argument step (default value is 1)

```

#### Source `iter`

``` cpp
auto iter(S state, F &&fun);
/// returns source
/// 1. argument state S
/// 2. argument function F that has type A(*S).
///      Function fun gets called for each input and creates stream of type A
///
/// example - implements count
template <typename T> 
static auto count(T from, T step = T(1)) {
  return iter(from, [step](T a) { return a + step; });
}
```

#### Source `from_range`

``` cpp
template <typename Iter>
auto from_range(Iter begin, Iter end)
/// returns source
/// 2 arguments that are `begin` and `end` from where data is generated
///
/// example - source stream from vector 
std::vector<int> v({1,2,3});
auto src = from_range(v.cbegin(),v.cend());
```

Tube streams
------------

Tube stream can modify, limit or filter stream. In general it is
something, that takes single data from source and sends it down towards
drain.

#### Tube `map`

``` cpp
template <typename F>
auto map(F f)
/// returns tube
/// 
/// argument - function B(A)
/// transforms received elements of  A into elements of B
///
/// example: tube that multiply recievd elements by 2 
map([](int num)-> std::string { 
          return num * 2; 
})
```

#### Tube `take`

``` cpp
template <typename T> 
auto take(T n)
/// returns tube
/// 
/// argument - number n
///   closes stream when n numbers pass this tube
///
/// example allow only 5 elements go trough
take(5) 
```

#### Tube `filter`

``` cpp
template <typename P>
auto filter(P p)
/// returns *tube*
///
/// argument function *bool(A)* as predicate
///   when predicate evaluates true upstream elements from source are send to downstream 
///
/// example: only even numbers from source goes down to drain
filter([](int i)-> bool { return i % 2 == 0;})
``` 

#### Tube `take_while`

``` cpp
template <typename P> 
auto take_while(P p)
/// returns *tube*
///
/// argument function *bool(A)* as predicate
///  when predicate evaluates false tubes stops sending data and terminates stream
///
/// example: stops stream when number is too large
take_while([](int i)-> bool { return i <= 3;})
``` 

Drain streams
-------------

Drain is last element in strem processing chain. It can be seen as data
receiver and aggregation.

#### Drain  `reducer`

``` cpp
template <typename R, typename T> 
auto reducer(T &&t, R r)
/// returns *drain*
///
/// 1. argument inital state
/// 2. argument function of type void(T& state,A val) 
///         1. argument of this function is reference on state
///         2. argument is received stream element
///
/// final value of reduction is final state when stream terminates
///
/// example: make drain sum() that calculates total of received integers
static auto sum(){
   return tubez::reducer(int(0), [](int& a,const int val){
      a +=  val;
   });
}
``` 

#### Drain  `reducer_pure`

``` cpp
template <typename R, typename T> 
auto reducer_pure(T &&t, R r)
/// returns *drain*
///
/// 1. argument inital state
/// 2. argument function r of type T(T,A) 
///   where 1. argument is previous state
///         2. argument is received stream element
/// example: make drain sum() that calculates total of received integers
static auto sum(){
   return tubez::reducer_pure(int(0), [](int a,int val){
      return a + val;
   });
}
```

#### Drain  `push_back`

``` cpp
template <typename T> 
static auto push_back(T &&t)
/// returns *drain*
///
/// 1. argument container T that has "push_back" method 
///
/// make vector with elements from 0 to 9
std::vector<int> v;
auto tube = count(0) | take(10) | push_back(v);
std::vector<int> v0_9 = run(tube);
```

#### Drain  `push_back`

``` cpp
template <typename T> 
static auto sum()
/// returns *drain*
///
/// accumulates received elements in sum
/// example - calculate sum of first 42 natural numbers 
count(1)  | take(42) | sum<int>();
```
