// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//
// Copyright(c) 2021 A.Sacharov.
// C++ wrapper class with  multiple loggers all sharing the same sink
//
// https://spdlog.docsforge.com/master/1.quickstart/#create-a-multiple-loggers-all-sharing-the-same-file-sink-aka-categories
// Docs Built May 30, 2021

/*************************************************************************/ /**
 * @file    writer.hh
 * \brief  C++ wrapper class with  multiple loggers all sharing the same sink
 * aka categories
 *
 * \author  AS
 * \date    2020-07-03
 *****************************************************************************/

#ifndef __INCLUDE_CPPSL_LOG_SINK_APPENDER_H__
#define __INCLUDE_CPPSL_LOG_SINK_APPENDER_H__

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <syslog.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/null_sink.h>
#include <fmt/format.h>

using std::shared_ptr;
using std::string;
using loglevel = spdlog::level::level_enum;

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

namespace cppsl::log {
   /**
    * Class Log implements logger in  application
    *
    * Class can  support up to three multi-sinks in the logger
    */
   class SinkAppender {

      std::string                     m_name;
      std::shared_ptr<spdlog::logger> m_logSp;
      spdlog::sink_ptr                m_console_sink;
      spdlog::sink_ptr                m_file_sink;
      spdlog::sink_ptr                m_daily_sink;
      spdlog::sink_ptr                m_rotate_sink;
      spdlog::sink_ptr                m_syslog_sink;
      spdlog::sink_ptr                m_rsyslog_sink;

    public:
      /**
       * default constructor
       * @param name - logging name
       */
      explicit SinkAppender(const std::string &name) : m_name(name) {}

      /**
       * destructor
       */
      ~SinkAppender() { drop_all(); }

      /**
       * Interval based flush. This is implemented by a single worker thread that periodically
       * calls flush() on each logger.
       * @param sec
       */
      void flush_every(std::chrono::seconds &sec) {
        spdlog::flush_every(std::chrono::seconds(sec));
      }

      /**
       * add basic file sink + multi sinks.
       * Basic logger simply writes to given file without any limitations or rotations.
       * @param multi_sinks_desc
       * @param filename
       * @param truncate
       * @param level
       */
      bool add_basic_file_sink(const std::string &filename, int truncate, loglevel lev);

      /**
       * add rotation file sink.
       * Such logger simply writes to given file with rotations.
       * @param filename
       * @param max_file_size
       * @param max_files
       * @param level
       */
      bool add_rotation_file_sink(const std::string &filename, size_t max_file_size, size_t max_files, loglevel lev);

      /**
       * Create file sink which create new file on the given time (default in midnight)
       * @param filename
       * @param hour
       * @param minute
       * @param level
       */
      bool add_daily_file_sink(const std::string &filename, int hour, int minute, loglevel lev);

      /**
       * Create and register no-/colored console sink
       * @param to_stderr
       * @param colored
       * @param level
       */
      bool add_console_sink(bool to_stderr, bool colored, loglevel lev);

      /**
       * Create and register a rsyslog sinks to remote server
       * @param ident
       * @param rsyslog_ip
       * @param facility
       * @param severity
       * @param lev
       * @param port
       * @param enable_formatting
       * @param log_buffer_max_size
       */
      bool add_rsyslog_sink(const std::string &ident, const std::string &rsyslog_ip,
                            int syslog_facility, loglevel lev,
                            int port = 514, bool enable_formatting = true,
                            int log_buffer_max_size = 1024 * 1024 * 16);

      /**
       * set pattern  for remote syslog
       * Default is: [%Y-%m-%d %H:%M:%S:%e] [%n] [%l] [%P] %@ : %v
       * @param pattern
       */
      void set_pattern_rsyslog_sink(const std::string &pattern) {
        if (m_rsyslog_sink) m_rsyslog_sink->set_pattern(pattern);
      }

      /**
       * Create and register a syslog sinks
       * @param syslog_ident
       * @param syslog_option
       * @param syslog_facility
       * @param enable_formatting
       * @param level
       */
      bool add_syslog_sink(const std::string &syslog_ident, int syslog_option, int syslog_facility,
                           bool enable_formatting, loglevel lev);

