#ifndef STRINGHELPERS_HPP
#define STRINGHELPERS_HPP

#include<sstream>

// std::to_string does not work on MinGW
template <typename T>
std::string toString(const T& value);

#include "StringHelpers.inl"
#endif //STRINGHELPERS_HPP