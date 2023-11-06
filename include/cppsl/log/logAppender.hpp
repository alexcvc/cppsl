// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//
// Copyright(c) 2021 A.Sacharov.
// C++ wrapper class with  multiple loggers all sharing the same sink
//
// https://spdlog.docsforge.com/master/1.quickstart/#create-a-multiple-loggers-all-sharing-the-same-file-sink-aka-categories
// Docs Built May 30, 2021

/*************************************************************************/ /**
 * @file
 * \brief  C++ wrapper class with  multiple loggers all sharing the same sink
 * aka categories
 * @details This class maintains spdlog details as a private inside a class.
 * Other objects can not access to details directly, instead, they can only
 * invoke a list of public functions. This abstracts the spdlog in logger
 * container. One of the advantages is being able to apply the useful functions
 * with no modification.
 * All headers with skins are hidden in the cpp file. This brings significant
 * benefit in compiling and breaking the dependency of all files on spdlog.
 * We can use a logger targets with no confusion with mixing types and extra headers.
 * Each of sinks would have their own output target in any case. This appender contain:
 *  - one sink to console target;
 *  - one sink to file target;
 *  - one sink to daily file target;
 *  - one sink to rotate file target;
 *  - one sink to syslog target;
 *  - one sink to remote syslog target;
 *
 * \author  AS
 * \date    2020-07-03
 *****************************************************************************/

#ifndef INCLUDE_CPPSL_LOG_LOG_APPENDER_HPP
#define INCLUDE_CPPSL_LOG_LOG_APPENDER_HPP

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <memory>
#include <vector>

#include <spdlog/spdlog.h>
#include <spdlog/logger.h>
#include <spdlog/fmt/fmt.h>
#include <fmt/core.h>

/**
 * @brief This namespace contains appender logger with skins.
 * C++ wrapper class with  multiple loggers all sharing the same sink aka categories
 */
namespace cppsl::log {

  using log_level = spdlog::level::level_enum;

  /**
   * @brief Class Log implements logger in  application
   */
  class LogAppender {
    std::string m_name;                       ///< name of log appender
    std::shared_ptr<spdlog::logger> m_logSp;  ///< appender handler
    spdlog::sink_ptr m_console_sink;          ///< log to console
    spdlog::sink_ptr m_file_sink;             ///< log to file
    spdlog::sink_ptr m_daily_sink;            ///< log to daily file
    spdlog::sink_ptr m_rotate_sink;           ///< log to rotate file
    spdlog::sink_ptr m_syslog_sink;           ///< log to syslog
    spdlog::sink_ptr m_rsyslog_sink;          ///< log to remote syslog

   public:
    /**
         * default constructor
      * @param name - logging name. It is useful, if you want to serve logger by name instead shared pointer
         */
    explicit LogAppender(std::string_view name) : m_name(name) {}

    /**
         * destructor
         */
    ~LogAppender() noexcept { drop_all(); }

    /**
      * Interval based flush. This is implemented by a single worker thread that periodically
      * calls flush() on each logger.
      * @param interval_seconds - interval in seconds
      */
    inline void flush_every(std::chrono::seconds &interval_seconds) noexcept {
      spdlog::flush_every(std::chrono::seconds(interval_seconds));
    }

    /**
      * add basic file sink + multi sinks.
      * Basic logger simply writes to given file without any limitations or rotations.
      * @param filename - name of logfile
      * @param truncate - if true - truncate by open
      * @param level - log level for this sink
      */
    [[nodiscard]] bool add_basic_file_sink(const std::string &filename, int truncate, log_level level) noexcept;

    /**
      * add rotation file sink.
      * Such logger simply writes to given file with rotations.
      * @param filename - target log file name
      * @param max_file_size
      * @param max_files
      * @param level - log level for this sink
      */
    [[nodiscard]] bool add_rotation_file_sink(const std::string &filename, size_t max_file_size, size_t max_files,
                                              log_level level) noexcept;

    /**
      * Create file sink which create new file on the given time (default in midnight)
      * @param filename - target log file name
      * @param hour
      * @param minute
      * @param level - log level for this sink
      */
    [[nodiscard]] bool add_daily_file_sink(const std::string &filename, int hour, int minute, log_level level) noexcept;