      /**
       * open logger with all added sinks
       * @param lev
       * @return true if successfully, otherwise - false
       */
      bool open_logger(loglevel lev);

      /**
       * Drop the reference to the given logger
       * @param logger_name
       */
      void drop(const std::vector<std::string> &logger_names);

      /**
       * Drop all references from the registry
       */
      void drop_all();

      /**
       * Get a logger's level
       * @param logger_name
       * @return
       */
      spdlog::level::level_enum level() {
        if (m_logSp) return m_logSp->level();
        else return spdlog::level::level_enum::off;
      }

      spdlog::level::level_enum level_console_sink() {
        if (m_console_sink) return m_console_sink->level();
        else return spdlog::level::level_enum::off;
      }

      void set_level_console_sink(spdlog::level::level_enum lev) {
        if (m_console_sink) m_console_sink->set_level(lev);
      }

      spdlog::level::level_enum level_base_file_sink() {
        if (m_file_sink) return m_file_sink->level();
        else return spdlog::level::level_enum::off;
      }

      void set_level_base_file_sink(spdlog::level::level_enum lev) {
        if (m_file_sink) m_file_sink->set_level(lev);
      }

      spdlog::level::level_enum level_daily_sink() {
        if (m_daily_sink) return m_daily_sink->level();
        else return spdlog::level::level_enum::off;
      }

      void set_level_daily_sink(spdlog::level::level_enum lev) {
        if (m_daily_sink) m_daily_sink->set_level(lev);
      }

      spdlog::level::level_enum level_rotation_file_sink() {
        if (m_rotate_sink) return m_rotate_sink->level();
        else return spdlog::level::level_enum::off;
      }

      void set_level_rotation_file_sink(spdlog::level::level_enum lev) {
        if (m_rotate_sink) m_rotate_sink->set_level(lev);
      }

      spdlog::level::level_enum level_syslog_sink() {
        if (m_syslog_sink) return m_syslog_sink->level();
        else return spdlog::level::level_enum::off;
      }

      void set_level_syslog_sink(spdlog::level::level_enum lev) {
        if (m_syslog_sink) m_syslog_sink->set_level(lev);
      }

      spdlog::level::level_enum level_rsyslog_sink() {
        if (m_rsyslog_sink) return m_rsyslog_sink->level();
        else return spdlog::level::level_enum::off;
      }

      void set_level_rsyslog_sink(spdlog::level::level_enum lev) {
        if (m_rsyslog_sink) m_rsyslog_sink->set_level(lev);
      }

      /**
       * set level only for logger
       * @param lev
       */
      void set_level(spdlog::level::level_enum lev) {
        if (m_logSp) {
          m_logSp->set_level(lev);
        }
      }

      /**
       * set level for all  flags
       * @param lev
       */
      void set_level_all(spdlog::level::level_enum lev) {
        set_level_console_sink(lev);
        set_level_base_file_sink(lev);
        set_level_daily_sink(lev);
        set_level_rotation_file_sink(lev);
        set_level_syslog_sink(lev);
        set_level_syslog_sink(lev);
      }

    private:
      /**
       * check existence and if needs - to create path
       * @param filename - file name
       */
      void check_create_path(const std::string &filename);

    public:

      /*
       * Template functions
       */

      //----------------------------------------------------------
      // trace
      //----------------------------------------------------------
      template<typename... Args>
      inline void trace(fmt::basic_string_view<char> fmt, const Args &...args) {
        if (m_logSp) {
          m_logSp->trace(fmt, args...);
        }
      }

      template<typename T>
      inline void trace(const T &msg) {
        if (m_logSp) {
          m_logSp->trace(msg);
        }
      }

      template<typename... Args>
      inline void trace_if(bool flag, fmt::basic_string_view<char> fmt, const Args &...args) {
        if (flag) {
          if (m_logSp) {
            m_logSp->trace(fmt, args...);
          }
        }
      }

