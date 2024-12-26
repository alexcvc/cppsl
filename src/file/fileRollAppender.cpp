//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************/ /**
* @file
* @brief   rolling file appender that rolls over the logfile once it has
* reached a certain size limit.
* @author  Alexander Sacharov <a.sacharov@asoft-labs.de>
* @ingroup COM4CPP Library
*****************************************************************************/

#include "cppsl/file/fileRollAppender.hpp"
#include <math.h>
#include <iomanip>
#include <iostream>

#include "spdlog/spdlog.h"

using namespace std;
using namespace cppsl;
using namespace cppsl::file;

FileRollAppender::FileRollAppender(const filesystem::path& filePath, bool append, ios_base::openmode mode)
    : FileBaseAppender(filePath, append, mode),
      m_maxBackupIndex(maxRollFileAppenderBackIndex > 0 ? maxRollFileAppenderBackIndex : 1),
      m_maxBackupIndexWidth((m_maxBackupIndex > 0) ? (unsigned short)log10((float)m_maxBackupIndex) + 1 : 1),
      m_maxFileSize(maxRollFileAppenderSize) {}

FileRollAppender::FileRollAppender(const std::filesystem::path& filePath, size_t maxFileSize, uint maxBackupIndex,
                                   bool append, std::ios_base::openmode mode)
    : FileBaseAppender(filePath, append, mode),
      m_maxBackupIndex(maxBackupIndex > 0 ? maxBackupIndex : 1),
      m_maxBackupIndexWidth((m_maxBackupIndex > 0) ? (unsigned short)log10((float)m_maxBackupIndex) + 1 : 1),
      m_maxFileSize(maxFileSize) {}

void FileRollAppender::setMaxBackupIndex(unsigned int maxBackups) {
  m_maxBackupIndex = maxBackups;
  m_maxBackupIndexWidth = (m_maxBackupIndex > 0) ? (unsigned short)log10((float)m_maxBackupIndex) + 1 : 1;
}

unsigned int FileRollAppender::getMaxBackupIndex() const {
  return m_maxBackupIndex;
}

void FileRollAppender::setMaximumFileSize(size_t maxFileSize) {
  m_maxFileSize = maxFileSize;
}

size_t FileRollAppender::getMaxFileSize() const {
  return m_maxFileSize;
}

void FileRollAppender::rollOver() {
  // close file
  closeFile();

  if (m_maxBackupIndex > 0) {
    // stream
    std::ostringstream filename_stream;

    filename_stream << m_filePath.string() << "." << std::setw(m_maxBackupIndexWidth) << std::setfill(char('0'))
                    << m_maxBackupIndex << std::ends;

    // remove the very last (oldest) file
    std::string last_log_filename = filename_stream.str();
    std::filesystem::remove(last_log_filename);

    // rename each existing file to the consequent one
    for (auto i = m_maxBackupIndex; i > 1; i--) {
      filename_stream.str(std::string());
      // set padding so the files are listed in order
      filename_stream << m_filePath.string() << "." << std::setw(m_maxBackupIndexWidth) << std::setfill(char('0'))
                      << i - 1 << std::ends;
      // rename file
      if (std::filesystem::exists(filename_stream.str())) {
        std::filesystem::rename(filename_stream.str(), last_log_filename);
      }
      last_log_filename = filename_stream.str();
    }

    // new file will be numbered 1
    std::filesystem::rename(m_filePath.string().c_str(), last_log_filename.c_str());
  }

  if (!std::filesystem::exists(m_filePath.string())) {
    try {
      std::ofstream{m_filePath.string()};
    } catch (...) {
      spdlog::error("create file {} failed", m_filePath.string());
    }
  }

  m_fs.open(m_filePath.string(), m_mode);
}

bool FileRollAppender::writeMessage(std::string_view message) {
  auto res = FileBaseAppender::writeMessage(message);

  if (res) {
    off_t offset = m_fs.tellg();
    if (offset < 0) {
      spdlog::error("append new event to log failed");
    } else {
      if (static_cast<size_t>(offset) >= m_maxFileSize) {
        rollOver();
      }
    }
  }
  return res;
}
