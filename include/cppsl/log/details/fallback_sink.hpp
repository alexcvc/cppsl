//
// Copyright (c) 2015 David Schury, Gabi Melman
// Copyright (c) 2021 Alexander Sacharov <a.sacharov@gmx.de>
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.
//

/*************************************************************************//**
 * @file
 * \brief       contains a simple custom sink made for spdlog that will
 * allow you to create a list of sinks and fallback on each tier if the previous
 * sink fails.
 * \author      Alexander Sacharov <a.sacharov@gmx.de>
 * \date        2021-07-28
 *****************************************************************************/

#ifndef INCLUDE_CPPSL_LOG_DETAILS_FALLBACK_SINK_HPP
#define INCLUDE_CPPSL_LOG_DETAILS_FALLBACK_SINK_HPP

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------

#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------
#include <spdlog/details/log_msg.h>
#include <spdlog/details/null_mutex.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/sink.h>

//----------------------------------------------------------------------------
// Public defines and macros
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Function Prototypes
//----------------------------------------------------------------------------

namespace spdlog::sinks {
   class fallback_sink : public sink {
    public:

      ~fallback_sink() {
        _sinks_to_remove.clear();
        _sinks.clear();
      }

      void log(const details::log_msg &msg) override {
        for (auto &sink : _sinks) {
          try {
            sink->log(msg);
            break;
          }
          catch (const std::exception &ex) {
            sink->flush();

            _sinks_to_remove.push_back(sink);
          }
        }

        if (!_sinks_to_remove.empty()) {
          for (auto sink_to_remove : _sinks_to_remove) {
            remove_sink(sink_to_remove);
          }
          _sinks_to_remove.clear();
        }
      }

      void flush() override {
        for (auto &sink : _sinks)
          sink->flush();
      }

      void add_sink(std::shared_ptr<sink> sink) {
        _sinks.push_back(sink);
      }

      void remove_sink(std::shared_ptr<sink> sink) {
        auto pos = std::find(_sinks.begin(), _sinks.end(), sink);

        _sinks.erase(pos);

      }

    private:
      std::vector<std::shared_ptr<sink>> _sinks;
      std::vector<std::shared_ptr<sink>> _sinks_to_remove;
   };
}

#endif /* INCLUDE_CPPSL_LOG_DETAILS_FALLBACK_SINK_HPP */
