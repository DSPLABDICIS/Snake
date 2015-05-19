/*
 * snakeException.hpp
 *
 *  Created on: Feb 13, 2013
 *      Author: luzdora
 */

#ifndef SNAKEEXCEPTION_HPP_
#define SNAKEEXCEPTION_HPP_

#include <string>
//#include "kernel/jafarException.hpp"

namespace snake {

    /** Base class for all exceptions defined in the module
     * snake.
     *
     * @ingroup snake
     */
    class SnakeException {

    public:

      /** This enumeration defines exceptions id for the module
       * snake.
       */
      enum ExceptionId {
        //        MY_ERROR /**< my error */
      };

      /** Constructor. You should not use this constructor directly,
       * prefer macros jfrThrowEx or jfrCreateEx which fill for you
       * parameters \c file_ and \c line_.
       *
       * @param id_ exception id
       * @param message_ message used for debug
       * @param file_ where the exception was thrown
       * @param line_ where the exception was thrown
       */
      SnakeException(ExceptionId id_,
                            const std::string& message_,
                            const std::string& file_, int line_) throw();

      virtual ~SnakeException() throw();

      ExceptionId getExceptionId() const throw();

    protected:

      ExceptionId id;

      static std::string exceptionIdToString(ExceptionId id_) throw();

    }; // class SnakeException

} // namespace snake

#endif /* SNAKEEXCEPTION_HPP_ */
