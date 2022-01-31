// Copyright(c) 2015 Gabi Melman.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//
// Copyright(c) 2017 zhishupp.
// <spdlog> C wrapper for support: __func__,  __FILE__, __LINE__
//
// Copyright(c) 2020 A.Sacharov.
// C wrapper function with log4c prefix

/*************************************************************************//**
* \file    spdlog4c.h
* @brief   logging class C-API wrapper functions
* @author  Alexander Sacharov <a.sacharov@gmx.de>
* @ingroup COM4CPP
*****************************************************************************/

#ifndef __CPPSL_INCLUDE_CPPSL_LOG_LOG_4_C_H__
#define __CPPSL_INCLUDE_CPPSL_LOG_LOG_4_C_H__

//-----------------------------------------------------------------------------
// includes <...>
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

//-----------------------------------------------------------------------------
// includes "..."
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Defines and macros
//----------------------------------------------------------------------------

/**
 * Levels in log4c
 */
#define LOG4C_TRACE        0
#define LOG4C_DEBUG        1
#define LOG4C_INFO         2
#define LOG4C_WARN         3
#define LOG4C_ERROR        4
#define LOG4C_CRITICAL     5
#define LOG4C_OFF          6

#define LOG4C_SUCCESS      0
#define LOG4C_CALL_ERROR   "Call function returns error: "

//----------------------------------------------------------------------------
// Typedefs, structs, enums, unions and variables
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Public Prototypes
//----------------------------------------------------------------------------

