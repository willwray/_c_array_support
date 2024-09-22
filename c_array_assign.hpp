/*
 SPDX-FileCopyrightText: 2024 The Lemuriad <wjwray@gmail.com>
 SPDX-License-Identifier: BSL-1.0
 Repo: https://github.com/Lemuriad/c_array_support
*/
#ifndef LML_C_ARRAY_ASSIGN_HPP
#define LML_C_ARRAY_ASSIGN_HPP
/*
  c_array_assign.hpp
  ==================

  Requires C++20 and depends on <concepts> and "c_array_support.hpp".

  This header defines 'assign(l)', generic assignment function, and its
  customization point 'assign_to', with C array specialization, plus a
  generic elementwise 'assign_elements(l,e...)' function:

   * assign(l) = r; a uniform assignment syntax for lvalue variables
   * assign_to<T[N]> an assignable reference-wrapper for array variables
   * assign_elements(l,e...) assigns elements directly by move or copy

  Traits and concepts for assign() are defined as versions of std traits
  that check element type e = lml::all_extents_removed<T> instead of T:

   * lml::is_assignable_v<T>      = std::is_assignable_v<e>
   * lml::is_copy_assignable_v<T> = std::is_copy_assignable_v<e>
   * lml::is_move_assignable_v<T> = std::is_move_assignable_v<e>

        ... plus all _trivially_ and _nothrow_ variants ...

   * lml::assignable_from<L,R> = std::assignable_from<eL,eR>
                                      && same_extents<L,R>

  New concepts are introduced for empty brace init and assignment: qq

   * lml::empty_list_initializable<T> true if T v{} is well-formed
   * lml::empty_list_assignable<T>    true if v = {} is well-formed

  (similar to std default_initializable<e> && assignable_from<e&,e>).

  The lml traits clearly lie about operator= (use std traits for that).
  They'll be true and agree with std traits once array copy semantics
  is implemented - wg21.link:/P1997 "Relaxing restrictions on array".
  Array copyability is automatically detected and used if possible.

  The problem
  ===========

  Assignment of C arrays is ill-formed today, e.g. given  int r[2] = {};

    int l[2] = r; // error: array must be initialized via braced-init
    l = r;        // error: array type 'int [2]' is not assignable
    l = {1,2};    // error: assigning to an array from an init-list

  C array copy initialization is out of reach of library solution, as is
  array as a function return type (or as a by-value function argument).

  C array assignment, on the other hand, is straightforward to implement
  but operator= can't be overloaded for array so can't be used directly.

  Usage
  =====
    lml::assign(l) = r
    lml::assign(l) = {}
    lml::assign(l) = {1,2}
    lml::assign_elements(l,4,2)

  An lvalue reference to l is returned, as for regular assignment l = r.

  assign_to
  =========
  The customization point class template assign_to has a specialization
  for C array, defined only if array copy semantics is not detected.

  assign_to is not intended to be used directly.
  assign(l) delegates to assign_to if assign_to<L> specialization exists

   lml::assign(l) returns assign_to{l}
                  or just lvalue l if assign_to isn't specialized for L

  A specialization of assign_to is provided for lml::tupl along with an
  overload of assign_elements.

  Performance
  ===========
  Copy optimizations such as memcpy specializations are not done, yet.
  Nested array copies have both constexpr and runtime implementations.
*/

#include <concepts>

#include "c_array_support.hpp"

#include "namespace.hpp"

// Detect language support for array copy semantics as proposed in P1997
//
inline constexpr bool is_copyable_array = std::copyable<int[1]>;

// is_X_constructible<T> array versions of std::is_X_assignable traits
//
template <typename T, typename...U>
inline constexpr bool is_constructible_v
              =  std::is_constructible_v<T,U...>;
template <c_array T, same_ish<T> U>
inline constexpr bool is_constructible_v<T,U>
               = std::is_constructible_v<all_extents_removed_t<T>
                                        ,all_extents_removed_t<U>>;
template <typename T, typename U> using is_constructible
                   = std::bool_constant<is_constructible_v<T,U>>;

template <typename T, typename...U>
inline constexpr bool is_trivially_constructible_v
              =  std::is_trivially_constructible_v<T,U...>;
