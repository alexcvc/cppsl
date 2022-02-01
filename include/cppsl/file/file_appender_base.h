//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * \file    baseAppender.hh
 * \brief   contains base class SinkAppender.
 * \author  Alexander Sacharov
 * \date    2022-01-06
 * \ingroup
 *****************************************************************************/

#ifndef __INCLUDE_CPPSL_FILE_FILE_APPENDER_BASE_H__
#define __INCLUDE_CPPSL_FILE_FILE_APPENDER_BASE_H__

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <stdarg.h>
#include <sys/stat.h>
#include <filesystem>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include <cppsl/log/some_withlog.h>

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

using namespace std::filesystem;

namespace cppsl::file {

  namespace fs = std::filesystem;

  static constexpr int BitsAccessPermissions = ( S_IFREG |  S_IREAD | S_IWRITE);

  /// @class
  ///
  /// @brief File SinkAppender
  ///
  /// @details
  ///
  /// @author Alexander Sacharov (AS)
  /// @date   2015/03/13

  class FilebaseAppender : cppsl::log::SomeWithLog {
   public:

    /// Constructs a base_file_appender.
    /// \param logPtr - shared pointer to log
    /// \param filePath the name of the file to which the SinkAppender has to log.
    /// \param append whether the SinkAppender has to truncate the file or just append
    /// to it if it already exists. Defaults to 'true'.
    /// \param mode file mode to open the logfile with. Defaults to ( _S_IREAD | _S_IWRITE ).
    FilebaseAppender(std::shared_ptr<cppsl::log::SinkAppender> logPtr,
                     const cppsl::file::fs::path &filePath,
                     bool append = true,
                     int mode = BitsAccessPermissions);

    virtual ~FilebaseAppender();

    /// get file path
    const cppsl::file::fs::path &getFilePath() const { return m_filePath; }

    /// Reopens the logfile.
    /// This can be useful for logfiles that are rotated externally,
    /// e.g. by logrotate. This method is a NOOP for FileAppenders that
    /// have been constructed with a file descriptor.
    /// \return true if the reopen succeeded, otherwise - false
    virtual bool reopenFile();

    /// Closes the logfile.
    void closeFile();

    /// Sets the append vs truncate flag.
    /// NB. currently the base_file_appender opens the logfile in the constructor.
    /// Therefore this method is too late to influence the first file opening.
    /// \param append false to truncate, true to append
    virtual void setAppend(bool append);

    /// Gets the value of the 'append' option.
    /// \return true if successfully, otherwise - false
    virtual bool getAppend() const;

    /// Sets the file open mode.
    /// \param mode file mode. Default ( _S_IREAD | _S_IWRITE ).
    virtual void setMode(int mode);

    /// Gets the file open mode.
    virtual int getMode() const;

   protected:
    /// Gets file descriptor.
    int getFileDesc() const { return m_fd; }

   protected:
    /// write message to file
    virtual void writeMessage(const std::string &message);

    cppsl::file::fs::path m_filePath;  ///< log file path
    int                   m_fd;        ///< descriptor
    int                   m_flags;     ///< flag
    int                   m_mode;      ///< mode
  };

}  // end namespace cppsl::file

#endif /* __INCLUDE_CPPSL_FILE_FILE_APPENDER_BASE_H__ */