    /**
      * Create and register no-/colored console sink
      * @param to_stderr
      * @param colored
      * @param level - log level for this sink
      */
    [[nodiscard]] bool add_console_sink(bool to_stderr, bool colored, log_level level) noexcept;

    /**
      * Create and register a rsyslog sinks to remote server
      * @param ident - The string pointed to by ident is prepended to every message.
      * @param rsyslog_ip - remote syslog server IP
      * @param syslog_facility - facility codes specifies what type of program is logging the message.
      * @param severity - severity of message
      * More information in https://man7.org/linux/man-pages/man3/syslog.3.html
      * @param lev - level of message
      * @param port - remote port of server
      * @param enable_formatting - default true. However the message format maybe changed.
      * @param log_buffer_max_size - buffer reserved in the message string. This increases performance when creating
      * log messages.
      */
    [[nodiscard]] bool add_rsyslog_sink(const std::string &ident, const std::string &rsyslog_ip, int syslog_facility,
                                        log_level lev, int port = 514, bool enable_formatting = true,
                                        int log_buffer_max_size = 1024 * 1024 * 16) noexcept;

    /**
      * Create and register a syslog sinks
      * @param syslog_ident- The string pointed to by ident is prepended to every message.
      * @param syslog_option - options. More information in https://man7.org/linux/man-pages/man3/syslog.3.html
      * @param syslog_facility - facility codes specifies what type of program is logging the message.
      * @param enable_formatting - default true. However the message format maybe changed.
      * @param level - log level for this sink
      */
    [[nodiscard]] bool add_syslog_sink(const std::string &syslog_ident, int syslog_option, int syslog_facility,
                                       bool enable_formatting, log_level level) noexcept;

    /**
      * open logger with all added sinks
      * @param level default logging level if not set in sink
      * @return true if successfully, otherwise - false
      */
    [[nodiscard]] bool open_logger(log_level level) noexcept;

    /**
      * Drop all references from the registry
      */
    void drop_all() noexcept;

    /**
     * @brief set level function with no exception
     * @param level - new logging level
     */
    void set_level_console_sink(spdlog::level::level_enum level) noexcept;

    void set_level_base_file_sink(spdlog::level::level_enum level) noexcept;

    void set_level_daily_sink(spdlog::level::level_enum level) noexcept;

    void set_level_rotation_file_sink(spdlog::level::level_enum level) noexcept;

    void set_level_syslog_sink(spdlog::level::level_enum level) noexcept;

    void set_level_rsyslog_sink(spdlog::level::level_enum level) noexcept;

    /**
     * set level only for logger (general and for next sinks default)
     * @param level - logging level
     */
    void set_level(spdlog::level::level_enum level) noexcept;

  private:
    /**
      * check existence and if needs - to create path
      * @param filename - file name
      * @return true if successfully, otherwise - false
      */
    [[nodiscard]] bool check_create_path(const std::string &filename);

  public:
    //----------------------------------------------------------
    // trace
    //----------------------------------------------------------
    template<typename... Args>
    inline void trace(fmt::format_string<Args...> fmt, Args &&...args) {
      if (m_logSp) {
        m_logSp->trace(fmt, std::forward<Args>(args)...);
      }
    }

    template<typename T>
    inline void trace(const T &msg) {
      if (m_logSp) {
        m_logSp->trace(msg);
      }
    }

