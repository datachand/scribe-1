//
// System dependent configuration for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_CONF_H
#define SCRIBE_CONF_H

#ifdef NDEBUG
    #define SCRIBE_DEBUG 1
#else
    #define SCRIBE_DEBUG 0
#endif

// OS specific configuration
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#   define SCRIBE_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
#   define SCRIBE_OSX
#elif defined(unix) || defined(__unix__) || defined(__unix)
#   define SCRIBE_UNIX
#elif defined(linux) || defined(__linux__) || defined(__linux)
#   define SCRIBE_LINUX
#else
#   error Unknown OS, can't decide which types to use!
#endif

#if defined(SCRIBE_WINDOWS)
#   define SCRIBE_TIME_T SYSTEMTIME
#   define SCRIBE_PID_T int
#elif defined(SCRIBE_OSX)
#   define SCRIBE_TIME_T struct timeval
#   define SCRIBE_PID_T pid_t
#elif defined(SCRIBE_UNIX) || defined(SCRIBE_LINUX)
#   if defined(_POSIX_TIMERS)
#       define SCRIBE_TIME_T struct timespec
#   else
#       define SCRIBE_TIME_T struct timeval
#   endif
#   define SCRIBE_PID_T pid_t
#endif

#endif