template <c_array T, same_ish<T> U>
inline constexpr bool is_trivially_constructible_v<T,U>
               = std::is_trivially_constructible_v<all_extents_removed_t<T>
                                                  ,all_extents_removed_t<U>>;
template <typename T, typename U> using is_trivially_constructible
                   = std::bool_constant<is_trivially_constructible_v<T,U>>;

template <typename T, typename...U>
inline constexpr bool is_nothrow_constructible_v
              =  std::is_nothrow_constructible_v<T,U...>;
template <c_array T, same_ish<T> U>
inline constexpr bool is_nothrow_constructible_v<T,U>
               = std::is_nothrow_constructible_v<all_extents_removed_t<T>
                                                ,all_extents_removed_t<U>>;
template <typename T, typename U> using is_nothrow_constructible
                   = std::bool_constant<is_nothrow_constructible_v<T,U>>;

// assignable_from<L,R> array-enabled version of std::assignable_from
//
template <typename L, typename R>
concept assignable_from
 = (is_copyable_array || ! c_array<L>
 ? std::assignable_from<L,R>
 : std::assignable_from<all_extents_removed_t<L>,
                        all_extents_removed_t<R>>
   && same_extents<std::remove_cvref_t<L>,
                   std::remove_cvref_t<R>>);

// is_X_assignable<T,U> array versions of std::is_X_assignable traits
//
template <typename T, typename U>
inline constexpr bool is_assignable_v
               = (is_copyable_array || ! c_array<T>
               ? std::is_assignable_v<T,U>
               : std::is_assignable_v<all_extents_removed_t<T>,
                                      all_extents_removed_t<U>>);
template <typename T, typename U> using is_assignable
                   = std::bool_constant<is_assignable_v<T,U>>;

template <typename T, typename U>
inline constexpr bool is_trivially_assignable_v
               = (is_copyable_array || ! c_array<T>
               ? std::is_trivially_assignable_v<T,U>
               : std::is_trivially_assignable_v<all_extents_removed_t<T>,
                                                all_extents_removed_t<U>>);
template <typename T, typename U> using is_trivially_assignable
                   = std::bool_constant<is_trivially_assignable_v<T,U>>;

template <typename T, typename U>
inline constexpr bool is_nothrow_assignable_v
               = (is_copyable_array || ! c_array<T>
               ? std::is_nothrow_assignable_v<T,U>
               : std::is_nothrow_assignable_v<all_extents_removed_t<T>,
                                              all_extents_removed_t<U>>);
template <typename T, typename U> using is_nothrow_assignable
                   = std::bool_constant<is_nothrow_assignable_v<T,U>>;

// is_X_assignable<T> array versions of std::is_X_assignable traits
//
template <typename T>
inline constexpr bool is_copy_assignable_v = (is_copyable_array
               ? std::is_copy_assignable_v<T>
               : std::is_copy_assignable_v<all_extents_removed_t<T>>);
template <typename T> using is_copy_assignable
       = std::bool_constant<is_copy_assignable_v<T>>;

template <typename T>
inline constexpr bool is_move_assignable_v = (is_copyable_array
               ? std::is_move_assignable_v<T>
               : std::is_move_assignable_v<all_extents_removed_t<T>>);
template <typename T> using is_move_assignable
       = std::bool_constant<is_move_assignable_v<T>>;

template <typename T>
inline constexpr bool is_trivially_copy_assignable_v = (is_copyable_array
               ? std::is_trivially_copy_assignable_v<T>
               : std::is_trivially_copy_assignable_v<all_extents_removed_t<T>>);
template <typename T> using is_trivially_copy_assignable
       = std::bool_constant<is_trivially_copy_assignable_v<T>>;

template <typename T>
inline constexpr bool is_trivially_move_assignable_v = (is_copyable_array
               ? std::is_trivially_move_assignable_v<T>
               : std::is_trivially_move_assignable_v<all_extents_removed_t<T>>);
template <typename T> using is_trivially_move_assignable
       = std::bool_constant<is_trivially_move_assignable_v<T>>;

template <typename T>
inline constexpr bool is_nothrow_copy_assignable_v = (is_copyable_array
               ? std::is_nothrow_copy_assignable_v<T>
               : std::is_nothrow_copy_assignable_v<all_extents_removed_t<T>>);