    template<typename... Args>
    inline void trace_if(bool flag, fmt::format_string<Args...> fmt, Args &&...args) {
      if (flag) {
        if (m_logSp) {
          m_logSp->trace(fmt, std::forward<Args>(args)...);
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
    inline void trace_name(const std::string &name, fmt::format_string<Args...> fmt, Args &&...args) {
      auto logPtr = spdlog::get(name);
      if (logPtr) {
        logPtr->trace(fmt, std::forward<Args>(args)...);
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
    inline void debug(fmt::format_string<Args...> fmt, Args &&...args) {
      if (m_logSp) {
        m_logSp->debug(fmt, std::forward<Args>(args)...);
      }
    }

    template<typename T>
    inline void debug(const T &msg) {
      if (m_logSp) {
        m_logSp->debug(msg);
      }
    }

    template<typename... Args>
    inline void debug_if(bool flag, fmt::format_string<Args...> fmt, Args &&...args) {
      if (flag) {
        if (m_logSp) {
          m_logSp->debug(fmt, std::forward<Args>(args)...);
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
    inline void debug_name(const std::string &name, fmt::format_string<Args...> fmt, Args &&...args) {
      auto logPtr = spdlog::get(name);
      if (logPtr) {
        logPtr->debug(fmt, std::forward<Args>(args)...);
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
    inline void info(fmt::format_string<Args...> fmt, Args &&...args) {
      if (m_logSp) {
        m_logSp->info(fmt, std::forward<Args>(args)...);
      }
    }

    template<typename T>
    inline void info(const T &msg) {
      if (m_logSp) {
        m_logSp->info(msg);
      }
    }

    template<typename... Args>
    inline void info_if(bool flag, fmt::format_string<Args...> fmt, Args &&...args) {
      if (flag) {
        if (m_logSp) {
          m_logSp->info(fmt, std::forward<Args>(args)...);
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
    inline void info_name(const std::string &name, fmt::format_string<Args...> fmt, Args &&...args) {
      auto logPtr = spdlog::get(name);
      if (logPtr) {
        logPtr->info(fmt, std::forward<Args>(args)...);
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
    inline void warn(fmt::format_string<Args...> fmt, Args &&...args) {
      if (m_logSp) {
        m_logSp->warn(fmt, std::forward<Args>(args)...);
      }
    }

    template<typename T>
    inline void warn(const T &msg) {
      if (m_logSp) {
        m_logSp->warn(msg);
      }
    }

    template<typename... Args>
    inline void warn_if(bool flag, fmt::format_string<Args...> fmt, Args &&...args) {
      if (flag) {
        if (m_logSp) {
          m_logSp->warn(fmt, std::forward<Args>(args)...);
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
    inline void warn_name(const std::string &name, fmt::format_string<Args...> fmt, Args &&...args) {
      auto logPtr = spdlog::get(name);
      if (logPtr) {
        logPtr->warn(fmt, std::forward<Args>(args)...);
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
    inline void error(fmt::format_string<Args...> fmt, Args &&...args) {
      if (m_logSp) {
        m_logSp->error(fmt, std::forward<Args>(args)...);
      }
    }

    template<typename T>
    inline void error(const T &msg) {
      if (m_logSp) {
        m_logSp->error(msg);
      }
    }

    template<typename... Args>
    inline void error_if(bool flag, fmt::format_string<Args...> fmt, Args &&...args) {
      if (flag) {
        if (m_logSp) {
          m_logSp->error(fmt, std::forward<Args>(args)...);
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
    inline void error_name(const std::string &name, fmt::format_string<Args...> fmt, Args &&...args) {
      auto logPtr = spdlog::get(name);
      if (logPtr) {
        logPtr->error(fmt, std::forward<Args>(args)...);
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
    inline void critical(fmt::format_string<Args...> fmt, Args &&...args) {
      if (m_logSp) {
        m_logSp->critical(fmt, std::forward<Args>(args)...);
      }
    }

    template<typename T>
    inline void critical(const T &msg) {
      if (m_logSp) {
        m_logSp->critical(msg);
      }
    }

    template<typename... Args>
    inline void critical_if(bool flag, fmt::format_string<Args...> fmt, Args &&...args) {
      if (flag) {
        if (m_logSp) {
          m_logSp->critical(fmt, std::forward<Args>(args)...);
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
    inline void critical_name(const std::string &name, fmt::format_string<Args...> fmt, Args &&...args) {
      auto logPtr = spdlog::get(name);
      if (logPtr) {
        logPtr->critical(fmt, std::forward<Args>(args)...);
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

}// namespace cppsl::log

namespace cppsl::log {
  using log_appenderPtr = std::shared_ptr<cppsl::log::LogAppender>;

  template<typename Factory = cppsl::log::LogAppender>
  inline auto create_log_appender(std::string_view name) {
    return std::make_shared<Factory>(name);
  }

}// namespace cppsl::log

#endif /* INCLUDE_CPPSL_LOG_LOG_APPENDER_HPP */
