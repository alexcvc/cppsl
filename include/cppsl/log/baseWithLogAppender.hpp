//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * @file
 * \brief  contains any with log.
 * \author Alexander Sacharov
 * \date    2022-01-07
 * \ingroup
 *****************************************************************************/

#ifndef INCLUDE_CPPSL_LOG_SOME_WITH_LOG_HPP
#define INCLUDE_CPPSL_LOG_SOME_WITH_LOG_HPP

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <memory>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include <cppsl/log/logAppender.hpp>

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------
namespace cppsl::log {

   //
   /// Class something with log
   /// Class implements Something which has a log appender
   /// @author Alexander Sacharov
   //
   class BaseWithLogAppender {
    public:
      /// constructor
      explicit BaseWithLogAppender(cppsl::log::log_appenderPtr logPtr) : m_logPtr(logPtr) {}

      /// destructor
      virtual ~BaseWithLogAppender() = default;

    protected:
      //-------------------------------------
      // protected members
      //-------------------------------------
     cppsl::log::log_appenderPtr m_logPtr;        ///< log-pointer

   };

} /* namespace cppsl */

#endif /* INCLUDE_CPPSL_LOG_SOME_WITH_LOG_HPP */
