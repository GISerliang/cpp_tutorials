#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include <unordered_map>

#include "logger.h"

using namespace std;

int main() {
  Logger::instance().use_file ( "./log/demo.log" );
//  logger->use_console();
  Logger::instance().set_severity ( Logger::debug );


#ifdef THREADSAFE
  LOGGER_DEBUG ( "THREADSAFE" ) << "THREADSAFE";
#endif

  LOGGER_INFO ( "info" ) << "info";
  LOGGER_WARN ( "warn" ) << "warn";
  LOGGER_DEBUG ( "debug" ) << "debug";
  LOGGER_ERROR ( "error" ) << "error";

  std::cin.get();
  return 0;
}