/* only for C++ */
#ifdef __cplusplus
extern "C" {
#endif

//
// Return true an existing logger or false if a logger with such name doesn't exist.
// example: if(spdlog::get("my_logger"))
//
bool log4c_get(const char* logger_name);

//
// Set global formatting
// example: spdlog::set_pattern("%Y-%m-%d %H:%M:%S.%e %l : %v");
//
void log4c_set_pattern(const char *pattern);

//
// Set global logging level
//
void log4c_set_level(int log_level);

//
// Set global flush level
//
void log4c_flush_on(int log_level);

//
// Create and register multi/single threaded basic file logger.
// Basic logger simply writes to given file without any limitations or rotations.
//
void log4c_basic_logger_mt(const char* logger_name, const char* filename, int truncate);
void log4c_basic_logger_st(const char* logger_name, const char* filename, int truncate);

//
// Create and register multi/single threaded basic file logger.
// Basic logger simply writes to given file without any limitations or rotations.
//
void log4c_rotating_logger_mt(const char* logger_name, const char* filename, size_t max_file_size, size_t max_files);
void log4c_rotating_logger_st(const char* logger_name, const char* filename, size_t max_file_size, size_t max_files);

//
// Create file logger which creates new file on the given time (default in midnight):
//
void log4c_daily_logger_mt(const char* logger_name, const char* filename, int hour, int minute);
void log4c_daily_logger_st(const char* logger_name, const char* filename, int hour, int minute);

//
// Create and register stdout/stderr loggers
//
void log4c_stdout_logger_mt(const char* logger_name);
void log4c_stdout_logger_st(const char* logger_name);
void log4c_stderr_logger_mt(const char* logger_name);
void log4c_stderr_logger_st(const char* logger_name);

//
// Create and register colored stdout/stderr loggers
//
void log4c_stdout_color_mt(const char* logger_name);
void log4c_stdout_color_st(const char* logger_name);
void log4c_stderr_color_mt(const char* logger_name);
void log4c_stderr_color_st(const char* logger_name);

//
// Create and register a syslog logger
//
void log4c_syslog_logger(const char* logger_name, const char* ident, int syslog_option);

//
// Drop the reference to the given logger
//
void log4c_drop(const char* logger_name);

//
// Drop all references from the registry
//
void log4c_drop_all();

//
// Set a logger's level
//
void log4c_set_logger_level(const char* logger_name, int log_level);

//
// Get a logger's level
//
int log4c_get_logger_level(const char* logger_name);

//
// Set a logger's pattern
//
void log4c_set_logger_pattern(const char* logger_name, const char* format_string);

//
// Set a logger's flush on
//
void log4c_logger_flush_on(const char* logger_name, int log_level);

//
// Flush a logger
//
void log4c_logger_flush(const char* logger_name);

//
// Logger logging message
//
void log4c_logger_log(const char* logger_name, int log_level, const char* msg);

//
// Logger logging variadic message
//
void log4c_logger_log_var(const char* logger_name, int log_level, const char* fmt, ...);

//
// Logger logging message with __func__,  __FILE__, __LINE__
//
void log4c_logger_log_ffl(const char* logger_name, int log_level, const char* func, const char* file, size_t line, int error_code, const char* msg);

//
// Logger logging variadic message with __func__,  __FILE__, __LINE__
//
void log4c_logger_log_ffl_var(const char* logger_name, int log_level, const char* func, const char* file, size_t line, int error_code, const char* fmt, ...);

//-----------------------------------------------------------------------------
//
// Basic logging macro with __func__, __FILE__, __LINE__
//
//-----------------------------------------------------------------------------
// trace function, for enter a function
#define TRACE_FUNCTION(name) \
    log4c_logger_log_ffl(name, LOG4C_TRACE, __func__, __FILE__, __LINE__, LOG4C_SUCCESS, "==>")

//-----------------------------------------------------------------------------
// trace logging
#define LOG4C_LOG_TRACE(name, message) \
    log4c_logger_log_ffl(name, LOG4C_TRACE, __func__, __FILE__, __LINE__, LOG4C_SUCCESS, message)

//-----------------------------------------------------------------------------
// debug logging
#define LOG4C_LOG_DEBUG(name, message) \
    log4c_logger_log_ffl(name, LOG4C_DEBUG, __func__, __FILE__, __LINE__, LOG4C_SUCCESS, message)

//-----------------------------------------------------------------------------
// info logging
#define LOG4C_LOG_INFO(name, message) \
    log4c_logger_log_ffl(name, LOG4C_INFO, __func__, __FILE__, __LINE__, LOG4C_SUCCESS, message)

//-----------------------------------------------------------------------------
// warn logging
#define LOG4C_LOG_WARN(name, message) \
    log4c_logger_log_ffl(name, LOG4C_WARN, __func__, __FILE__, __LINE__, LOG4C_SUCCESS, message)

//-----------------------------------------------------------------------------
// error logging
#define LOG4C_LOG_ERROR(name, errcode, message) \
    log4c_logger_log_ffl(name, LOG4C_ERROR, __func__, __FILE__, __LINE__, errcode, message)

//-----------------------------------------------------------------------------
// critical logging
#define LOG4C_LOG_CRITI(name, errcode, message) \
    log4c_logger_log_ffl(name, LOG4C_CRITICAL, __func__, __FILE__, __LINE__, errcode, message)

//-----------------------------------------------------------------------------
// trace logging variadic
#define LOG4C_LOG_TRACE_VAR(name, message, ...) \
    log4c_logger_log_ffl_var(name, LOG4C_TRACE, __func__, __FILE__, __LINE__, LOG4C_SUCCESS, message, __VA_ARGS__)

//-----------------------------------------------------------------------------
// debug logging variadic
#define LOG4C_LOG_DEBUG_VAR(name, message, ...) \
    log4c_logger_log_ffl_var(name, LOG4C_DEBUG, __func__, __FILE__, __LINE__, LOG4C_SUCCESS, message, __VA_ARGS__)

//-----------------------------------------------------------------------------
// info logging variadic
#define LOG4C_LOG_INFO_VAR(name, message, ...) \
    log4c_logger_log_ffl_var(name, LOG4C_INFO, __func__, __FILE__, __LINE__, LOG4C_SUCCESS, message, __VA_ARGS__)

//-----------------------------------------------------------------------------
// warn logging variadic
#define LOG4C_LOG_WARN_VAR(name, message, ...) \
    log4c_logger_log_ffl_var(name, LOG4C_WARN, __func__, __FILE__, __LINE__, LOG4C_SUCCESS, message, __VA_ARGS__)

//-----------------------------------------------------------------------------
// error logging variadic
#define LOG4C_LOG_ERROR_VAR(name, errcode, message, ...) \
    log4c_logger_log_ffl_var(name, LOG4C_ERROR, __func__, __FILE__, __LINE__, errcode, message, __VA_ARGS__)

//-----------------------------------------------------------------------------
// critical logging variadic
#define LOG4C_LOG_CRITI_VAR(name, errcode, message, ...) \
    log4c_logger_log_ffl_var(name, LOG4C_CRITICAL, __func__, __FILE__, __LINE__, errcode, message, __VA_ARGS__)

//-----------------------------------------------------------------------------
//
// Extended logging macro
//
//-----------------------------------------------------------------------------
// error logging with break
#define LOG4C_IF_ERROR_BREAK(name, errcode, message) { \
    if (errcode != LOG4C_SUCCESS) { \
        LOG4C_LOG_ERROR(name, errcode, message); \
        break; \
    } \
}

