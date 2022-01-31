//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * \file   scopedThread.h
 * \brief  contains scoped joinable thread .
 * \author Alexander Sacharov
 * \date    2022-01-31
 * \ingroup
 *****************************************************************************/

#ifndef __INCLUDE_CPPSL_THREADING_SCOPEDTHREAD_H__
#define __INCLUDE_CPPSL_THREADING_SCOPEDTHREAD_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <thread>

//----------------------------------------------------------------------------
// Defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Prototypes
//----------------------------------------------------------------------------

namespace cppsl::threading {

   class ScopedThread {
      std::thread t;
    public:
      explicit ScopedThread(std::thread t_) :
         t(std::move(t_)) {
        if (!t.joinable())
          throw std::logic_error("No thread");
      }
      ~ScopedThread() {
        t.join();
      }
      ScopedThread(ScopedThread const &) = delete;
      ScopedThread &operator=(ScopedThread const &) = delete;
   };

} // end of cppsl::threading

#endif //__INCLUDE_CPPSL_THREADING_SCOPEDTHREAD_H__
