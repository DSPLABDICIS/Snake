/*
 * snakeException.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: luzdora
 */

#include "snakeException.hpp"
#include <sstream>

using std::string;

/*
 * class SnakeException
 */

snake::SnakeException::SnakeException(ExceptionId id_, const string& message_, const string& file_, int line_) throw()
{}

snake::SnakeException::~SnakeException() throw() {}

snake::SnakeException::ExceptionId snake::SnakeException::getExceptionId() const throw() {
  return id;
}

string snake::SnakeException::exceptionIdToString(ExceptionId id_) throw() {
  switch(id_) {
//   case MY_ERROR:
//     return "MY_ERROR";

  default:
    std::stringstream s;
    s << id_;
    return s.str();
  }
}
