/*
 SPDX-FileCopyrightText: 2024 The Lemuriad <wjwray@gmail.com>
 SPDX-License-Identifier: BSL-1.0
 Repo: https://github.com/Lemuriad/c_array_support
*/
/* ALLOW_ZERO_SIZE_ARRAY.hpp

   Silence warnings on use of zero-size array type T[0]
   (else -Werror or -pedantic-error will cause compile FAIL)

   #include this header twice, sandwiching the code to silence:

struct Array
{
# include "ALLOW_ZERO_SIZE_ARRAY.hpp" // disable warnings
  int a[0];
# include "ALLOW_ZERO_SIZE_ARRAY.hpp" // reenable warnings
}

   Use surgically, on the smallest code sections.
   Occasionally compile with warnings and check them...
*/

#if !defined(ALLOW_ZERO_SIZE_ARRAY)

#    define  ALLOW_ZERO_SIZE_ARRAY

#if defined(__clang__) || defined(__GNUG__)
_Pragma("GCC diagnostic push")
_Pragma("GCC diagnostic ignored \"-Wpedantic\"")
#elif defined(_MSC_VER)
__pragma(warning(push))
__pragma(warning(disable:4200)) // L2: nonstandard extension used: zero-sized array in struct/union
__pragma(warning(disable:4815)) // zero-sized array in stack object will have no elements...
__pragma(warning(disable:4816)) // L4: parameter has a zero-sized array which will be truncated ...
#else
#endif

#else

# undef ALLOW_ZERO_SIZE_ARRAY

#if defined(__clang__) || defined(__GNUG__)
_Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
__pragma(warning(pop))
#else
#endif

#endif
