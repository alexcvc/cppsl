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
 * \brief       contains implementation log writer class.
 * \date        2021-07-28
 *****************************************************************************/

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <cppsl/log/logAppender.hpp>

#include <filesystem>
#include <iostream>
#include <string_view>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/syslog_sink.h>
#include <cppsl/log/details/rsyslog_sink.hpp>
#include <fmt/format.h>

namespace cppsl::log {
  using std::string;
  using log_level = spdlog::level::level_enum;
  namespace fs = std::filesystem;

/*************************************************************************/ /**
 * open logger with all added sinks
 * @param level default logging level if not set in sink
 * @return true if successfully, otherwise - false
 */
  bool LogAppender::open_logger(log_level level) noexcept {
    try {
      m_logSp.reset();

      if (m_console_sink) {
        if (m_logSp)
          m_logSp->sinks().push_back(m_console_sink);
        else
          m_logSp = std::make_shared<spdlog::logger>(m_name, m_console_sink);
      }

      if (m_file_sink) {
        if (m_logSp)
          m_logSp->sinks().push_back(m_file_sink);
        else
          m_logSp = std::make_shared<spdlog::logger>(m_name, m_file_sink);
      }

      if (m_rotate_sink) {
        if (m_logSp)
          m_logSp->sinks().push_back(m_rotate_sink);
        else
          m_logSp = std::make_shared<spdlog::logger>(m_name, m_rotate_sink);
      }

      if (m_daily_sink) {
        if (m_logSp)
          m_logSp->sinks().push_back(m_daily_sink);
        else
          m_logSp = std::make_shared<spdlog::logger>(m_name, m_daily_sink);
      }

      if (m_syslog_sink) {
        if (m_logSp)
          m_logSp->sinks().push_back(m_syslog_sink);
        else
          m_logSp = std::make_shared<spdlog::logger>(m_name, m_syslog_sink);
      }

      if (m_rsyslog_sink) {
        if (m_logSp)
          m_logSp->sinks().push_back(m_rsyslog_sink);
        else
          m_logSp = std::make_shared<spdlog::logger>(m_name, m_rsyslog_sink);
      }

      if (!m_logSp)
        throw spdlog::spdlog_ex("Log pointer is null");

      m_logSp->set_level(level);

      // or you can even set multi_sink logger as default logger
      if (m_console_sink) {
        spdlog::set_default_logger(std::make_shared<spdlog::logger>("console", m_console_sink));
        spdlog::set_level(m_console_sink->level());
      }

      spdlog::register_logger(m_logSp);

      return true;
    } catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log open failed: " << ex.what() << std::endl;
      return false;
    }
  }

/**
 * @brief
 * @param filename
 * @param truncate
 * @param level
 * @return
 */
  bool LogAppender::add_basic_file_sink(const std::string &filename, int truncate, log_level level) noexcept {
    try {
      // create path
      if (!check_create_path(filename)) {
        throw std::runtime_error(fmt::format("cannot check or create path for: {}", filename));
      }
      m_file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filename, truncate);
      m_file_sink->set_level(level);
      return true;
    } catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
    }
  }

/**
 * @brief
 * @param filename
 * @param max_file_size
 * @param max_files
 * @param level
 * @return
 */
  bool LogAppender::add_rotation_file_sink(const std::string &filename, size_t max_file_size, size_t max_files,
                                           log_level level) noexcept {
    try {
      if (!check_create_path(filename)) {
        throw std::runtime_error(fmt::format("cannot check or create path for: {}", filename));
      }
    m_rotate_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filename, max_file_size, max_files);
      m_rotate_sink->set_level(level);
      return true;
    } catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
    }
  }

/**
 * @brief
 * @param filename
 * @param hour
 * @param minute
 * @param level
 * @return
 */
  bool LogAppender::add_daily_file_sink(const std::string &filename, int hour, int minute, log_level level) noexcept {
    try {
      // create path
      if (!check_create_path(filename)) {
        throw std::runtime_error(fmt::format("cannot check or create path for: {}", filename));
      }
    m_daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(filename, hour, minute);
      m_daily_sink->set_level(level);

      return true;
    } catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
    }
  }

