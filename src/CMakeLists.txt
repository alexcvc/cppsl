### Set project name
set(TargetName cppsl)

### List of CPP (source) library files.
set(${TargetName}_SRC
   "file/fileBaseAppender.cpp"
   "file/fileRollAppender.cpp"
   "log/logAppender.cpp"
   "buffer/ring_buffer.cpp"
   "threads/threadWorker.cpp"
)

set(${TargetName}_HDR
   "../include/cppsl/buffer/cyclebuffer.hpp"
   "../include/cppsl/buffer/ring_buffer.hpp"
   "../include/cppsl/buffer/ring_buffer.tcc"
   "../include/cppsl/console/menuDriver.hpp"
   "../include/cppsl/container/dequeSafe.hpp"
   "../include/cppsl/container/listSafe.hpp"
   "../include/cppsl/container/queueLockFree.hpp"
   "../include/cppsl/container/queueSafe.hpp"
   "../include/cppsl/file/fileBaseAppender.hpp"
   "../include/cppsl/file/fileRollAppender.hpp"
   "../include/cppsl/log/details/fallback_sink.hpp"
   "../include/cppsl/log/details/rsyslog_sink.hpp"
   "../include/cppsl/log/baseLogAppender.hpp"
   "../include/cppsl/log/logAppender.hpp"
   "../include/cppsl/math/constants.hpp"
   "../include/cppsl/math/functions.hpp"
   "../include/cppsl/math/samplRateConvFIR.hpp"
   "../include/cppsl/math/samplRateConvLagrange.hpp"
   "../include/cppsl/math/samplRateConvLinear.hpp"
   "../include/cppsl/threads/threadWorker.hpp"
   "../include/cppsl/time/functions.hpp"
   "../include/cppsl/time/stopTimer.hpp"
   "../include/cppsl/time/watch.hpp"
   "../include/cppsl/byteSwap.hpp"
   "../include/cppsl/enumBitmask.hpp"
   "../include/cppsl/result.hpp"
)

### add executable
add_library(${TargetName} ${${TargetName}_SRC} ${${TargetName}_HDR})

# add include directories
target_include_directories(${TargetName}
   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
   PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>"
   PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../submodules/spdlog/include>"
   PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../submodules/fmt/include>"
   "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")

set_target_properties(${TargetName} PROPERTIES
   PUBLIC_HEADER "${${TargetName}_HDR}"
)
#target_link_libraries(${TargetName} PRIVATE fmt::fmt-header-only)
#target_link_libraries(${TargetName} PRIVATE fmt::fmt)
#target_link_libraries(${TargetName} PRIVATE fmt::fmt spdlog::spdlog)

# definitions
if (${IS_BIG_ENDIAN})
   target_compile_definitions(${TargetName} PUBLIC "-D_BENDIAN")
else ()
   target_compile_definitions(${TargetName} PUBLIC "-D_LENDIAN")
endif ()

target_compile_options(${TargetName} PRIVATE
   "-Wall"
   "-Wextra"
   "-Werror=return-type")
