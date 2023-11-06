//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************/ /**
 * @file
 * \brief   contains base file SinkAppender.
 * \author  Alexander Sacharov
 * \date    2022-01-10
 * \ingroup
 *****************************************************************************/

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <cppsl/file/fileBaseAppender.hpp>
#include <iostream>
#include <memory>

using namespace std;
using namespace cppsl;
using namespace cppsl::file;

//----------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Typedefs, enums, unions, variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Functions Prototypes
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Function Definitions
//----------------------------------------------------------------------------

FileBaseAppender::FileBaseAppender(cppsl::log::log_appenderPtr logPtr, const std::filesystem::path &filePath,
                                   bool append, std::ios_base::openmode mode)
   : cppsl::log::BaseWithLogAppender(logPtr), m_filePath(filePath), m_mode(mode) {
  if (!append) {
    mode |= std::ios::trunc;
  } else {
    mode &= (std::ios_base::app | std::ios_base::ate);
  }

  // create path to log files
  if (!std::filesystem::exists(filePath.parent_path())) {
    try {
      std::filesystem::create_directories(filePath.parent_path());
    } catch (...) {
      m_logPtr->error("create directories {} failed", filePath.parent_path().string());
    }
  }

  if (!std::filesystem::exists(filePath)) {
    try {
      std::ofstream{filePath};
    } catch (...) {
      m_logPtr->error("create file {} failed", filePath.string());
    }
  }

  m_fs.open(m_filePath.string(), m_mode);
  if (!m_fs.is_open()) {
    m_logPtr->error("open file {} failed", m_filePath.string());
  }
}

void FileBaseAppender::closeFile() {
  if (m_fs.is_open()) {
    m_fs.close();
  }
}

void FileBaseAppender::setAppend(bool append) {
  if (append) {
    m_mode &= ~std::ios_base::trunc;
  } else {
    m_mode |= std::ios_base::trunc;
  }
}

bool FileBaseAppender::getAppend() const {
  return (m_mode & std::ios_base::trunc) == 0;
}

void FileBaseAppender::setMode(std::ios_base::openmode mode) {
  m_mode = mode;
}

std::ios_base::openmode FileBaseAppender::getMode() const {
  return m_mode;
}

/// append new event
bool FileBaseAppender::writeMessage(std::string_view message) {
  if (m_fs.is_open()) {
    if (!m_fs.write(message.data(), message.length())) {
      m_logPtr->warn("write message to event-Log file failed");
    } else {
      return true;
    }
  } else {
    m_logPtr->warn("attempt to write to closed stream: {}", m_filePath.string());
  }
  return false;
}

/// reopen file
bool FileBaseAppender::reopenFile() {
  if (!m_filePath.empty()) {
    closeFile();
    m_fs.open(m_filePath.string(), m_mode);
    if (!m_fs.is_open()) {
      m_logPtr->error("reopen file failure. File {}", m_filePath.string());
      return false;
    } else {
      return true;
    }
  } else {
    return true;
  }
}