/**
 * @brief
 * @param to_stderr
 * @param colored
 * @param level
 * @return
 */
  bool LogAppender::add_console_sink(bool to_stderr, bool colored, log_level level) noexcept {
    try {
      if (!to_stderr && colored)
        m_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      else if (!to_stderr)
        m_console_sink = std::make_shared<spdlog::sinks::stdout_sink_mt>();
      else if (colored)
        m_console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
      else
        m_console_sink = std::make_shared<spdlog::sinks::stderr_sink_mt>();

      m_console_sink->set_level(level);

      return true;
    } catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
    }
  }

  bool LogAppender::add_syslog_sink(const std::string &syslog_ident, int syslog_option, int syslog_facility,
                                    bool enable_formatting, log_level level) noexcept {
    try {
    m_syslog_sink = std::make_shared<spdlog::sinks::syslog_sink_mt>(syslog_ident, syslog_option, syslog_facility,
                                                         enable_formatting);
      m_syslog_sink->set_level(level);

      return true;
    } catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
    }
  }

  bool LogAppender::add_rsyslog_sink(const string &ident, const string &rsyslog_ip, int syslog_facility, log_level lev,
                                     int port, bool enable_formatting, int log_buffer_max_size) noexcept {
    try {
      m_rsyslog_sink = std::make_shared<spdlog::sinks::rsyslog_sink_mt>(ident, rsyslog_ip, syslog_facility,
                                                                        log_buffer_max_size, port, enable_formatting);
      m_rsyslog_sink->set_pattern("[%Y-%m-%d %H:%M:%S:%e] [%n] [%l] [%P] %@ : %v");
      m_rsyslog_sink->set_level(lev);

      return true;
    } catch (const spdlog::spdlog_ex &ex) {
      std::cout << "Log initialization failed: " << ex.what() << std::endl;
      return false;
    }
  }

  void LogAppender::drop_all() noexcept {
    spdlog::drop(m_name);
  }

  void LogAppender::set_level_console_sink(spdlog::level::level_enum level) noexcept {
    if (m_console_sink) {
      m_console_sink->set_level(level);
    }
  }

  void LogAppender::set_level_base_file_sink(spdlog::level::level_enum level) noexcept {
    if (m_file_sink) {
      m_file_sink->set_level(level);
    }
  }

  void LogAppender::set_level_daily_sink(spdlog::level::level_enum level) noexcept {
    if (m_daily_sink) {
      m_daily_sink->set_level(level);
    }
  }

  void LogAppender::set_level_rotation_file_sink(spdlog::level::level_enum level) noexcept {
    if (m_rotate_sink) {
      m_rotate_sink->set_level(level);
    }
  }

  void LogAppender::set_level_syslog_sink(spdlog::level::level_enum level) noexcept {
    if (m_syslog_sink) {
      m_syslog_sink->set_level(level);
    }
  }

  void LogAppender::set_level_rsyslog_sink(spdlog::level::level_enum level) noexcept {
    if (m_rsyslog_sink) {
      m_rsyslog_sink->set_level(level);
    }
  }

  /**
     * set level only for logger (general and for next sinks default)
     * @param level - logging level
     */
  void LogAppender::set_level(spdlog::level::level_enum level) noexcept {
    if (m_logSp) {
      m_logSp->set_level(level);
    }
  }






/**
 * check existence and if needs - to create path
 * @param filename - file name
 * @return true if successfully, otherwise - false
 */
  bool LogAppender::check_create_path(const std::string &filename) {
    // check parent folder
    auto folder = fs::path(filename).parent_path();
    if (folder.empty())
      folder.assign("./");
    else if (!fs::exists(folder)) {
      // check  create directory
      return fs::create_directories(folder);
    }
    return true;
  }

}   // namespace cppsl::log
