//
// Copyright (c) 2022 Alexander Sacharov <a.sacharov@gmx.de>
//               All rights reserved.
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * \file    baseAppender.cpp
 * \brief   contains base file appender.
 * \author  Alexander Sacharov
 * \date    2022-01-10
 * \ingroup
 *****************************************************************************/

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <memory>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include <cppsl/file/baseAppender.hpp>

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

filebase_appender::filebase_appender(std::shared_ptr<cppsl::log::appender> logPtr, const cppsl::file::fs::path &filePath,
                                     bool append, int mode)
   : cppsl::log::anylog(logPtr), m_filePath(filePath), m_flags(O_CREAT | O_APPEND | O_WRONLY), m_mode(mode) {
   if (!append) {
      m_flags |= O_TRUNC;
   }

   /// create path to log files
   if (!cppsl::file::fs::exists(filePath.parent_path())) {
      try {
         cppsl::file::fs::create_directories(filePath.parent_path());
      }
      catch (...) {
         cerr << "Create directories " << filePath.parent_path().string().c_str() << " fault" << endl;
      }
   }
   m_fd = open(m_filePath.string().c_str(), m_flags, m_mode);
   if (m_fd == -1) {
      cerr << "Open file " << m_filePath.string().c_str() << " fault" << endl;
   }
}

filebase_appender::~filebase_appender() { closeFile(); }

void filebase_appender::closeFile() {
   if (m_fd != -1) {
      close(m_fd);
      m_fd = -1;
   }
}

void filebase_appender::setAppend(bool append) {
   if (append) {
      m_flags &= ~O_TRUNC;
   }
   else {
      m_flags |= O_TRUNC;
   }
}

bool filebase_appender::getAppend() const { return (m_flags & O_TRUNC) == 0; }

void filebase_appender::setMode(int mode) { m_mode = mode; }

int filebase_appender::getMode() const { return m_mode; }

/// append new event
void filebase_appender::writeMessage(const std::string &message) {
   if (m_fd != -1) {
      if (!write(m_fd, message.data(), (unsigned int) message.length())) {
         cerr << "Write message to event-Log file failed" << endl;
      }
   }
   else {
      cerr << "Attempt to write to closed stream: " << m_filePath.string().c_str() << endl;
   }
}

/// reopen file
bool filebase_appender::reopenFile() {
   if (!m_filePath.empty()) {
      int fd = open(m_filePath.string().c_str(), m_flags, m_mode);
      if (fd < 0) {
         wcerr << "reopen file failure. File " << m_filePath.string().c_str() << endl;
         m_fd = -1;
         return false;
      }
      else {
         if (m_fd != -1) {
            close(m_fd);
         }
         m_fd = fd;
         return true;
      }
   }
   else {
      return true;
   }
}
