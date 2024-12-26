//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************/ /**
 * @file
 * \brief   contains roll file SinkAppender.
 * \author  Alexander Sacharov
 * \date    2022-01-07
 * \ingroup C++ support library
 *****************************************************************************/

#pragma once

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <cppsl/file/fileBaseAppender.hpp>

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

namespace cppsl::file {

static const size_t maxRollFileAppenderSize = 5 * 1024 * 1024;   ///< max size of roll file
static const size_t maxRollFileAppenderBackIndex = 10;           ///< max size of roll file

/// @brief this class implements a rolling FileAppender that rolls over the
/// logfile once it has reached a certain size limit.
///
/// @author Alexander Sacharov (AS)
/// @date   2015/03/13

class FileRollAppender : public FileBaseAppender {
 public:
  FileRollAppender(const std::filesystem::path& filePath, size_t maxFileSize = maxRollFileAppenderSize,
                   uint maxBackupIndex = maxRollFileAppenderBackIndex, bool append = true,
                   std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);

  FileRollAppender(const std::filesystem::path& filePath, bool append, std::ios_base::openmode mode);

  /**
   * sets the maximum number of backup files that can be created for a particular log file.
   * If the number of log files exceeds this limit, the oldest log file will be deleted
   * to make room for the new one.
   * This helps to prevent the accumulation of too many log files in the system.
   *
   * @param maxBackups - specifies the maximum number of backup files that can be created.
   */
  virtual void setMaxBackupIndex(unsigned int maxBackups);

  /**
   * Returns the maximum backup index for the log file.
   * @return The maximum backup index as an unsigned integer.
   */
  [[nodiscard]] virtual unsigned int getMaxBackupIndex() const;

  /**
   * Sets the maximum file size for the log file.
   * @param maxFileSize The maximum file size in bytes.
   */
  virtual void setMaximumFileSize(size_t maxFileSize);

  /**
   * Returns the maximum file size for the log file.
   * @return The maximum file size as a size_t.
   */
  [[nodiscard]] virtual size_t getMaxFileSize() const;

  /**
   * Performs a rollover of the log file, creating a new log file if the maximum file size has been reached.
   */
  virtual void rollOver();

  /**
   * Writes the given message to the log file.
   * @param message The message to be written to the log file.
   */
  [[nodiscard]] virtual bool writeMessage(std::string_view message) override;

 protected:
  uint m_maxBackupIndex;            ///< the maximum backup index allowed.
  uint16_t m_maxBackupIndexWidth;   ///< width, in terms of number of digits, of the maximum backup index
  size_t m_maxFileSize;             ///< represents the maximum file size allowed.
};
}   // end namespace cppsl::file
