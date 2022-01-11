//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * \file   anyWithLog.hpp
 * \brief  contains any class with log.
 * \author Alexander Sacharov
 * \date    2022-01-07
 * \ingroup
 *****************************************************************************/

#ifndef F2762122_51B0_43FF_AE35_3A9F7B864965
#define F2762122_51B0_43FF_AE35_3A9F7B864965

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------

#include <memory>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include <cppsl/log/appender.hpp>

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

  /** cppsl::XmlFileWizard
   *  @brief
   */
  class anylog {
   public:
    //-------------------------------------
    // public methods
    //-------------------------------------
    /// destructor
    virtual ~anylog() = default;

    /// constructor
    explicit anylog(cppsl::logAppenderPtr logPtr) : m_logPtr(logPtr) {}

   protected:
    //-------------------------------------
    // protected members
    //-------------------------------------
    cppsl::logAppenderPtr m_logPtr;        ///< log-pointer

  };

} /* namespace cppsl */

#endif /* F2762122_51B0_43FF_AE35_3A9F7B864965 */
