/*
 * This file is part of the source code of the software program
 * Vampire. It is protected by applicable
 * copyright laws.
 *
 * This source code is distributed under the licence found here
 * https://vprover.github.io/license.html
 * and in the source directory
 */
/**
 * @file Forwards.hpp
 * Forward declarations of some classes
 */
#ifndef __Debug_Output__
#define __Debug_Output__

#include <iostream>
#include <utility>

namespace Kernel {

template<class A, class B>
std::ostream& operator<<(std::ostream& out, std::pair<A,B> const& self)
{ return out << "(" << self.first << ", " << self.second << ")"; }

template<unsigned i, unsigned sz, class Tup> 
struct __OutputTuple
{
  static void apply(std::ostream& out, Tup const& self)
  {
    out << std::get<i>(self) << ", ";
    __OutputTuple<i + 1, sz, Tup>::apply(out, self);
  }
};

template<unsigned i, class Tup> 
struct __OutputTuple<i, i, Tup>  {
  static void apply(std::ostream& out, Tup const& self)
  {
    out << std::get<i>(self);
  }
};

template<class... As> std::ostream& operator<<(std::ostream& out, std::tuple<As...> const& self)
{ 
  out << "(";
  __OutputTuple<0, std::tuple_size<std::tuple<As...>>::value - 1, std::tuple<As...>>::apply(out, self);
  out << ")";
  return out;
}

/** Newtype in order to nicely output a pointer.
 * Usage: `out << outputPtr(ptr) << std::endl;` 
 */
template<class T>
struct OutputPtr {
  T* self;
  friend std::ostream& operator<<(std::ostream& out, OutputPtr const& self)
  { return self.self ? out << *self.self : out << "NULL"; }
};

template<class T>
OutputPtr<T> outputPtr(T* self) { return { .self = self, }; }

/** Newtype for outputting a datatype that implements it in multiline format.
 * Usage: `out << multiline(substitutioTree) << std::endl;` 
 *
 * You can implement this for a `MyType` by implementing 
 * std::ostream& operator<<(std::ostream&, OutputMultiline<MyType>)
 */
template<class T>
struct OutputMultiline { T const& self; };

template<class T>
OutputMultiline<T> multiline(T const& self)
{ return { self }; }


template<class T>
void repeat(std::ostream& out, T const& c, int times) 
{ for (int i = 0; i < times; i++) out << c; };

static constexpr char const* INDENT = "    ";

} // namespace Kernel
#endif // __Debug_Output__
