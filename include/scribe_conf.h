//
//
// System dependent configuration for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_CONF_H
#define SCRIBE_CONF_H

#define SCRIBE_VERSION_MAJOR "0"
#define SCRIBE_VERSION_MINOR "0"
#define SCRIBE_VERSION_PATCH "0"
#define SCRIBE_VERSION ""SCRIBE_VERSION_MAJOR"."SCRIBE_VERSION_MINOR"."SCRIBE_VERSION_PATCH""

#ifdef NDEBUG
#   define SCRIBE_DEBUG 0
#else
#   define SCRIBE_DEBUG 1
#endif

#define SCRIBE_RWLOCK_DELAY 50

#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

// OS specific configuration
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#   define SCRIBE_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
#   define SCRIBE_OSX
#elif defined(unix) || defined(__unix__) || defined(__unix)
#   define SCRIBE_UNIX
#elif defined(linux) || defined(__linux__) || defined(__linux)
#   define SCRIBE_LINUX
#elif !(defined(__gnuc__) || defined(__GNUC__))
#   error "Unknown OS, can\'t decide which types to use!"
#endif

#if defined(SCRIBE_WINDOWS)
#   define SCRIBE_TIME_T SYSTEMTIME
#   define SCRIBE_PID_T int
#else
#   define SCRIBE_TIME_T struct timeval
#   define SCRIBE_PID_T pid_t
#endif

#endif

