// Copyright(c) 2015-present, Gabi Melman & spdlog contributors.
// Copyright (c) 2021 Alexander Sacharov <a.sacharov@gmx.de>
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * @file
 * \brief       contains sink for syslog to remote server.
 * \author      Alexander Sacharov <a.sacharov@gmx.de>
 * \date        2021-07-28
 *****************************************************************************/

#ifndef INCLUDE_CPPSL_LOG_DETAILS_RSYSLOG_SINK_HPP
#define INCLUDE_CPPSL_LOG_DETAILS_RSYSLOG_SINK_HPP

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <array>
#include <string>
#include <map>
#include <sstream>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include "spdlog/sinks/base_sink.h"
#include "spdlog/details/null_mutex.h"
#include "spdlog/common.h"
#include "spdlog/details/synchronous_factory.h"

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

namespace spdlog {
   namespace sinks {
      /**
       * Sink that write to rsyslog using udp.
       */
      template<typename Mutex>
      class rsyslog_sink final : public base_sink<Mutex> {

         std::map<int, int> m_mapSeveritySpd2Syslog = {
            {   SPDLOG_LEVEL_CRITICAL, LOG_CRIT   }
            , { SPDLOG_LEVEL_ERROR   , LOG_ERR    }
            , { SPDLOG_LEVEL_WARN    , LOG_WARNING}
            , { SPDLOG_LEVEL_INFO    , LOG_INFO }
            , { SPDLOG_LEVEL_DEBUG   , LOG_DEBUG }
            , { SPDLOG_LEVEL_TRACE   , LOG_DEBUG }
            , { SPDLOG_LEVEL_OFF     , 0 }
            ,};

         const int m_log_buffer_max_size{16777216};   ///< 16K
         struct sockaddr_in m_sockaddr;               ///< address
         int m_fd{-1};                                ///< socket descriptor
         int m_facility{0};                           ///< facility
         std::string m_ident;                         ///< identity
         std::string m_buffer;                        ///< buffer

      public:
         /**
          * constructor
          * @param ident - identity
          * @param server_ip - remote server IP
          * @param facility - facility codes
          * @param port - port is 514
          * @param log_buffer_max_size
          * @param enable_formatting
          */
         rsyslog_sink(const std::string &ident,
                      const std::string &server_ip,
                      int facility,
                      int log_buffer_max_size,
                      uint16_t port,
                      bool enable_formatting)
            : m_log_buffer_max_size(log_buffer_max_size), m_facility(facility), m_ident(ident), m_enable_formatting(enable_formatting) {

            if (m_log_buffer_max_size > std::numeric_limits<int>::max()) {
               SPDLOG_THROW(spdlog_ex("too large maxLogSize"));
            }

            m_buffer.reserve(m_log_buffer_max_size);
            // socket
            memset(&m_sockaddr, 0, sizeof(m_sockaddr));
            m_sockaddr.sin_family = AF_INET;
            m_sockaddr.sin_port = htons(port);
            inet_pton(AF_INET, server_ip.c_str(), &m_sockaddr.sin_addr);

            // open  socket
            open_socket();
         }

         /**
          * destructor
          */
         ~rsyslog_sink() override {
            close(m_fd);
         }

         /**
          * delete copy constructor and assignment
          */
         rsyslog_sink(const rsyslog_sink &) = delete;
         rsyslog_sink &operator=(const rsyslog_sink &) = delete;

      protected:
         /**
          * sink message after severity filter
          * @param msg
          */
         void sink_it_(const details::log_msg &msg) override {

            if( msg.level != level::off) {
               string_view_t payload;
               memory_buf_t formatted;
               if (m_enable_formatting) {
                  base_sink<Mutex>::formatter_->format(msg, formatted);
                  payload = string_view_t(formatted.data(), formatted.size());
               }
               else {
                  payload = msg.payload;
               }
               size_t length = payload.size();
               // limit to max int
               length = length > static_cast<size_t>(std::numeric_limits<int>::max()) ? static_cast<size_t>(std::numeric_limits<int>::max())
                                                                                      : length;
               std::stringstream ss;
               // <%u>%s:
               ss << "<" << m_facility + m_mapSeveritySpd2Syslog[msg.level] << ">" << m_ident << ": ";

               m_buffer += ss.str();
               length = length > m_log_buffer_max_size - m_buffer.size() ? m_log_buffer_max_size - m_buffer.size() : length;
               if (length > 0) {
                  m_buffer.append(payload.data(), length);
               }
               if( write(m_fd, m_buffer.c_str(), m_buffer.size()) == -1) perror("write error");
               m_buffer.clear();
            }
         }

         void flush_() override {}

         bool m_enable_formatting{false};

      private:
         /**
          * open UDP socket
          */
         void open_socket() {
            int nb = 1;

            if ((m_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
               SPDLOG_THROW(spdlog_ex("failed create socket"));
            } else if (ioctl(m_fd, FIONBIO, &nb) == -1) {
               SPDLOG_THROW(spdlog_ex("failed ioctl socket FIONBIO"));
            } else if (connect(m_fd, reinterpret_cast<struct sockaddr *>(&m_sockaddr), sizeof(m_sockaddr)) < 0) {
               SPDLOG_THROW(spdlog_ex("failed connect socket"));
            }
         }
      };

      using rsyslog_sink_mt = rsyslog_sink<std::mutex>;
      using rsyslog_sink_st = rsyslog_sink<details::null_mutex>;
   } // namespace sinks

// Create and register a syslog logger
   template<typename Factory = synchronous_factory>
   inline std::shared_ptr<logger> rsyslog_logger_mt(const std::string &logger_name,
                                                    const std::string &ident,
                                                    const std::string &rsyslog_ip,
                                                    uint facility,
                                                    int log_buffer_max_size = 1024 * 1024 * 16,
                                                    uint16_t port = 514,
                                                    bool enable_formatting = true) {
      return Factory::template create<sinks::rsyslog_sink_mt>(logger_name, ident, rsyslog_ip, facility, log_buffer_max_size, port,
                                                              enable_formatting);
   }

   template<typename Factory = synchronous_factory>
   inline std::shared_ptr<logger> rsyslog_logger_st(const std::string &logger_name,
                                                    const std::string &ident,
                                                    const std::string &rsyslog_ip,
                                                    uint facility,
                                                    int log_buffer_max_size = 1024 * 1024 * 16,
                                                    uint16_t port = 514,
                                                    bool enable_formatting = true) {
      return Factory::template create<sinks::rsyslog_sink_st>(logger_name, ident, rsyslog_ip, facility, log_buffer_max_size, port,
                                                              enable_formatting);
   }

} // namespace spdlog

#endif /* INCLUDE_CPPSL_LOG_DETAILS_RSYSLOG_SINK_HPP */
