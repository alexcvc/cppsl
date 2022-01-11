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
#include <cppsl/log/appender.hpp>
#include <cppsl/log/details/rsyslog_sink.hpp>

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
bool appender::open_logger(loglevel lev) {

   try {
      logPtr_.reset();

      if( console_sink_ ) {
         if(logPtr_) logPtr_->sinks().push_back(console_sink_);
         else logPtr_ = std::make_shared<spdlog::logger>(name_, console_sink_);
      }

      if( file_sink_ ) {
         if(logPtr_) logPtr_->sinks().push_back(file_sink_);
         else logPtr_ = std::make_shared<spdlog::logger>(name_, file_sink_);
      }

      if( rotate_sink_ ) {
         if(logPtr_) logPtr_->sinks().push_back(rotate_sink_);
         else logPtr_ = std::make_shared<spdlog::logger>(name_, rotate_sink_);
      }

      if( daily_sink_ ) {
         if(logPtr_) logPtr_->sinks().push_back(daily_sink_);
         else logPtr_ = std::make_shared<spdlog::logger>(name_, daily_sink_);
      }

      if( syslog_sink_ ) {
         if(logPtr_) logPtr_->sinks().push_back(syslog_sink_);
         else logPtr_ = std::make_shared<spdlog::logger>(name_, syslog_sink_);
      }

      if( rsyslog_sink_ ) {
         if(logPtr_) logPtr_->sinks().push_back(rsyslog_sink_);
         else logPtr_ = std::make_shared<spdlog::logger>(name_, rsyslog_sink_);
      }

      if(!logPtr_)
         throw spdlog::spdlog_ex("Log pointer is null");

      logPtr_->set_level(lev);

      // or you can even set multi_sink logger as default logger
      if (console_sink_) {
         spdlog::set_default_logger(std::make_shared<spdlog::logger>("console", console_sink_));
         spdlog::set_level(console_sink_->level());
      }

      spdlog::register_logger(logPtr_);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log open failed: " << ex.what() << std::endl;
      return false;
   }
}

bool appender::add_basic_file_sink(const std::string &filename, int truncate, loglevel lev) {
   try {
      // create path
      check_create_path(filename);

      file_sink_ = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, truncate);
      file_sink_->set_level(lev);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
   }
}

bool appender::add_rotation_file_sink(const std::string &filename, size_t max_file_size, size_t max_files, loglevel lev) {
   try {
      // create path
      check_create_path(filename);

      rotate_sink_ = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, max_file_size, max_files);
      rotate_sink_->set_level(lev);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
   }
}

bool appender::add_daily_file_sink(const std::string &filename, int hour, int minute, loglevel lev) {
   try {
      // create path
      check_create_path(filename);

      daily_sink_ = std::make_shared<spdlog::sinks::daily_file_sink_mt>(filename, hour, minute);
      daily_sink_->set_level(lev);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
   }
}

bool appender::add_console_sink(bool to_stderr, bool colored, loglevel lev) {
   try {

      if (!to_stderr && colored)
         console_sink_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      else if (!to_stderr)
         console_sink_ = std::make_shared<spdlog::sinks::stdout_sink_mt>();
      else if (colored)
         console_sink_ = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
      else
         console_sink_ = std::make_shared<spdlog::sinks::stderr_sink_mt>();

      console_sink_->set_level(lev);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
   }
}

bool appender::add_syslog_sink(const std::string &syslog_ident, int syslog_option, int syslog_facility,
                             bool enable_formatting, loglevel lev) {
   try {

      syslog_sink_ = std::make_shared<spdlog::sinks::syslog_sink_mt>(syslog_ident, syslog_option, syslog_facility, enable_formatting);
      syslog_sink_->set_level(lev);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
   }
}

bool appender::add_rsyslog_sink(const string &ident, const string &rsyslog_ip,  int syslog_facility, loglevel lev,
                              int port, bool enable_formatting, int log_buffer_max_size) {
   try {

      rsyslog_sink_ = std::make_shared<spdlog::sinks::rsyslog_sink_mt>(ident, rsyslog_ip, syslog_facility, log_buffer_max_size, port,
                                                                      enable_formatting);
      rsyslog_sink_->set_pattern("[%Y-%m-%d %H:%M:%S:%e] [%n] [%l] [%P] %@ : %v");
      rsyslog_sink_->set_level(lev);

      return true;
   }
   catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
   }
}

void appender::drop(const std::vector<std::string> &logger_names) {
   for (auto &it : logger_names) {
      spdlog::drop(it);
   }
}

void appender::drop_all() {
   spdlog::drop(name_);
}

/**
 * check existence and if needs - to create path
 * @param filename - file name
 */
void appender::check_create_path(const std::string &filename) {
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


