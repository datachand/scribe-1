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

#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

#define SCRIBE_VERSION_MAJOR "0"
#define SCRIBE_VERSION_MINOR "1"
#define SCRIBE_VERSION_PATCH "0"
#define SCRIBE_VERSION ""SCRIBE_VERSION_MAJOR"."SCRIBE_VERSION_MINOR"."SCRIBE_VERSION_PATCH""

#if defined(NDEBUG)
#   define SCRIBE_DEBUG 0
#else
#   define SCRIBE_DEBUG 1
#endif

#if !defined(SCRIBE_RWLOCK_DELAY)
#   define SCRIBE_RWLOCK_DELAY 250
#endif

#if !defined(SCRIBE_RINGBUFER_CAPACITY)
#   define SCRIBE_RINGBUFFER_CAPACITY 64
#endif

// OS specific configuration
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(_WIN64)
#   define SCRIBE_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
#   define SCRIBE_OSX
#elif defined(unix) || defined(__unix__) || defined(__unix)
#   define SCRIBE_UNIX
#elif defined(linux) || defined(__linux__) || defined(__linux)
#   define SCRIBE_LINUX
#else
#   error "Unknown OS, cannot decide which types to use!"
#endif

#if defined(SCRIBE_WINDOWS)
#   define SCRIBE_TIME_T SYSTEMTIME
#   define SCRIBE_PID_T int
#else
#   define SCRIBE_TIME_T struct timeval
#   define SCRIBE_PID_T pid_t
#endif

// GNU extension availability
#if defined(__gnuc__) || defined(__GNUC__) || defined(__GNUG__)
#   define SCRIBE_GNUC_AVAILABLE 1
#else
#   define SCRIBE_GNUC_AVAILABLE 0
#endif

#if !defined(SCRIBE_WINDOWS)
#include <unistd.h>
#   define SCRIBE_STDIN_FILENO STDIN_FILENO
#   define SCRIBE_STDOUT_FILENO STDOUT_FILENO
#   define SCRIBE_STDERR_FILENO STDERR_FILENO
#else
#   define SCRIBE_STDIN_FILENO 0
#   define SCRIBE_STDOUT_FILENO 1
#   define SCRIBE_STDERR_FILENO 3
#endif

// which compiler/language is being used
#if defined(__cplusplus)
#   define SCRB_LANGUAGE "C++"
#else
#   define SCRB_LANGUAGE "C"
#endif

#if defined(__clang__)
#   define SCRB_CC "clang/LLVM"
#   define SCRB_CC_VER __clang_version__
#elif defined(__ICC) || defined(__INTEL_COMPILER)
#   define SCRB_CC "Intel ICC"
#   define SCRB_CC_VER __VERSION__
#elif defined(__gnuc__) || defined(__GNUC__) || defined(__GNUG__)
#   define SCRB_CC "GNU GCC/G++"
#   define SCRB_CC_VER __VERSION__
#elif defined(__HP_cc) || defined(__HP_aCC)
#   define SCRB_CC "HP C/aC++"
#if defined(__cplusplus)
#   define SCRB_CC_VER __HP_aCC
#else
#   define SCRB_CC_VER __HP_cc
#endif
#elif defined(__IBMC__) || defined(__IBMCPP__)
#   define SCRB_CC "IBM XL C/C++"
#if defined(__cplusplus)
#   define SCRB_CC_VER __IBMCPP__
#else
#   define SCRB_CC_VER __xlc__
#endif
#elif defined(_MSC_VER)
#   define SCRB_CC "MS Visual Studio"
#if defined(_MSC_FULL_VER)
#   define SCRB_CC_VER _MSC_FULL_VER
#else
#   define SCRB_CC_VER _MSC_VER
#endif
#elif defined(__PGI)
#   define SCRB_CC "Portland Group PGCC/PGCPP"
#   define SCRB_CC_VER __PGIC__"."__PGIC_MINOR__"."__PGIC_PATCHLEVEL__
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
#   define SCRB_CC "Oracle Solaris Studio"
#if defined(__cplusplus)
#   define SCRB_CC_VER __SUNPRO_CC
#else
#   define SCRB_CC_VER __SUNPRO_C
#endif
#else
#   define SCRB_CC "unknown_cc"
#   define SCRB_CC_VER "unknown_cc_ver"
#endif

#endif

