SET(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR})

SET(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR})
include(GenerateExportHeader)

#############################################################
ADD_SUBDIRECTORY(coutcerr_tutorials)
ADD_SUBDIRECTORY(noncopyable_tutorials)
ADD_SUBDIRECTORY(singleton_tutorials)
ADD_SUBDIRECTORY(logger_tutorials)
