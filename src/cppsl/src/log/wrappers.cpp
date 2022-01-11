// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//
// Copyright(c) 2017 zhishupp.
// <spdlog> C wrapper for support: __FUNCTION__,  __FILE__, __LINE__
//
// Copyright(c) 2021 A.Sacharov.
// C wrapper function with log4c prefix

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <string>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/syslog_sink.h>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include "cppsl/log/wrappers.h"

//----------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------
#define MSG_BUF_MAX_LEN       2048
#define LOG_SUFFIX_STR        " [{}|{}:{}]"
#define DO_UNBRACE_MSG        1

//----------------------------------------------------------------------------
// Typedefs, enums, unions, variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Declarations
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Local Function Definitions
//----------------------------------------------------------------------------

/*************************************************************************//**
 * extract name from file
 * @param file
 * @return name
 */
static std::string extract_name(const char *file) {
   std::string name = file;
   std::string::size_type pos = name.find_last_of("/\\");

   return (pos != std::string::npos) ? name.substr(pos + 1) : name;
}

/*************************************************************************//**
 * replace all in string
 * @param str
 * @param src
 * @param dst
 */
static void replace_all(std::string &str, const std::string &src, const std::string &dst) {
   std::string::size_type pos = 0;
   std::string::size_type src_size = src.size();
   std::string::size_type dst_size = dst.size();
   while ((pos = str.find(src, pos)) != std::string::npos) {
      str.replace(pos, src_size, dst);
      pos += dst_size;
   }
}

/*************************************************************************//**
 * unbrace  {{ and }}
 * @param msg
 * @return new string
 */
 static std::string &unbrace(std::string &msg) {
   replace_all(msg, "{", "{{");
   replace_all(msg, "}", "}}");
   return msg;
}

//----------------------------------------------------------------------------
// Global Function Definitions
//----------------------------------------------------------------------------

/*************************************************************************//**
 * @brief get/find logger
 * @param logger_name
 * @return true if successfully found, otherwise - false
 */
bool log4c_get(const char *logger_name) {
   return (spdlog::get(logger_name) != nullptr);
}

/*************************************************************************//**
 * @brief Set global formatting. Example: spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e %l : %v");
 * @param pattern
 */
void log4c_set_pattern(const char *pattern) {
   spdlog::set_pattern(pattern);
}

/*************************************************************************//**
 * Set global logging level
 * @param log_level
 */
void log4c_set_level(int log_level) {
   spdlog::set_level(static_cast<spdlog::level::level_enum>(log_level));
}

/*************************************************************************//**
 * Create and register multi/single threaded basic file logger.
 * Basic logger simply writes to given file without any limitations or rotations.
 * @param logger_name
 * @param filename
 * @param truncate
 */
void log4c_basic_logger_mt(const char *logger_name, const char *filename, int truncate) {
   spdlog::basic_logger_mt(logger_name, filename, truncate);
}

//-----------------------------------------------------------------------------
void log4c_basic_logger_st(const char *logger_name, const char *filename, int truncate) {
   spdlog::basic_logger_st(logger_name, filename, truncate);
}

/*************************************************************************//**
 * Create and register multi/single threaded rotating file logger.
 * @param logger_name
 * @param filename
 * @param max_file_size
 * @param max_files
 */
void log4c_rotating_logger_mt(const char *logger_name, const char *filename, size_t max_file_size, size_t max_files) {
   spdlog::rotating_logger_mt(logger_name, filename, max_file_size, max_files);
}

//-----------------------------------------------------------------------------
void log4c_rotating_logger_st(const char *logger_name, const char *filename, size_t max_file_size, size_t max_files) {
   spdlog::rotating_logger_st(logger_name, filename, max_file_size, max_files);
}

/*************************************************************************//**
 * Create file logger which creates new file on the given time (default in midnight)
 * @param logger_name
 * @param filename
 * @param hour
 * @param minute
 */
void log4c_daily_logger_mt(const char *logger_name, const char *filename, int hour, int minute) {
   spdlog::daily_logger_mt(logger_name, filename, hour, minute);
}

