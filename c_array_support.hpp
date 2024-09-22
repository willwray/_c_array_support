/*
 SPDX-FileCopyrightText: 2024 The Lemuriad <wjwray@gmail.com>
 SPDX-License-Identifier: BSL-1.0
 Repo: https://github.com/Lemuriad/c_array_support
*/
#ifndef LML_C_ARRAY_SUPPORT_HPP
#define LML_C_ARRAY_SUPPORT_HPP
/*
  c_array_support.hpp
  ===================

  c_array concept, type traits and utilities for handing C arrays.

  Depends on std <type_traits> and uses C++20 features.

  C arrays are hard to handle, multidimensional arrays especially.
  Generic code ought to handle arrays but they're often neglected.
  If arrays are unloved then zero-size arrays are total outcasts.

  This header provides tools to deal with arrays, possibly nested
  and possibly zero-size, plus corrected versions of some std lib
  traits where current library implementations give wrong results
  for zero-size arrays (it's undefined behavior, so fair enough).

  This is an incomplete collection, including only tools that have
  proven useful so far in practice.

 Replacement versions of std traits that work for T[0]:
  (i.e. lml::is_array_v replaces std::is_array_v, etc.)

  - is_array_v
  - is_bounded_array_v
  - rank_v
  - remove_extent_t
  - remove_all_extents
  - remove_all_extents_t

 Concepts:
  - c_array<A>: matches C array, including references to C array
  - c_array_unpadded<A>: matches unpadded C array, including references

 Value traits:
  - flat_size<A>: the total number of elements in flattened array A
  - same_extents<A,B>: predicate tells if A and B have the same extents

 Aliases:
  - c_array_t<T,I...>: template alias to C array type T[I][...]
  - extent_removed_t<A>: remove_extent, under any reference qualifier
  - all_extents_removed_t<T>: remove_all_extents, under any ref qual
  - flat_cast_t<A>: type of the flattened array A preserving cvref quals

 Functions:
  - flat_cast(a) returns flattened 1D array, preserving cvref quals
  - subscript(a,i): returns a[i], an rvalue if 'a' is an rvalue
  - flat_index(a,i=0): returns element at i in flat_cast(a)
*/

#include "util_traits.hpp"

#include "namespace.hpp"

// c_array_t<T,I...> is an alias to array type T[I][...]
// e.g. c_array_t<int> -> int (not an array if T is not)
//      c_array_t<int,0,2> -> int[0][2]
//
namespace impl {
#include "ALLOW_ZERO_SIZE_ARRAY.hpp"

template <typename T, int...>
extern T c_array_tv;
//
template <typename T, int... I>
using c_array_t = decltype(c_array_tv<T,I...>);
//
template <typename T, int J, int...I>
extern c_array_t<T,I...> c_array_tv<T, J, I...>[J];

#include "ALLOW_ZERO_SIZE_ARRAY.hpp"
}// impl
//
template <typename T, int... I>
using c_array_t = impl::c_array_t<T,I...>;

// is_array_v<A>
// same as std::is_array_v but supports zero-size array
// (detects array by 'decay' adjustment of a requires-expr parameter
//  after is_object has rejected reference, void and function types)
//
template <typename A>
inline constexpr bool is_array_v = requires {
  requires std::is_object_v<A>;
  requires requires (A p) {requires ! std::is_same_v<A, decltype(p)>;};
};

// is_bounded_array_v<A>
// same as std::is_bounded_array_v but supports zero-size array
//
template <typename A>
inline constexpr bool is_bounded_array_v = is_array_v<A>
          && ! std::is_unbounded_array_v<A>;

// remove_extent_t<A>
namespace impl {
template <typename A> requires (! is_array_v<A>)
auto remove_extent_f() -> std::type_identity<A>;
//
template <typename A> requires is_array_v<A>
auto remove_extent_f(A a={}) -> std::remove_pointer<decltype(a)>;
}
// remove_extent_t<A>
// same as std::remove_extent_t but supports zero-size array
//
template <typename A> using remove_extent_t =
    typename decltype(impl::remove_extent_f<A>())::type;

// remove_all_extents<A>
// same as std::remove_all_extents but supports zero-size array
//
template <typename A>
struct remove_all_extents { using type = A; };
//
template <typename A> requires is_array_v<A>
struct remove_all_extents<A> {
    using type = typename remove_all_extents<remove_extent_t<A>>::type;
};
// remove_all_extents_t<A>
// same as std::remove_all_extents_t but supports zero-size array
//
template <typename A>
using remove_all_extents_t = typename remove_all_extents<A>::type;

// rank_v<A>:
// same as std::rank_v but supports zero-size array
//
template <typename A>
inline constexpr auto rank_v = [] {
    if constexpr (is_array_v<A>)
         return 1 + rank_v<remove_extent_t<A>>; // recurse
    else return 0;
}();

// c_array concept: matches C array, including references to C array,
// including zero-size array, but not including unbounded array type.
// The second parameter allows to constrain or test the element type,
// cv unqualified: c_array<char> auto& a = "a"; matches const char
//
template <typename A,
          typename E = remove_extent_t<std::remove_cvref_t<A>>>
concept c_array = is_bounded_array_v<std::remove_cvref_t<A>>
                        && requires (std::remove_cvref_t<A> p)
                         { requires std::is_same_v<E*,decltype(p)>; };

// flat_size<A>: total number of elements in array A, or 1 for non array
// computed by recursion; the product of extents of all ranks of A
// (and not by sizeof which may fail for zero-size arrays)
//
template <typename Ar>
inline constexpr auto flat_size = [] {
    using A = std::remove_cvref_t<Ar>;
    using E = remove_extent_t<A>;
    if constexpr (is_array_v<E>)
         return std::extent_v<A> * flat_size<E>; // recurse
    else return std::extent_v<A> + !is_array_v<A>;
}();

