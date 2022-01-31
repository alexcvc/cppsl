//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * \file   SomeWithLog.h
 * \brief  contains any with log.
 * \author Alexander Sacharov
 * \date    2022-01-07
 * \ingroup
 *****************************************************************************/

#ifndef __INCLUDE_CPPSL_LOG_SOME_WITHLOG_H__
#define __INCLUDE_CPPSL_LOG_SOME_WITHLOG_H__

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <memory>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include <cppsl/log/sink_appender.h>

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
   /// Class implements some object with log SinkAppender
   /// @author Alexander Sacharov
   //
   class SomeWithLog {
    public:
      /// constructor
      explicit SomeWithLog(cppsl::logAppenderPtr logPtr) : m_logPtr(logPtr) {}

      /// destructor
      virtual ~SomeWithLog() = default;

    protected:
      //-------------------------------------
      // protected members
      //-------------------------------------
      cppsl::logAppenderPtr m_logPtr;        ///< log-pointer

   };

} /* namespace cppsl */

#endif /* __INCLUDE_CPPSL_LOG_SOME_WITHLOG_H__ */
