// Copyright 2019 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.
//
// Macros for counting the number of arguments passed to a variadic
// function-like macro.
#pragma once

#include "pw_preprocessor/boolean.h"

// PW_ARG_COUNT counts the number of arguments it was called with. It evalulates
// to an integer literal in the range 0 to 64. Counting more than 64 arguments
// is not currently supported.
//
// PW_ARG_COUNT is most commonly used to count __VA_ARGS__ in a variadic macro.
// For example, the following code counts the number of arguments passed to a
// logging macro:
//
/*   #define LOG_INFO(format, ...) {                             \
         static const int kArgCount = PW_ARG_COUNT(__VA_ARGS__); \
         SendLog(kArgCount, format, ##__VA_ARGS__);              \
       }
*/
// clang-format off
#define PW_ARG_COUNT(...)                            \
  _PW_ARG_COUNT_IMPL(__VA_ARGS__,                    \
                     64, 63, 62, 61, 60, 59, 58, 57, \
                     56, 55, 54, 53, 52, 51, 50, 49, \
                     48, 47, 46, 45, 44, 43, 42, 41, \
                     40, 39, 38, 37, 36, 35, 34, 33, \
                     32, 31, 30, 29, 28, 27, 26, 25, \
                     24, 23, 22, 21, 20, 19, 18, 17, \
                     16, 15, 14, 13, 12, 11, 10,  9, \
                      8,  7,  6,  5, 4,  3,  2,  PW_HAS_ARGS(__VA_ARGS__))

// Expands to 1 if one or more arguments are provided, 0 otherwise.
#define PW_HAS_ARGS(...) PW_NOT(PW_HAS_NO_ARGS(__VA_ARGS__))

// Expands to 0 if one or more arguments are provided, 1 otherwise. This
// approach is from Jens Gustedt's blog:
//   https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/
//
// Normally, with a standard-compliant C preprocessor, it's impossible to tell
// whether a variadic macro was called with no arguments or with one argument.
// A macro invoked with no arguments is actually passed one empty argument.
//
// This macro works by checking for the presense of a comma in four situations.
// These situations give the following information about __VA_ARGS__:
//
//   1. It is two or more variadic arguments.
//   2. It expands to one argument surrounded by parentheses.
//   3. It is a function-like macro that produces a comma when invoked.
//   4. It does not interfere with calling a macro when placed between it and
//      parentheses.
//
// If a comma is not present in 1, 2, 3, but is present in 4, then __VA_ARGS__
// is empty. For this case (0001), and only this case, a corresponding macro
// that expands to a comma is defined. The presence of this comma determines
// whether any arguments were passed in.
//
// C++20 introduces __VA_OPT__, which would greatly simplify this macro.
#define PW_HAS_NO_ARGS(...)                                            \
  _PW_HAS_NO_ARGS(_PW_HAS_COMMA(__VA_ARGS__),                          \
                  _PW_HAS_COMMA(_PW_MAKE_COMMA_IF_CALLED __VA_ARGS__), \
                  _PW_HAS_COMMA(__VA_ARGS__()),                        \
                  _PW_HAS_COMMA(_PW_MAKE_COMMA_IF_CALLED __VA_ARGS__()))

#define _PW_HAS_COMMA(...)                                           \
  _PW_ARG_COUNT_IMPL(__VA_ARGS__,                                    \
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)

#define _PW_ARG_COUNT_IMPL(a64, a63, a62, a61, a60, a59, a58, a57, \
                           a56, a55, a54, a53, a52, a51, a50, a49, \
                           a48, a47, a46, a45, a44, a43, a42, a41, \
                           a40, a39, a38, a37, a36, a35, a34, a33, \
                           a32, a31, a30, a29, a28, a27, a26, a25, \
                           a24, a23, a22, a21, a20, a19, a18, a17, \
                           a16, a15, a14, a13, a12, a11, a10, a09, \
                           a08, a07, a06, a05, a04, a03, a02, a01, \
                           count, ...)                             \
  count

// clang-format on
#define _PW_HAS_NO_ARGS(a1, a2, a3, a4) \
  _PW_HAS_COMMA(_PW_PASTE_RESULTS(a1, a2, a3, a4))
#define _PW_PASTE_RESULTS(a1, a2, a3, a4) _PW_HAS_COMMA_CASE_##a1##a2##a3##a4
#define _PW_HAS_COMMA_CASE_0001 ,
#define _PW_MAKE_COMMA_IF_CALLED(...) ,

// Expands to a comma followed by __VA_ARGS__, if __VA_ARGS__ is non-empty.
// Otherwise, expands to nothing. This is useful when calling a function with
// __VA_ARGS__, since it removes the extra comma when no arguments are
// provided.
//
// This is a more flexible, standard-compliant version of ##__VA_ARGS__. Unlike
// ##__VA_ARGS__, this can be used to eliminate an unwanted comma when
// __VA_ARGS__ expands to an empty argument because an outer macro was called
// with __VA_ARGS__ instead of ##__VA_ARGS__.
//
// This can be used to call variadic functions or provide variadic template
// parameters from a macro. For example:
//
//  #define MY_PRINTF(fmt, ...) MY_PRINTF_IMPL(fmt, __VA_ARGS__)
//  #define MY_PRINTF_IMPL(fmt, ...) printf(fmt PW_COMMA_ARGS(__VA_ARGS__))
//
#define PW_COMMA_ARGS(...) _PW_COMMA_ARGS(PW_HAS_ARGS(__VA_ARGS__), __VA_ARGS__)

#define _PW_COMMA_ARGS(has_args, ...) _PW_COMMA_ARGS_X(has_args, __VA_ARGS__)
#define _PW_COMMA_ARGS_X(has_args, ...) _PW_COMMA_ARGS_##has_args(__VA_ARGS__)
#define _PW_COMMA_ARGS_0(...)                // no args, no comma
#define _PW_COMMA_ARGS_1(...) , __VA_ARGS__  // comma, followed by args
