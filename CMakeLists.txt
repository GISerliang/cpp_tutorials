#############################################################
# CMake settings
CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)

SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
# set path to additional CMake modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
# POLICIES
CMAKE_POLICY(SET CMP0048 NEW)
CMAKE_POLICY(SET CMP0053 NEW)

SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
SET(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
SET(CMAKE_RELWITHDEBINFO_POSTFIX "rd" CACHE STRING "add a postfix, usually empty on windows")
SET(CMAKE_MINSIZEREL_POSTFIX "s" CACHE STRING "add a postfix, usually empty on windows")

IF (CMAKE_BUILD_TYPE MATCHES "Release")
  SET(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/build/release)
ELSEIF (CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
  SET(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/build/minsizerel)
ELSEIF (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
  SET(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/build/relwithdebinfo)
ELSEIF (CMAKE_BUILD_TYPE MATCHES "Debug")
  SET(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/build/debug)
ENDIF ()

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# don't relink if only the shared object changes
SET(CMAKE_LINK_DEPENDS_NO_SHARED ON)
SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

SET(LIBRARY_TYPE SHARED)

#############################################################
# Project and version
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(COMPLETE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})

SET(PROJECT_NAME "cpp-tutorials")
#SET(APP_NAME "OPENGL-DEMO")
SET(RELEASE_NAME "dev")
PROJECT(${PROJECT_NAME} VERSION ${COMPLETE_VERSION} LANGUAGES C CXX)

# Note the version no is Mmmpp for Major/minor/patch, 0-padded, thus '10100' for 1.1.0
MATH(EXPR HNGIS_VERSION_INT "${CPACK_PACKAGE_VERSION_MAJOR}*10000+${CPACK_PACKAGE_VERSION_MINOR}*100+${CPACK_PACKAGE_VERSION_PATCH}")
MESSAGE(STATUS "${PROJECT_NAME} version: ${COMPLETE_VERSION} ${RELEASE_NAME} (${HNGIS_VERSION_INT})")

#############################################################
IF (MSVC)
  SET(_warnings "")
  #  IF (NOT USING_NMAKE AND NOT USING_NINJA)
  #    SET(_warnings "${_warnings} /W4" )
  #  ENDIF (NOT USING_NMAKE AND NOT USING_NINJA)

  # disable warnings
  SET(_warnings "${_warnings} /wd4003 ") # warning C4003: “max”宏的实参不足
  SET(_warnings "${_warnings} /wd4091 ")  # 'typedef': ignored on left of '' when no variable is declared (occurs in MS DbgHelp.h header)
  SET(_warnings "${_warnings} /wd4100 ")  # unused formal parameters
  SET(_warnings "${_warnings} /wd4127 ")  # constant conditional expressions (used in Qt template classes)
  SET(_warnings "${_warnings} /wd4190 ")  # 'identifier' has C-linkage specified, but returns UDT 'identifier2' which is incompatible with C
  SET(_warnings "${_warnings} /wd4231 ")  # nonstandard extension used : 'identifier' before template explicit instantiation (used in Qt template classes)
  SET(_warnings "${_warnings} /wd4244 ")  # conversion from '...' to '...' possible loss of data
  SET(_warnings "${_warnings} /wd4251 ")  # needs to have dll-interface to be used by clients of class (occurs in Qt template classes)
  SET(_warnings "${_warnings} /wd4275 ")  # non dll-interface class '...' used as base for dll-interface class '...'
  SET(_warnings "${_warnings} /wd4290 ")  # c++ exception specification ignored except to indicate a function is not __declspec(nothrow) (occurs in sip generated bindings)
  SET(_warnings "${_warnings} /wd4456 ")  # declaration of '...' hides previous local declaration
  SET(_warnings "${_warnings} /wd4457 ")  # declaration of '...' hides a function parameter
  SET(_warnings "${_warnings} /wd4458 ")  # declaration of '...' hides class member
  SET(_warnings "${_warnings} /wd4505 ")  # unreferenced local function has been removed (QgsRasterDataProvider::extent)
  SET(_warnings "${_warnings} /wd4510 ")  # default constructor could not be generated (sqlite3_index_info, QMap)
  SET(_warnings "${_warnings} /wd4512 ")  # assignment operator could not be generated (sqlite3_index_info)
  SET(_warnings "${_warnings} /wd4610 ")  # user defined constructor required (sqlite3_index_info)
  SET(_warnings "${_warnings} /wd4706 ")  # assignment within conditional expression (pal)
  SET(_warnings "${_warnings} /wd4714 ")  # function '...' marked as __forceinline not inlined (QString::toLower/toUpper/trimmed)
  SET(_warnings "${_warnings} /wd4800 ")  # 'int' : forcing value to bool 'true' or 'false' (performance warning)
  SET(_warnings "${_warnings} /wd4819 ")  # 请将该文件保存为 Unicode 格式以防止数据丢失
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_warnings}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_warnings}")
ELSE (MSVC)
  # add warnings via flags (not as definitions as on Mac -Wall can not be overridden per language )
  SET(_warnings "-Wall -Wextra -Wno-long-long -Wformat-security -Wno-strict-aliasing")
  SET(WERROR FALSE CACHE BOOL "Treat build warnings as errors.")
  IF (WERROR)
    SET(_warnings "${_warnings} -Werror")
  ENDIF (WERROR)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_warnings}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_warnings}")

  # Qt produces lots of warnings with strict aliasing (as of Qt 4.4.0 & GCC 4.3)
  # There are redundant declarations in Qt and GDAL
  # ADD_DEFINITIONS( -fstrict-aliasing -Wstrict-aliasing=1 -Wredundant-decls )

  IF ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-return-type-c-linkage -Wno-overloaded-virtual -Wimplicit-fallthrough")
  ENDIF ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")

  # add any extra CXXFLAGS flags set by user. can be -D CXX_EXTRA_FLAGS or environment variable
  # command line -D option overrides environment variable
  # e.g. useful for suppressing transient upstream warnings in dependencies, like Qt
  SET(CXX_EXTRA_FLAGS "" CACHE STRING "Additional appended CXXFLAGS")
  IF ("${CXX_EXTRA_FLAGS}" STREQUAL "" AND DEFINED $ENV{CXX_EXTRA_FLAGS})
    SET(CXX_EXTRA_FLAGS "$ENV{CXX_EXTRA_FLAGS}")
  ENDIF ("${CXX_EXTRA_FLAGS}" STREQUAL "" AND DEFINED $ENV{CXX_EXTRA_FLAGS})
  IF (NOT "${CXX_EXTRA_FLAGS}" STREQUAL "")
    MESSAGE(STATUS "Appending CXX_EXTRA_FLAGS")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_EXTRA_FLAGS}")
  ENDIF (NOT "${CXX_EXTRA_FLAGS}" STREQUAL "")
ENDIF (MSVC)

#############################################################
# enable use of c++11 features where available
# full c++11 support in clang 3.3+: http://clang.llvm.org/cxx_status.html
# for Mac, this is probably Apple LLVM 4.2 (based on LLVM 3.2svn, in XCode 4.6+)
#   or definitely Apple LLVM 5.0 (based on LLVM 3.3svn, in Xcode 5+):
#   https://gist.github.com/yamaya/2924292

SET(CMAKE_CXX_STANDARD 11)

#############################################################
ADD_SUBDIRECTORY(src)
#############################################################