//-----------------------------------------------------------------------------
// error logging with continue
#define LOG4C_IF_ERROR_CONTINUE(name, errcode, message) { \
    if (errcode != LOG4C_SUCCESS) { \
        LOG4C_LOG_ERROR(name, errcode, message); \
        continue; \
    } \
}

//-----------------------------------------------------------------------------
// error logging with return
#define LOG4C_IF_ERROR_RETURN(name, errcode, message) { \
    if (errcode != LOG4C_SUCCESS) { \
        LOG4C_LOG_ERROR(name, errcode, message); \
        return errcode; \
    } \
}

//-----------------------------------------------------------------------------
// error logging with go on
#define LOG4C_IF_ERROR_GO_ON(name, errcode, message) { \
    if (errcode != LOG4C_SUCCESS) { \
        LOG4C_LOG_ERROR(name, errcode, message); \
    } \
}

//-----------------------------------------------------------------------------
// error logging method with break
#define LOG4C_IF_METHOD_ERROR_BREAK(name, errcode, method) { \
    errcode = method; \
    if (errcode != LOG4C_SUCCESS) { \
        LOG4C_LOG_ERROR(name, errcode, LOG4C_CALL_ERROR #method); \
        break; \
    } \
}

//-----------------------------------------------------------------------------
// error logging method with continue
#define LOG4C_IF_METHOD_ERROR_CONTINUE(name, errcode, method) { \
    errcode = method; \
    if (errcode != LOG4C_SUCCESS) { \
        LOG4C_LOG_ERROR(name, errcode, LOG4C_CALL_ERROR #method); \
        continue; \
    } \
}

//-----------------------------------------------------------------------------
// error logging method with return
#define LOG4C_IF_METHOD_ERROR_RETURN(name, errcode, method) { \
    errcode = method; \
    if (errcode != LOG4C_SUCCESS) { \
        LOG4C_LOG_ERROR(name, errcode, LOG4C_CALL_ERROR #method); \
        return errcode; \
    } \
}

//-----------------------------------------------------------------------------
// error logging method with go on
#define IF_METHOD_ERROR_GO_ON(name, errcode, method) { \
    errcode = method; \
    if (errcode != LOG4C_SUCCESS) { \
        LOG4C_LOG_ERROR(name, errcode, LOG4C_CALL_ERROR #method); \
    } \
}

//-----------------------------------------------------------------------------
// error logging with break variadic
#define LOG4C_IF_ERROR_BREAK_VAR(name, errcode, message, ...) { \
    if (errcode != LOG4C_SUCCESS) { \
        LOG4C_LOG_ERROR_VAR(name, errcode, message, __VA_ARGS__); \
        break; \
    } \
}

//-----------------------------------------------------------------------------
// error logging with continue variadic
#define LOG4C_IF_ERROR_CONTINUE_VAR(name, errcode, message, ...) { \
    if (errcode != LOG4C_SUCCESS) { \
        LOG4C_LOG_ERROR_VAR(name, errcode, message, __VA_ARGS__); \
        continue; \
    } \
}

//-----------------------------------------------------------------------------
// error logging with return variadic
#define LOG4C_IF_ERROR_RETURN_VAR(name, errcode, message, ...) { \
    if (errcode != LOG4C_SUCCESS) { \
        LOG4C_LOG_ERROR_VAR(name, errcode, message, __VA_ARGS__); \
        return errcode; \
    } \
}

//-----------------------------------------------------------------------------
// error logging with go on variadic
#define LOG4C_IF_ERROR_GO_ON_VAR(name, errcode, message, ...) { \
    if (errcode != LOG4C_SUCCESS) { \
        LOG4C_LOG_ERROR_VAR(name, errcode, message, __VA_ARGS__); \
    } \
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* __CPPSL_INCLUDE_CPPSL_LOG_LOG_4_C_H__ */