      template<typename T>
      inline void trace_if(bool flag, const T &msg) {
        if (flag) {
          if (m_logSp) {
            m_logSp->trace(msg);
          }
        }
      }

      template<typename... Args>
      inline void trace_if_verbose(bool if_verbose, fmt::basic_string_view<char> fmt, const Args &...args) {
        if (if_verbose) {
          if (m_logSp) {
            m_logSp->trace(fmt, args...);
          }
        }
      }

      template<typename T>
      inline void trace_if_verbose(bool if_verbose, const T &msg) {
        if (if_verbose) {
          if (m_logSp) {
            m_logSp->trace(msg);
          }
        }
      }

      template<typename... Args>
      inline void trace_name(const std::string &name, fmt::basic_string_view<char> fmt, const Args &...args) {
        auto logPtr = spdlog::get(name);
        if (logPtr) {
          logPtr->trace(fmt, args...);
        }
      }

      template<typename T>
      inline void trace_name(const std::string &name, const T &msg) {
        std::shared_ptr<spdlog::logger> logPtr = spdlog::get(name);
        if (logPtr) {
          // logging to multi-sink logger
          logPtr->trace(msg);
        }
      }

      //----------------------------------------------------------
      // debug
      //----------------------------------------------------------
      template<typename... Args>
      inline void debug(fmt::basic_string_view<char> fmt, const Args &...args) {
        if (m_logSp) {
          m_logSp->debug(fmt, args...);
        }
      }

      template<typename T>
      inline void debug(const T &msg) {
        if (m_logSp) {
          m_logSp->debug(msg);
        }
      }

      template<typename... Args>
      inline void debug_if(bool flag, fmt::basic_string_view<char> fmt, const Args &...args) {
        if (flag) {
          if (m_logSp) {
            m_logSp->debug(fmt, args...);
          }
        }
      }

      template<typename T>
      inline void debug_if(bool flag, const T &msg) {
        if (flag) {
          if (m_logSp) {
            m_logSp->debug(msg);
          }
        }
      }

      template<typename... Args>
      inline void debug_name(const std::string &name, fmt::basic_string_view<char> fmt, const Args &...args) {
        auto logPtr = spdlog::get(name);
        if (logPtr) {
          logPtr->debug(fmt, args...);
        }
      }

      template<typename T>
      inline void debug_name(const std::string &name, const T &msg) {
        std::shared_ptr<spdlog::logger> logPtr = spdlog::get(name);
        if (logPtr) {
          // logging to multi-sink logger
          logPtr->debug(msg);
        }
      }

      //----------------------------------------------------------
      // info
      //----------------------------------------------------------
      template<typename... Args>
      inline void info(fmt::basic_string_view<char> fmt, const Args &...args) {
        if (m_logSp) {
          m_logSp->info(fmt, args...);
        }
      }

      template<typename T>
      inline void info(const T &msg) {
        if (m_logSp) {
          m_logSp->info(msg);
        }
      }

      template<typename... Args>
      inline void info_if(bool flag, fmt::basic_string_view<char> fmt, const Args &...args) {
        if (flag) {
          if (m_logSp) {
            m_logSp->info(fmt, args...);
          }
        }
      }

      template<typename T>
      inline void info_if(bool flag, const T &msg) {
        if (flag) {
          if (m_logSp) {
            m_logSp->info(msg);
          }
        }
      }

      template<typename... Args>
      inline void info_name(const std::string &name, fmt::basic_string_view<char> fmt, const Args &...args) {
        auto logPtr = spdlog::get(name);
        if (logPtr) {
          logPtr->info(fmt, args...);
        }
      }

      template<typename T>
      inline void info_name(const std::string &name, const T &msg) {
        std::shared_ptr<spdlog::logger> logPtr = spdlog::get(name);
        if (logPtr) {
          // logging to multi-sink logger
          logPtr->info(msg);
        }
      }

      //----------------------------------------------------------
      // warn
      //----------------------------------------------------------
      template<typename... Args>
      inline void warn(fmt::basic_string_view<char> fmt, const Args &...args) {
        if (m_logSp) {
          m_logSp->warn(fmt, args...);
        }
      }