template <typename T> using is_nothrow_copy_assignable
       = std::bool_constant<is_nothrow_copy_assignable_v<T>>;

template <typename T>
inline constexpr bool is_nothrow_move_assignable_v = (is_copyable_array
               ? std::is_nothrow_move_assignable_v<T>
               : std::is_nothrow_move_assignable_v<all_extents_removed_t<T>>);
template <typename T> using is_nothrow_move_assignable
       = std::bool_constant<is_nothrow_move_assignable_v<T>>;

// empty_list_initializable<T> concept
//  true if T can be copy initialized from empty list; T v = {};
//   i.e. T has a default constructor, not explicit (or deleted?)
//
template <typename T>
concept empty_list_initializable = requires (void f(T&&)) { f({}); };
//
template <typename T>
using is_empty_list_initializable = std::bool_constant<
         empty_list_initializable<T> >;

// empty_list_assignable<T> concept:
//  true if elements can be assigned to from an empty init-list, v = {}
//   (true for array of empty_list_assignable element type).
//
template <typename T>
concept empty_list_assignable = (is_copyable_array
  ? requires (T v) { static_cast<T>(v) = {}; }
  : requires (all_extents_removed_t<T> v) {
      static_cast<all_extents_removed_t<T>>(v) = {};
    });

template <typename T>
using is_empty_list_assignable = std::bool_constant<
         empty_list_assignable<T> >;

template <typename T>
using is_nothrow_empty_list_assignable = std::bool_constant<
         noexcept(std::declval<all_extents_removed_t<T>&>() = {})>;

// assign_to customization point to specialize as a reference-wrapper
//                                  with operator= overloads
// invoked by assign() function for types with assign_to specialization
//
template <typename L> struct assign_to;
template <typename L> assign_to(L&&) -> assign_to<L&&>;

template <typename L> concept assign_toable
          = requires { sizeof(assign_to<L>); };

// assign_to<c_array> specialization for array assignment, if needed.
// operator=(R) -> L& returns the unwrapped type, not the wrapper type.
//
template <c_array L>
  requires (! std::copyable<L>)
struct assign_to<L>
{
  L& l;

  using value_type = std::remove_reference_t<L>;

  // operator=({}) overload for emtpy braced-init
  //
  constexpr L& operator=(std::true_type) const
      noexcept(noexcept(flat_index(l) = {}))
    requires empty_list_assignable<L&>
  {
      for (int i = 0; i != flat_size<L>; ++i)
          flat_index(l, i) = {};
      return l;
  }

  // operator=(lval) overload for array lvalues (and rvalue variables)
  //
  template <c_array R>
    requires assignable_from<L, R&&>
  constexpr L& operator=(R&& r) const
      noexcept(noexcept(flat_index(l) = flat_index((R&&)r)))
  {
      for (int i = 0; i != flat_size<L>; ++i)
          flat_index(l, i) = flat_index((R&&)r, i);
      return l;
  }

  // operator=(rval) overload for array rvalue from braced-init
  //
  constexpr L& operator=(value_type const& r) const
      noexcept(noexcept(flat_index(l) = flat_index(r)))
  {
      for (int i = 0; i != flat_size<L>; ++i)
          flat_index(l, i) = flat_index(r, i);
      return l;
  }

};

// assign(l) returns assign_to{l}, if assign_toable, else reference-to-l
template <typename L>
constexpr decltype(auto) assign(L&& l) noexcept
{
    if constexpr (assign_toable<L&&>)
        return std::add_const_t<assign_to<L&&>>{l};
    else
        return (L&&)l;
}

template <c_array L, typename...T>
  requires (assignable_from<extent_removed_t<L>,T> && ...)
constexpr auto& assign_elements(L&& t, T&&...v)
  noexcept(noexcept( ((assign(extent_removed_t<L>(*t)) = (T&&)v),...) ))
{
  static_assert(sizeof...(T) == std::extent_v<std::remove_cvref_t<L>>,
               "assign_elements requires all elements to be assigned.");
  auto p = t;
  ((assign(static_cast<extent_removed_t<L>>(*p++)) = (T&&)v),...);
  return t;
}

#include "namespace.hpp"

#endif // LML_C_ARRAY_ASSIGN_HPP
