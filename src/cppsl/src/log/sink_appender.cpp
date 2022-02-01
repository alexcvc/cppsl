// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//
// Copyright(c) 2021 A.Sacharov.
// C++ wrapper class with  multiple loggers all sharing the same sink
//
// https://spdlog.docsforge.com/master/1.quickstart/#create-a-multiple-loggers-all-sharing-the-same-file-sink-aka-categories
// Docs Built May 30, 2021

/*************************************************************************//**
 * \file        logWriter.cpp
 * \brief       contains implementation log writer class.
 * \date        2021-07-28
 *****************************************************************************/

//auto pos = std::find(m_log_sinks.begin(), m_log_sinks.end(), [&name](const log_sink& x) { return x.m_logPtr->name().compare(name)==0;});

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/syslog_sink.h>
#include <filesystem>
#include <regex.h>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include <cppsl/log/sink_appender.h>
#include <cppsl/log/details/rsyslog_sink.h>

using namespace cppsl::log;
using namespace std::filesystem;
namespace filesys = std::filesystem;

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

/*************************************************************************//**
 * open logger with all added sinks
 * @param lev
 * @return true if successfully, otherwise - false
 */
bool SinkAppender::open_logger(loglevel lev) {

   try {
      m_logSp.reset();

      if( m_console_sink ) {
         if(m_logSp) m_logSp->sinks().push_back(m_console_sink);
         else m_logSp = std::make_shared<spdlog::logger>(m_name, m_console_sink);
      }

      if( m_file_sink ) {
         if(m_logSp) m_logSp->sinks().push_back(m_file_sink);
         else m_logSp = std::make_shared<spdlog::logger>(m_name, m_file_sink);
      }

      if( m_rotate_sink ) {
         if(m_logSp) m_logSp->sinks().push_back(m_rotate_sink);
         else m_logSp = std::make_shared<spdlog::logger>(m_name, m_rotate_sink);
      }

      if( m_daily_sink ) {
         if(m_logSp) m_logSp->sinks().push_back(m_daily_sink);
         else m_logSp = std::make_shared<spdlog::logger>(m_name, m_daily_sink);
      }

      if( m_syslog_sink ) {
         if(m_logSp) m_logSp->sinks().push_back(m_syslog_sink);
         else m_logSp = std::make_shared<spdlog::logger>(m_name, m_syslog_sink);
      }

      if( m_rsyslog_sink ) {
         if(m_logSp) m_logSp->sinks().push_back(m_rsyslog_sink);
         else m_logSp = std::make_shared<spdlog::logger>(m_name, m_rsyslog_sink);
      }

      if(!m_logSp)
         throw spdlog::spdlog_ex("Log pointer is null");

      m_logSp->set_level(lev);

      // or you can even set multi_sink logger as default logger
      if (m_console_sink) {
         spdlog::set_default_logger(std::make_shared<spdlog::logger>("console", m_console_sink));
         spdlog::set_level(m_console_sink->level());
      }

      spdlog::register_logger(m_logSp);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log open failed: " << ex.what() << std::endl;
      return false;
   }
}

bool SinkAppender::add_basic_file_sink(const std::string &filename, int truncate, loglevel lev) {
   try {
      // create path
      check_create_path(filename);

     m_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, truncate);
      m_file_sink->set_level(lev);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
   }
}

bool SinkAppender::add_rotation_file_sink(const std::string &filename, size_t max_file_size, size_t max_files, loglevel lev) {
   try {
      // create path
      check_create_path(filename);

     m_rotate_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, max_file_size, max_files);
      m_rotate_sink->set_level(lev);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
   }
}

bool SinkAppender::add_daily_file_sink(const std::string &filename, int hour, int minute, loglevel lev) {
   try {
      // create path
      check_create_path(filename);

     m_daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(filename, hour, minute);
      m_daily_sink->set_level(lev);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
   }
}

bool SinkAppender::add_console_sink(bool to_stderr, bool colored, loglevel lev) {
   try {

      if (!to_stderr && colored)
        m_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      else if (!to_stderr)
        m_console_sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
      else if (colored)
        m_console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
      else
        m_console_sink = std::make_shared<spdlog::sinks::stderr_sink_mt>();

      m_console_sink->set_level(lev);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
   }
}

bool SinkAppender::add_syslog_sink(const std::string &syslog_ident, int syslog_option, int syslog_facility,
                                   bool enable_formatting, loglevel lev) {
   try {

     m_syslog_sink = std::make_shared<spdlog::sinks::syslog_sink_mt>(syslog_ident, syslog_option, syslog_facility, enable_formatting);
      m_syslog_sink->set_level(lev);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
   }
}

bool SinkAppender::add_rsyslog_sink(const string &ident, const string &rsyslog_ip, int syslog_facility, loglevel lev,
                                    int port, bool enable_formatting, int log_buffer_max_size) {
   try {

     m_rsyslog_sink = std::make_shared<spdlog::sinks::rsyslog_sink_mt>(ident, rsyslog_ip, syslog_facility, log_buffer_max_size, port,
                                                                       enable_formatting);
      m_rsyslog_sink->set_pattern("[%Y-%m-%d %H:%M:%S:%e] [%n] [%l] [%P] %@ : %v");
      m_rsyslog_sink->set_level(lev);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
   }
}

void SinkAppender::drop(const std::vector<std::string> &logger_names) {
   for (auto &it : logger_names) {
      spdlog::drop(it);
   }
}

void SinkAppender::drop_all() {
   spdlog::drop(m_name);
}

/**
 * check existence and if needs - to create path
 * @param filename - file name
 */
void SinkAppender::check_create_path(const std::string &filename) {
   // check  create directory
   filesys::path f(filename);
   /// create path to log files
   if (!filesys::exists(f.parent_path())) {
      try {
         filesys::create_directories(f.parent_path());
      }
      catch (...) {
         std::cerr << "Create directories " << f.parent_path().string().c_str() << " fault" << std::endl;
      }
   }
}