//-----------------------------------------------------------------------------
void log4c_daily_logger_st(const char *logger_name, const char *filename, int hour, int minute) {
   spdlog::daily_logger_st(logger_name, filename, hour, minute);
}

/*************************************************************************//**
 * @brief Create and register stdout/stderr loggers
 * @param logger_name
 */
void log4c_stdout_logger_mt(const char *logger_name) {
   spdlog::stdout_logger_mt(logger_name);
}

//-----------------------------------------------------------------------------
void log4c_stdout_logger_st(const char *logger_name) {
   spdlog::stdout_logger_st(logger_name);
}

//-----------------------------------------------------------------------------
void log4c_stderr_logger_mt(const char *logger_name) {
   spdlog::stderr_logger_mt(logger_name);
}

//-----------------------------------------------------------------------------
void log4c_stderr_logger_st(const char *logger_name) {
   spdlog::stderr_logger_st(logger_name);
}

/*************************************************************************//**
 * @brief Create and register colored stdout/stderr loggers
 * @param logger_name
 */
void log4c_stdout_color_mt(const char *logger_name) {
   spdlog::stdout_color_mt(logger_name);
}

//-----------------------------------------------------------------------------
void log4c_stdout_color_st(const char *logger_name) {
   spdlog::stdout_color_st(logger_name);
}

//-----------------------------------------------------------------------------
void log4c_stderr_color_mt(const char *logger_name) {
   spdlog::stderr_color_mt(logger_name);
}

//-----------------------------------------------------------------------------
void log4c_stderr_color_st(const char *logger_name) {
   spdlog::stderr_color_st(logger_name);
}

/*************************************************************************//**
 * Create and register a syslog logger
 * @param logger_name
 * @param ident
 * @param syslog_option
 */
void log4c_syslog_logger(const char *logger_name, const char *ident, int syslog_option) {

   spdlog::syslog_logger_mt(logger_name, ident, syslog_option);

}

/*************************************************************************//**
 * Drop the reference to the given logger
 * @param logger_name
 */
void log4c_drop(const char *logger_name) {
   spdlog::drop(logger_name);
}

/*************************************************************************//**
 * @brief Drop all references from the registry
 */
void log4c_drop_all() {
   spdlog::drop_all();
}

/*************************************************************************//**
 * set level for logger
 * @param logger_name
 * @param log_level
 */
void log4c_set_logger_level(const char *logger_name, int log_level) {
   std::shared_ptr<spdlog::logger> logger_ptr = spdlog::get(logger_name);
   if (logger_ptr != nullptr) {
      logger_ptr->set_level(static_cast<spdlog::level::level_enum>(log_level));
   }
}

/*************************************************************************//**
 * get level for logger
 * @param logger_name
 * @return
 */
int log4c_get_logger_level(const char *logger_name) {
   std::shared_ptr<spdlog::logger> logger_ptr = spdlog::get(logger_name);
   if (logger_ptr != nullptr) {
      return static_cast<int>(logger_ptr->level());
   }

   return -1;
}

/*************************************************************************//**
 * Set a logger's pattern
 * @param logger_name
 * @param format_string
 */
void log4c_set_logger_pattern(const char *logger_name, const char *format_string) {
   std::shared_ptr<spdlog::logger> logger_ptr = spdlog::get(logger_name);
   if (logger_ptr != nullptr) {
      return logger_ptr->set_pattern(format_string);
   }
}

/*************************************************************************//**
 * Set a logger's flush on
 * @param logger_name
 * @param log_level
 */
void log4c_logger_flush_on(const char *logger_name, int log_level) {
   std::shared_ptr<spdlog::logger> logger_ptr = spdlog::get(logger_name);
   if (logger_ptr != nullptr) {
      logger_ptr->flush_on(static_cast<spdlog::level::level_enum>(log_level));
   }
}

/*************************************************************************//**
 * Flush a logger
 * @param logger_name
 */
void log4c_logger_flush(const char *logger_name) {
   std::shared_ptr<spdlog::logger> logger_ptr = spdlog::get(logger_name);
   if (logger_ptr != nullptr) {
      logger_ptr->flush();
   }
}

/*************************************************************************//**
 * Logger logging message
 * @param logger_name 
 * @param log_level 
 * @param msg 
 */
