//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * \file    rollAppender.hpp
 * \brief   contains roll file appender.
 * \author  Alexander Sacharov
 * \date    2022-01-07
 * \ingroup C++ support library
 *****************************************************************************/

#ifndef C5EBA765_8A29_465E_9F0A_DB76582AC3D2
#define C5EBA765_8A29_465E_9F0A_DB76582AC3D2

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <stdarg.h>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include <cppsl/log/anylog.h>
#include <cppsl/file/file_appender_base.h>

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

namespace cppsl::file {

  static const size_t MaxRollFileAppenderSize = 5 * 1024 * 1024;       ///< max size of roll file
  static const size_t MaxRollFileAppenderBackIndex = 10;               ///< max size of roll file

  /// @class PSS_RollingFileAppender
  ///
  /// @brief this class implements a rolling FileAppender that rolls over the
  /// logfile once it has reached a certain size limit.
  ///
  /// @details
  ///
  /// @author Alexander Sacharov (AS)
  /// @date   2015/03/13

  class fileroll_appender : public filebase_appender {
   public:
    fileroll_appender(std::shared_ptr<cppsl::log::appender> logPtr,
                      const cppsl::file::fs::path &filePath,
                      size_t maxFileSize = MaxRollFileAppenderSize,
                      unsigned int maxBackupIndex = MaxRollFileAppenderBackIndex,
                      bool append = true,
                      int mode = BitsAccessPermissions);

    virtual void setMaxBackupIndex(unsigned int maxBackups);

    virtual unsigned int getMaxBackupIndex() const;

    virtual void setMaximumFileSize(size_t maxFileSize);

    virtual size_t getMaxFileSize() const;

    virtual void rollOver();

    virtual void writeMessage(const std::string &message) override;

   protected:

    unsigned int        m_maxBackupIndex;
    unsigned short int  m_maxBackupIndexWidth;   // keep constant index width by zeroing leading positions
    size_t              m_maxFileSize;
  };
}  // end namespace cppsl::file

#endif /* C5EBA765_8A29_465E_9F0A_DB76582AC3D2 */