// same_extents<A,B>: trait to tell if A and B have the same extents;
// array types with the same extents or both rank 0 (non-array) types
//
template <typename A, typename B>
inline constexpr bool same_extents =
      (std::rank_v<A> == 0 && std::rank_v<B> == 0)
   || (std::extent_v<A> == std::extent_v<B>
    && same_extents<remove_extent_t<A>,remove_extent_t<B>>);

// c_array_unpadded concept:
// matches C array (and reference-to-array) that has no padding,
// i.e. sizeof array == total element count times sizeof element
// (false on MSVC for zero-size arrays, true on GCC and Clang)
// (are there other cases of 'padded' arrays? Best be paranoid)
//
template <typename A>
concept c_array_unpadded = c_array<A>
    && flat_size<A> * sizeof(remove_all_extents_t<
                             std::remove_cvref_t<A>>) == sizeof(A);

// extent_removed_t<T> remove_extent, under any reference qualifier
//                e.g. extent_removed_t<int(&&)[1][2]> -> int(&&)[2]
template <c_array A>
using extent_removed_t = apply_ref_t<A,remove_extent_t<
                                     std::remove_reference_t<A>>>;

// all_extents_removed_t<T> remove_all_extents, under any ref qualifier
//                     e.g. all_extents_removed_t<int(&)[1][2]> -> int&
template <typename T>
using all_extents_removed_t = apply_ref_t<T,remove_all_extents_t<
                                          std::remove_reference_t<T>>>;

// flat_cast_t<A> type of the flattened array A, preserving cvref quals
//               e.g. flat_cast_t<int const(&)[2][3]> -> int const(&)[6]
//
template <c_array_unpadded A, typename E = remove_all_extents_t<
                                           std::remove_reference_t<A>>>
using flat_cast_t = apply_ref_t<A, E[flat_size<A>]>;

// flat_cast(a) cast to flat_cast_t, a reinterpret_cast for an ND array.
//            Returns a, the identity, for 1D array so can be constexpr.
//
// Prefer flat_index(a,i) over flat_cast(a)[i] to avoid reinterpet_cast.
//
template <c_array_unpadded A>
constexpr auto&& flat_cast(A&& a) noexcept {
  if constexpr (rank_v<std::remove_cvref_t<A>> == 1)
    return (A&&)a;
  else
    return reinterpret_cast<flat_cast_t<A&&>>(a);
}

// subscript(a,i)
// returns a[i], an rvalue if argument 'a' is an array rvalue
//  workaround for MSVC https://developercommunity.visualstudio.com/t/
//  subscript-expression-with-an-rvalue-array-is-an-xv/1317259
//
template <c_array A, typename Int = int>
constexpr auto subscript(A&& a, Int i = 0) noexcept
           -> extent_removed_t<A&&>
{
  if constexpr (std::is_lvalue_reference_v<decltype(a)>)
    return a[i];
  else {
    using R = extent_removed_t<A&&>;
    return [](R&v)noexcept->R {return R(v);}(a[i]); // move(a[i]) equiv
  }
}

// flat_index_recurse(a,i)
// Returns the element at index i of the flattened array. A recursive
// implementation is constexpr-correct but may give poor runtime codegen
// if it isn't inlined with the div/mod arithmetic folded out.
//
constexpr auto& flat_index_recurse(c_array auto& a, auto i)
  noexcept
{
  using E = decltype(a[0]);
  if constexpr (c_array<E>)
  {
    constexpr auto M = flat_size<E>;
    return flat_index_recurse(a[i/M], i%M);
  }
  else return a[i];
}

// flat_index(a,i)
//   for non-array a returns a (the identity function, and i is ignored)
//   else returns the element at index i of the flattened array.
// Non-constant evaluation avoids div/mod maths, which has poor codegen.
// Note that the index is not bounds-checked; constant evaluation fails
// for out-of-bounds access. Non-constant evaluated access should use
// static analysis and instrumented runs to check for bounds errors.
//
template <typename A, typename Int = int>
constexpr auto flat_index(A&& a, Int i = 0) noexcept
           -> all_extents_removed_t<A&&>
{
  using R = all_extents_removed_t<A&&>;
  constexpr auto mover = [](R&v)noexcept->R{return R(v);};
  if constexpr (!c_array<A>)
    return mover(a);
  else if (std::is_constant_evaluated() || ! c_array_unpadded<A>)
  {
    using E = std::remove_cvref_t<decltype(a[0])>;

    if constexpr (rank_v<E> == 0) {
      return mover(a[i]);
    }
    else if constexpr (rank_v<E> == 1) {
      constexpr auto N = std::extent_v<E>;
      return mover(a[i/N][i%N]);
    }
    else if constexpr (rank_v<E> == 2) {
      constexpr auto M = std::extent_v<E,0>;
      constexpr auto N = std::extent_v<E,1>;
      return mover(a[i/N/M][i/N%M][i%N]);
    }
    else if constexpr (rank_v<E> == 3) {
      constexpr auto L = std::extent_v<E,0>;
      constexpr auto M = std::extent_v<E,1>;
      constexpr auto N = std::extent_v<E,2>;
      return mover(a[i/L/M/N][i/N/M%L][i/N%M][i%N]);
    }
    else
      return mover(flat_index_recurse(a,i));
  }
  else
    return mover(flat_cast(a)[i]); // No bounds check
}

#include "namespace.hpp"

#endif // LML_C_ARRAY_SUPPORT_HPP