      template<typename T>
      inline void warn(const T &msg) {
        if (m_logSp) {
          m_logSp->warn(msg);
        }
      }

      template<typename... Args>
      inline void warn_if(bool flag, fmt::basic_string_view<char> fmt, const Args &...args) {
        if (flag) {
          if (m_logSp) {
            m_logSp->warn(fmt, args...);
          }
        }
      }

      template<typename T>
      inline void warn_if(bool flag, const T &msg) {
        if (flag) {
          if (m_logSp) {
            m_logSp->warn(msg);
          }
        }
      }

      template<typename... Args>
      inline void warn_name(const std::string &name, fmt::basic_string_view<char> fmt, const Args &...args) {
        auto logPtr = spdlog::get(name);
        if (logPtr) {
          logPtr->warn(fmt, args...);
        }
      }

      template<typename T>
      inline void warn_name(const std::string &name, const T &msg) {
        std::shared_ptr<spdlog::logger> logPtr = spdlog::get(name);
        if (logPtr) {
          // logging to multi-sink logger
          logPtr->warn(msg);
        }
      }

      //----------------------------------------------------------
      // error
      //----------------------------------------------------------
      template<typename... Args>
      inline void error(fmt::basic_string_view<char> fmt, const Args &...args) {
        if (m_logSp) {
          m_logSp->error(fmt, args...);
        }
      }

      template<typename T>
      inline void error(const T &msg) {
        if (m_logSp) {
          m_logSp->error(msg);
        }
      }

      template<typename... Args>
      inline void error_if(bool flag, fmt::basic_string_view<char> fmt, const Args &...args) {
        if (flag) {
          if (m_logSp) {
            m_logSp->error(fmt, args...);
          }
        }
      }

      template<typename T>
      inline void error_if(bool flag, const T &msg) {
        if (flag) {
          if (m_logSp) {
            m_logSp->error(msg);
          }
        }
      }

      template<typename... Args>
      inline void error_name(const std::string &name, fmt::basic_string_view<char> fmt, const Args &...args) {
        auto logPtr = spdlog::get(name);
        if (logPtr) {
          logPtr->error(fmt, args...);
        }
      }

      template<typename T>
      inline void error_name(const std::string &name, const T &msg) {
        std::shared_ptr<spdlog::logger> logPtr = spdlog::get(name);
        if (logPtr) {
          // logging to multi-sink logger
          logPtr->error(msg);
        }
      }

      //----------------------------------------------------------
      // critical
      //----------------------------------------------------------
      template<typename... Args>
      inline void critical(fmt::basic_string_view<char> fmt, const Args &...args) {
        if (m_logSp) {
          m_logSp->critical(fmt, args...);
        }
      }

      template<typename T>
      inline void critical(const T &msg) {
        if (m_logSp) {
          m_logSp->critical(msg);
        }
      }

      template<typename... Args>
      inline void critical_if(bool flag, fmt::basic_string_view<char> fmt, const Args &...args) {
        if (flag) {
          if (m_logSp) {
            m_logSp->critical(fmt, args...);
          }
        }
      }

      template<typename T>
      inline void critical_if(bool flag, const T &msg) {
        if (flag) {
          if (m_logSp) {
            m_logSp->critical(msg);
          }
        }
      }

      template<typename... Args>
      inline void critical_name(const std::string &name, fmt::basic_string_view<char> fmt, const Args &...args) {
        auto logPtr = spdlog::get(name);
        if (logPtr) {
          logPtr->critical(fmt, args...);
        }
      }

      template<typename T>
      inline void critical_name(const std::string &name, const T &msg) {
        std::shared_ptr<spdlog::logger> logPtr = spdlog::get(name);
        if (logPtr) {
          // logging to multi-sink logger
          logPtr->critical(msg);
        }
      }

   };
} // end of cppsl::log

namespace cppsl {
   using logAppender = cppsl::log::SinkAppender;
   using logAppenderPtr = std::shared_ptr<logAppender>;
}

#endif /* __INCLUDE_CPPSL_LOG_SINK_APPENDER_H__ */