void log4c_logger_log(const char *logger_name, int log_level, const char *msg) {
   std::shared_ptr<spdlog::logger> logger_ptr = spdlog::get(logger_name);
   if (logger_ptr != nullptr) {
      if (!logger_ptr->should_log(static_cast<spdlog::level::level_enum>(log_level))) return;

#ifdef DO_UNBRACE_MSG
      std::string str_msg = msg;
      str_msg = unbrace(str_msg);
      logger_ptr->log(static_cast<spdlog::level::level_enum>(log_level), str_msg);
#else
      logger_ptr->log(static_cast<spdlog::level::level_enum>(log_level), msg);
#endif
   }
}

/*************************************************************************//**
 * Logger logging variadic message
 * @param logger_name 
 * @param log_level 
 * @param fmt 
 * @param ... 
 */
void log4c_logger_log_var(const char *logger_name, int log_level, const char *fmt, ...) {
   std::shared_ptr<spdlog::logger> logger_ptr = spdlog::get(logger_name);
   if (logger_ptr != nullptr) {
      if (!logger_ptr->should_log(static_cast<spdlog::level::level_enum>(log_level))) return;

      char msg[MSG_BUF_MAX_LEN];

      va_list args;
      va_start(args, fmt);
      vsnprintf(msg, sizeof(msg), fmt, args);
      va_end(args);

#ifdef DO_UNBRACE_MSG
      std::string str_msg = msg;
      str_msg = unbrace(str_msg);
      logger_ptr->log(static_cast<spdlog::level::level_enum>(log_level), str_msg);
#else
      logger_ptr->log(static_cast<spdlog::level::level_enum>(log_level), msg);
#endif
   }
}

/*************************************************************************//**
 * Logger logging message with __FUNCTION__,  __FILE__, __LINE__
 * @param logger_name 
 * @param log_level 
 * @param func 
 * @param file 
 * @param line 
 * @param error_code 
 * @param msg 
 */
void log4c_logger_log_ffl(const char *logger_name, int log_level, const char *func, const char *file, size_t line, int error_code,
                          const char *msg) {
   std::shared_ptr<spdlog::logger> logger_ptr = spdlog::get(logger_name);
   if (logger_ptr != nullptr) {
      if (!logger_ptr->should_log(static_cast<spdlog::level::level_enum>(log_level))) return;

      std::string str_msg = msg;
#ifdef DO_UNBRACE_MSG
      str_msg = unbrace(str_msg);
#endif
      str_msg = ((error_code != LOG4C_SUCCESS) ? ("ErrorCode: " + std::to_string(error_code) + ", ") : "") + str_msg + LOG_SUFFIX_STR;

      logger_ptr->log(static_cast<spdlog::level::level_enum>(log_level), str_msg.c_str(), func, extract_name(file).c_str(), line);
   }
}

/*************************************************************************//**
 * Logger logging variadic message with __FUNCTION__,  __FILE__, __LINE__
 * @param logger_name 
 * @param log_level 
 * @param func 
 * @param file 
 * @param line 
 * @param error_code 
 * @param fmt 
 * @param ... 
 */
void log4c_logger_log_ffl_var(const char *logger_name, int log_level, const char *func, const char *file, size_t line, int error_code,
                              const char *fmt, ...) {
   std::shared_ptr<spdlog::logger> logger_ptr = spdlog::get(logger_name);
   if (logger_ptr != nullptr) {
      if (!logger_ptr->should_log(static_cast<spdlog::level::level_enum>(log_level))) return;

      char msg[MSG_BUF_MAX_LEN];

      va_list args;
      va_start(args, fmt);
      vsnprintf(msg, sizeof(msg), fmt, args);
      va_end(args);

      std::string str_msg = msg;
#ifdef DO_UNBRACE_MSG
      str_msg = unbrace(str_msg);
#endif
      str_msg = ((error_code != LOG4C_SUCCESS) ? ("ErrorCode: " + std::to_string(error_code) + ", ") : "") + str_msg + LOG_SUFFIX_STR;

      logger_ptr->log(static_cast<spdlog::level::level_enum>(log_level), str_msg.c_str(), func, extract_name(file).c_str(), line);
   }
}
