#include "logger.h"

#include <ctime>
#include <stdexcept>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <sstream>

#define LOGGER_FORMAT LOGGER> %Y-%m-%d %H:%M:%S:

#ifndef DEFAULT_LOG_SEVERITY
  #ifdef DEBUG
    #define DEFAULT_LOG_SEVERITY 0
  #else
    #define DEFAULT_LOG_SEVERITY 2
  #endif
#endif

#ifndef STRINGIFY
  #define STRINGIFY(n) STRINGIFY_HELPER(n)
  #define STRINGIFY_HELPER(n) #n
#endif

#ifdef THREADSAFE

std::mutex Logger::severity_mutex_;
std::mutex Logger::format_mutex_;

std::atomic<Logger::severity_type> Logger::severity_level_ {
#else

Logger::severity_type Logger::severity_level_ {
#endif
#if DEFAULT_LOG_SEVERITY == 0
  Logger::debug
#elif DEFAULT_LOG_SEVERITY == 1
  Logger::warn
#elif DEFAULT_LOG_SEVERITY == 2
  Logger::error
#elif DEFAULT_LOG_SEVERITY == 3
  Logger::info
#else
#error "Wrong default log severity level specified!"
#endif
};

Logger::severity_map Logger::object_severity_level_ = Logger::severity_map();

std::string Logger::format_ = STRINGIFY ( LOGGER_FORMAT );

std::string Logger::str() {
  char buf[256];
  const time_t tm = time ( 0 );
  std::strftime ( buf, sizeof ( buf ), Logger::format_.c_str(), localtime ( &tm ) );
  return buf;
}

// output
std::ofstream Logger::file_output_;
std::string Logger::file_name_;
std::streambuf* Logger::saved_buf_ = 0;

void Logger::use_file ( std::string const& filepath ) {
  // save clog rdbuf
  if ( saved_buf_ == 0 ) {
    saved_buf_ = std::clog.rdbuf();
  }

  // use a file to output as clog rdbuf
  if ( file_name_ != filepath ) {
    file_name_ = filepath;

    if ( file_output_.is_open() ) {
      file_output_.close();
    }

    file_output_.open ( file_name_.c_str(), std::ios::out | std::ios::app );

    if ( file_output_ ) {
      std::clog.rdbuf ( file_output_.rdbuf() );
    } else {
      std::stringstream s;
      s << "cannot redirect log to file " << file_name_;
      throw std::runtime_error ( s.str() );
    }
  }
}

void Logger::use_console() {
  // save clog rdbuf
  if ( saved_buf_ == 0 ) {
    saved_buf_ = std::clog.rdbuf();
  }

  // close the file to force a flush
  if ( file_output_.is_open() ) {
    file_output_.close();
  }

  std::clog.rdbuf ( saved_buf_ );
}
