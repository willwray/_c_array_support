/*
 SPDX-FileCopyrightText: 2024 The Lemuriad <wjwray@gmail.com>
 SPDX-License-Identifier: BSL-1.0
 Repo: https://github.com/Lemuriad/c_array_support
*/
#ifndef LML_UTIL_TRAITS_HPP
#define LML_UTIL_TRAITS_HPP

#include <type_traits>

#include "namespace.hpp"

// same_ish<T,U> concept utility, removes cvref to match qualifed types
//                                (ToDo: move to a generic traits lib)
template <typename T, typename U>
concept same_ish = std::is_same_v<U, std::remove_cvref_t<T>>;

// rvalue<T> concept utility, false for lvalue T
//                                (ToDo: move to a generic traits lib)
template <typename T>
concept rvalue = std::is_rvalue_reference_v<T&&>;

// apply_ref and copy_ref utilities
//
namespace impl {

template<typename L, typename R>
auto apply_ref()
{
  if constexpr (std::is_reference_v<L>)
  {
    if constexpr (std::is_lvalue_reference_v<L>)
      return std::type_identity<R&>{};
    else
      return std::type_identity<R&&>{};
  }
  else
    return std::type_identity<R>{};
}

template<typename L, typename R>
auto apply_cv()
{
  if constexpr (std::is_const_v<L>)
  {
    if constexpr (std::is_volatile_v<L>)
      return std::type_identity<R const volatile>{};
    else
      return std::type_identity<R const>{};
  }
  else
  {
    if constexpr (std::is_volatile_v<L>)
      return std::type_identity<R volatile>{};
    else
      return std::type_identity<R>{};
  }
}
} // impl

// apply_ref<L,R> applies any reference qualifier on L to R
//             => reference collapse if R is already a reference
template <typename L, typename R>
using apply_ref = decltype(impl::apply_ref<L,R>());

template <typename L, typename R>
using apply_ref_t = typename apply_ref<L,R>::type;

// copy_ref<L,R> imposes any reference qualifier on L to R
//
template <typename L, typename R>
using copy_ref = apply_ref<L, std::remove_reference_t<R>>;

template <typename L, typename R>
using copy_ref_t = typename copy_ref<L,R>::type;

// apply_cv<L,R> applies any cv qualifier on L to R
//
template <typename L, typename R>
using apply_cv = decltype(impl::apply_cv<L,R>());

template <typename L, typename R>
using apply_cv_t = typename apply_cv<L,R>::type;

// copy_cv<L,R> imposes any cv qualifiers on L to R, removing any ref
//
template <typename L, typename R>
using copy_cv = apply_cv<L, std::remove_cvref_t<R>>;

template <typename L, typename R>
using copy_cv_t = typename copy_cv<L,R>::type;

template <typename L, typename R>
using apply_cvref = apply_ref<L,apply_cv_t<std::remove_reference_t<L>,R>>;

template <typename L, typename R>
using apply_cvref_t = typename apply_cvref<L,R>::type;

template <typename L, typename R>
using copy_cvref = apply_cvref<L,std::remove_cvref_t<R>>;

template <typename L, typename R>
using copy_cvref_t = typename copy_cvref<L,R>::type;

#include "namespace.hpp"

#endif // LML_UTIL_TRAITS_HPP
