#ifndef LOGGER_H
#define LOGGER_H

// std
#include <iostream>
#include <sstream>
#include <ostream>
#include <iosfwd>
#include <string>
#include <unordered_map>
#ifdef THREADSAFE
  #include <mutex>
  #include <atomic>
#endif

#include "noncopyable.h"
#include "singleton.h"

class Logger:
  public singleton<Logger, CreateStatic>,
  private noncopyable {
 public:
  enum severity_type {
    debug = 0,
    warn = 1,
    error = 2,
    info = 3
  };

  using severity_map = std::unordered_map<std::string, severity_type>;

  // global security level
  static severity_type get_severity() {
    return severity_level_;
  }

  static void set_severity ( severity_type severity_level ) {
    severity_level_ = severity_level;
  }

  // per object security levels
  static severity_type get_object_severity ( std::string const& object_name ) {
#ifdef THREADSAFE
    std::lock_guard<std::mutex> lock ( severity_mutex_ );
#endif
    severity_map::iterator it = object_severity_level_.find ( object_name );

    if ( object_name.empty() || it == object_severity_level_.end() ) {
      return severity_level_;
    } else {
      return it->second;
    }
  }

  static void set_object_severity ( std::string const& object_name,
                                    severity_type const& security_level ) {
    if ( !object_name.empty() ) {
#ifdef THREADSAFE
      std::lock_guard<std::mutex> lock ( severity_mutex_ );
#endif
      object_severity_level_[object_name] = security_level;
    }
  }

  static void clear_object_severity() {
#ifdef THREADSAFE
    std::lock_guard<std::mutex> lock ( severity_mutex_ );
#endif

    object_severity_level_.clear();
  }

  // format
  static std::string const& get_format() {
    return format_;
  }

  static void set_format ( std::string const& format ) {
#ifdef THREADSAFE
    std::lock_guard<std::mutex> lock ( format_mutex_ );
#endif
    format_ = format;
  }

  // interpolate the format string for output
  static std::string str();

  // output
  static void use_file ( std::string const& filepath );
  static void use_console();

 private:
  static severity_map object_severity_level_;
  static std::string format_;
  static std::ofstream file_output_;
  static std::string file_name_;
  static std::streambuf* saved_buf_;

#ifdef THREADSAFE
  static std::atomic<severity_type> severity_level_;
  static std::mutex severity_mutex_;
  static std::mutex format_mutex_;
#else
  static severity_type severity_level_;
#endif
};


template<class Ch, class Tr, class A>
class logger_sink {
 public:
  using stream_buffer = std::basic_ostringstream<Ch, Tr, A>;

  void operator() ( Logger::severity_type const& /*severity*/, stream_buffer const& s ) {
#ifdef THREADSAFE
    static std::mutex mutex;
    std::lock_guard<std::mutex> lock ( mutex );
#endif
    std::clog << Logger::str() << " " << s.str() << std::endl;
  }
};

template<template <class Ch, class Tr, class A> class OutputPolicy,
         Logger::severity_type severity_type_,
         class Ch = char,
         class Tr = std::char_traits<Ch>,
         class A = std::allocator<Ch> >
class base_log :
  public noncopyable {
 public:
  using output_policy = OutputPolicy<Ch, Tr, A>;

  base_log() {}

  base_log ( const char* object_name ) {
    if ( object_name != nullptr ) {
      object_name_ = object_name;
    }
  }

  ~base_log() {
    if ( check_severity() ) {
      output_policy() ( severity_type_, streambuf_ );
    }
  }

  template<class T>
  base_log& operator<< ( T const& x ) {

    streambuf_ << x;
    return *this;
  }

 private:
  inline bool check_severity() {
    return severity_type_ >= Logger::get_object_severity ( object_name_ );
  }

  typename output_policy::stream_buffer streambuf_;
  std::string object_name_;
};

using base_log_info = base_log<logger_sink, Logger::info>;
using base_log_debug = base_log<logger_sink, Logger::debug>;
using base_log_warn = base_log<logger_sink, Logger::warn>;
using base_log_error = base_log<logger_sink, Logger::error>;

// real interfaces
class info : public base_log_info {
 public:
  info() : base_log_info() {}
  info ( const char* object_name ) : base_log_info ( object_name ) {}
};
class warn : public base_log_warn {
 public:
  warn() : base_log_warn() {}
  warn ( const char* object_name ) : base_log_warn ( object_name ) {}
};

class debug : public base_log_debug {
 public:
  debug() : base_log_debug() {}
  debug ( const char* object_name ) : base_log_debug ( object_name ) {}
};

class error : public base_log_error {
 public:
  error() : base_log_error() {}
  error ( const char* object_name ) : base_log_error ( object_name ) {}
};

// logging helpers
#define LOGGER_INFO(s) info(#s)
#define LOGGER_DEBUG(s) debug(#s)
#define LOGGER_WARN(s) warn(#s)
#define LOGGER_ERROR(s) error(#s)

#endif // LOGGER_H
