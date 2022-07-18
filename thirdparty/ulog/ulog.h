/**
 * @file ulog.h
 * @brief 
 * @author gerald (hongyi.zhu@aurefluidics.com)
 * @version 1.0
 * @date 2022-07-18
 * 
 * @copyright Copyright (c) 2022  Shanghai AureFluidics Technology Co.,Ltd
 * 
 * @par ChangeLog:
 * <table>
 * <tr><th>Date           <th>Version     <th>Author      <th>Description
 * <tr><td>2022-07-18     <td>1.0         <td>gerald      <td>the first version
 * </table>
 */

#ifndef __ULOG_H
#define __ULOG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  ULOG_TRACE_LEVEL=100,
  ULOG_DEBUG_LEVEL,
  ULOG_INFO_LEVEL,
  ULOG_WARNING_LEVEL,
  ULOG_ERROR_LEVEL,
  ULOG_CRITICAL_LEVEL,
  ULOG_ALWAYS_LEVEL
} ulog_level_t;

// The following macros enable or disable uLog.  If `ULOG_ENABLED` is
// defined at compile time, a macro such as `ULOG_INFO(...)` expands
// into `ulog_message(ULOG_INFO_LEVEL, ...)`.  If `ULOG_ENABLED` is not
// defined, then the same macro expands into `do {} while(0)` and will
// not generate any code at all.  
//
// There are two ways to enable uLog: you can uncomment the following
// line, or -- if it is commented out -- you can add -DULOG_ENABLED to
// your compiler switches.
#define ULOG_ENABLED

#ifdef ULOG_ENABLED
  #define ULOG_INIT() ulog_init()
  #define ULOG_SUBSCRIBE(a, b) ulog_subscribe(a, b)
  #define ULOG_UNSUBSCRIBE(a) ulog_unsubscribe(a)
  #define ulog_level_name(a) ulog_level_name(a)
  #define ULOG(...) ulog_message(__VA_ARGS__)
  #define ULOG_TRACE(...) ulog_message(ULOG_TRACE_LEVEL, __VA_ARGS__)
  #define ULOG_DEBUG(...) ulog_message(ULOG_DEBUG_LEVEL, __VA_ARGS__)
  #define ULOG_INFO(...) ulog_message(ULOG_INFO_LEVEL, __VA_ARGS__)
  #define ULOG_WARNING(...) ulog_message(ULOG_WARNING_LEVEL, __VA_ARGS__)
  #define ULOG_ERROR(...) ulog_message(ULOG_ERROR_LEVEL, __VA_ARGS__)
  #define ULOG_CRITICAL(...) ulog_message(ULOG_CRITICAL_LEVEL, __VA_ARGS__)
  #define ULOG_ALWAYS(...) ulog_message(ULOG_ALWAYS_LEVEL, __VA_ARGS__)
#else
  // uLog vanishes when disabled at compile time...
  #define ULOG_INIT() do {} while(0)
  #define ULOG_SUBSCRIBE(a, b) do {} while(0)
  #define ULOG_UNSUBSCRIBE(a) do {} while(0)
  #define ulog_level_name(a) do {} while(0)
  #define ULOG(s, f, ...) do {} while(0)
  #define ULOG_TRACE(f, ...) do {} while(0)
  #define ULOG_DEBUG(f, ...) do {} while(0)
  #define ULOG_INFO(f, ...) do {} while(0)
  #define ULOG_WARNING(f, ...) do {} while(0)
  #define ULOG_ERROR(f, ...) do {} while(0)
  #define ULOG_CRITICAL(f, ...) do {} while(0)
  #define ULOG_ALWAYS(f, ...) do {} while(0)
#endif

typedef enum {
  ULOG_ERR_NONE = 0,
  ULOG_ERR_SUBSCRIBERS_EXCEEDED,
  ULOG_ERR_NOT_SUBSCRIBED,
} ulog_err_t;

// define the maximum number of concurrent subscribers
#define ULOG_MAX_SUBSCRIBERS 6

// maximum length of formatted log message
#define ULOG_MAX_MESSAGE_LENGTH 120

/**
 * @brief: prototype for uLog subscribers.
 */
typedef void (*ulog_function_t)(ulog_level_t severity, char *msg);

void ulog_init();
ulog_err_t ulog_subscribe(ulog_function_t fn, ulog_level_t threshold);
ulog_err_t ulog_unsubscribe(ulog_function_t fn);
const char *ulog_level_name(ulog_level_t level);
void ulog_message(ulog_level_t severity, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __ULOG_H */
