SET(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR})

SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})

SET(APP_HDRS
  include/singleton.h
  )

include_directories(
	include/
)

ADD_EXECUTABLE(singleton_tutorials ${APP_HDRS} main.cpp)
