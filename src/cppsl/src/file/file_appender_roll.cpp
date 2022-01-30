//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
* @file    fileRollAppender.cpp
* @brief   rolling FileAppender that rolls over the logfile once it has
 * reached a certain size limit.
* @author  Alexander Sacharov <a.sacharov@asoft-labs.de>
* @ingroup COM4CPP Library
*****************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include "cppsl/file/file_appender_roll.h"

using namespace std;
using namespace cppsl;
using namespace cppsl::file;

/// @breaf    PSS_RollFileAppender.
///
/// @details  constructor
///
/// @author    Alexander Sacharov (AS)
/// Access     public
/// @return
/// @param     const fsns::wpath & filePath
/// @param     size_t maxFileSize
/// @param     unsigned int maxBackupIndex
/// @param     bool append
/// @param     int mode

fileroll_appender::fileroll_appender(std::shared_ptr<cppsl::log::appender> logPtr,
                                     const cppsl::file::fs::path   &filePath,
                                     size_t                          maxFileSize,
                                     unsigned int                    maxBackupIndex,
                                     bool                            append /*= true*/,
                                     int                             mode /*( _S_IREAD | _S_IWRITE ) */) :
   filebase_appender(logPtr, filePath, append, mode),
   m_maxBackupIndex(maxBackupIndex > 0 ? maxBackupIndex : 1),
   m_maxBackupIndexWidth((m_maxBackupIndex > 0) ? (unsigned short) log10((float) m_maxBackupIndex) + 1 : 1),
   m_maxFileSize(maxFileSize) {
}

/// @breaf    setMaxBackupIndex.
///
/// @details  set maximum backup index
///
/// @author    Alexander Sacharov (AS)
/// Access     virtual public
/// @return    void
/// Qualifier
/// @param     unsigned int maxBackups

void fileroll_appender::setMaxBackupIndex(unsigned int maxBackups) {
   m_maxBackupIndex = maxBackups;
   m_maxBackupIndexWidth = (m_maxBackupIndex > 0) ? (unsigned short) log10((float) m_maxBackupIndex) + 1 : 1;
}

unsigned int fileroll_appender::getMaxBackupIndex() const {
   return m_maxBackupIndex;
}

void fileroll_appender::setMaximumFileSize(size_t maxFileSize) {
   m_maxFileSize = maxFileSize;
}

size_t fileroll_appender::getMaxFileSize() const {
   return m_maxFileSize;
}

/// @breaf    rollOver.
///
/// @details  PSS_NAMESPACE::PSS_RollFileAppender::rollOver
///
/// @author    Alexander Sacharov (AS)
/// Access     virtual public
/// @return    void
/// Qualifier
void fileroll_appender::rollOver() {
   close(m_fd);
   if (m_maxBackupIndex > 0) {
      // stream
      std::ostringstream filename_stream;

      filename_stream << m_filePath.string() << "." << std::setw(m_maxBackupIndexWidth) << std::setfill(char('0')) << m_maxBackupIndex
                      << std::ends;

      // remove the very last (oldest) file
      std::string last_log_filename = filename_stream.str();
      remove(last_log_filename.c_str());

      // rename each existing file to the consequent one
      for (unsigned int i = m_maxBackupIndex; i > 1; i--) {
         filename_stream.str(std::string());
         // set padding so the files are listed in order
         filename_stream << m_filePath.string() << "." << std::setw(m_maxBackupIndexWidth) << std::setfill(char('0')) << i - 1 << std::ends;
         // rename file
         rename(filename_stream.str().c_str(), last_log_filename.c_str());
         last_log_filename = filename_stream.str();
      }

      // new file will be numbered 1
      rename(m_filePath.string().c_str(), last_log_filename.c_str());
   }
   m_fd = open(m_filePath.string().c_str(), m_flags, m_mode);
}

void fileroll_appender::writeMessage(const std::string &message) {
   filebase_appender::writeMessage(message);

   off_t offset = lseek(m_fd, 0, SEEK_END);
   if (offset < 0) {
      std::cerr << "append new event to log failed" << std::endl;
   }
   else {
      if (static_cast<size_t> (offset) >= m_maxFileSize) {
         rollOver();
      }
   }
}

